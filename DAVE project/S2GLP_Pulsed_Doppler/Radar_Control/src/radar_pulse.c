/**
    @file  radar_pulse.c
*/

/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */


#include <stdint.h>

#include "Protocol.h"
#include "PayloadHelper.h"
#include "datastore.h"
#include "config.h"
#include "radar_api.h"
#include "radar_pulse.h"
#include "EndpointRadars2glpulse.h"
#include "datastore_eep.h"
#include "sense2gol_bsp_limits.h"
/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
 */

extern void send_result_response(uint8_t endpoint, pulse_result_response_t *p_pulse_result);

/*
==============================================================================
   4. DATA
==============================================================================
 */

// move the instantiation to the algorithm component!
const pulse_para_value_def_t c_para_value_def_list = {

	.max_speed_mps_lower_boundary             = ((float)BSP_MIN_SPEED_KMPH / (3.6f * SPEED_DIVIDER)),
	.max_speed_mps_upper_boundary             = ((float)BSP_MAX_SPEED_KMPH / (3.6f * SPEED_DIVIDER)),

	.min_speed_mps_lower_boundary             = ((float)BSP_MIN_SPEED_KMPH / (3.6f * SPEED_DIVIDER)),
	.min_speed_mps_upper_boundary             = ((float)BSP_MAX_SPEED_KMPH / (3.6f * SPEED_DIVIDER)),

	.frame_time_sec_lower_boundary            = 50.0f / 1000.0f,
	.frame_time_sec_upper_boundary            = (float)BSP_MAX_FRAME_PERIOD_MSEC / 1000.0f,

	.sample_list                              = {BSP_MIN_NUM_SAMPLES_PER_CHIRP, 64, 128, BSP_MAX_NUM_SAMPLES_PER_CHIRP},

	.sampling_freq_hz_lower_boundary          = (float)BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ,
	.sampling_freq_hz_upper_boundary          = (float)BSP_MAX_DOPPLER_SAMPLING_FREQ_HZ,

	.doppler_sensitivity_nu_lower_boundary    = (float)BSP_MIN_SPEED_DETECTION_THRESHOLD,
	.doppler_sensitivity_nu_upper_boundary    = (float)BSP_MAX_SPEED_DETECTION_THRESHOLD,

	.motion_sensitivity_nu_lower_boundary     = (float)BSP_MIN_MOTION_DETECTION_THRESHOLD,
	.motion_sensitivity_nu_upper_boundary     = (float)BSP_MAX_MOTION_DETECTION_THRESHOLD,

	.number_of_skip_samples_nu_lower_boundary = 0,
	.number_of_skip_samples_nu_upper_boundary = BSP_MAX_SAMPLE_SKIP_COUNT,

	.pulse_width_usec_lower_boundary          = BSP_MIN_PULSE_WIDTH_USEC,
	.pulse_width_usec_upper_boundary          = BSP_MAX_PULSE_WIDTH_USEC,
};

pulse_rf_shield_info_t rf_shield_info[] = {
	{
		.rf_frequency_khz        = 24050000,
		.major_rf_shield_version = 1,
		.minor_rf_shield_version = 0,
		.num_tx_antennas         = 1,
		.num_rx_antennas         = BSP_NUM_RX_ANTENNAS,
		.rf_shield_board_id      = "RBBPDV10",
		.description             = "Sense2GoL Pulse BGT24LTR11 shield V1.0",
		.modulation              = "Pulsed Doppler",
	},
	{
		.rf_frequency_khz        = 24050000,
		.major_rf_shield_version = 3,
		.minor_rf_shield_version = 0,
		.num_tx_antennas         = 1,
		.num_rx_antennas         = BSP_NUM_RX_ANTENNAS,
		.rf_shield_board_id      = RF_BOARD_ID,
		.description             = "Sense2GoL Pulse BGT24LTR11 shield V3.0",
		.modulation              = "Pulsed Doppler",
	},
};

static size_t result_repeat_count = 0;
static size_t raw_data_repeat_count = 0;
static uint8_t g_endpoint = 0;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

void send_parameter_unsolicited(void)
{
	pulse_para_values_t temp_para;
	copyStore2comPara(&temp_para);
	send_parameter_values(g_endpoint, &temp_para);
}

//============================================================================

void set_result_repeat_count(uint8_t endpoint, size_t count)
{
	result_repeat_count = count;
	g_endpoint = endpoint;
}

//============================================================================

void set_raw_data_repeat_count(uint8_t endpoint, size_t count)
{
	raw_data_repeat_count = count;
	g_endpoint = endpoint;
}

//============================================================================

void radar_pulse_raw_data(acq_buf_obj *p_acq_buf, device_settings_t *p_dev)
{
	if (raw_data_repeat_count > 0)
	{
		send_pulse_frame_data(g_endpoint, p_acq_buf, p_dev);
		raw_data_repeat_count -= 1;
	}
}

//============================================================================

void radar_pulse_results_updates(algo_result_t *p_algo_result)
{
	if (result_repeat_count > 0)
	{
		pulse_result_response_t pulse_result;

		pulse_result.frame_count             = p_algo_result->frame_counter;
		pulse_result.velocity_mps            = p_algo_result->velocity_kmph / 3.6f;
		pulse_result.level_nu                = p_algo_result->level;
		pulse_result.target_departing_bool   = p_algo_result->target_departing;
		pulse_result.target_approaching_bool = p_algo_result->target_approaching;
		pulse_result.motion_detected_bool    = p_algo_result->motion_detected;
		pulse_result.doppler_frequency_hz    = p_algo_result->doppler_frequency_hz;

		send_result_response(g_endpoint, &pulse_result);

		result_repeat_count -= 1;
	}
}

//============================================================================

void copyStore2comPara(pulse_para_values_t *p_com_para)
{
	algo_settings_t *p_algo = ds_algo_get_settings();
	device_settings_t *p_dev = ds_device_get_settings();

	p_com_para->max_speed_mps              = p_algo->max_speed_kmph / 3.6f;
	p_com_para->min_speed_mps              = p_algo->min_speed_kmph / 3.6f;

	p_com_para->frame_time_sec             = p_dev->frame_period_usec / 1000000.0f;
	p_com_para->number_of_samples_nu       = (uint16_t)p_dev->num_samples_per_chirp;

	p_com_para->sampling_freq_hz           = (float)p_dev->adc_sampling_freq_Hz;
	p_com_para->doppler_sensitivity_nu     = p_algo->speed_detection_threshold;
	p_com_para->motion_sensitivity_nu      = p_algo->motion_detection_threshold;

	p_com_para->use_high_gain_doppler_bool = (uint8_t)((p_algo->use_high_gain_doppler == 0L) ? 0U : 1U);

	uint32_t temp_frame_period_usec        = bsp_calc_min_frame_interval_usec(p_dev);

	p_com_para->continuous_mode_bool       = (temp_frame_period_usec == p_dev->frame_period_usec) ? 1 : 0;
	p_com_para->number_of_skip_samples_nu  = (uint16_t)p_dev->num_sample_skip_count;
	p_com_para->pulse_width_usec           = p_dev->pulse_width_usec;
}

//============================================================================

uint32_t copyComPara2Store(pulse_para_values_t *p_com_para)
{
	algo_settings_t algo;
	device_settings_t dev;

	algo_settings_t *p_algo  = ds_algo_get_settings();
	device_settings_t *p_dev = ds_device_get_settings();

	uint32_t error = 0;

	memcpy(&algo, p_algo, sizeof(algo));
	memcpy(&dev, p_dev, sizeof(dev));

	algo.max_speed_kmph             = p_com_para->max_speed_mps * 3.6f;
	algo.min_speed_kmph             = p_com_para->min_speed_mps * 3.6f;

	dev.num_samples_per_chirp       = (uint32_t)p_com_para->number_of_samples_nu;

	dev.adc_sampling_freq_Hz        = (uint32_t)p_com_para->sampling_freq_hz;
	algo.speed_detection_threshold  = p_com_para->doppler_sensitivity_nu;
	algo.motion_detection_threshold = p_com_para->motion_sensitivity_nu;
	algo.use_high_gain_doppler      = (uint32_t)((p_com_para->use_high_gain_doppler_bool == 0L) ? 0L : 1L);
	dev.num_sample_skip_count       = (uint32_t)p_com_para->number_of_skip_samples_nu;
	dev.pulse_width_usec            = p_com_para->pulse_width_usec;

	uint32_t temp_frame_period      = bsp_calc_min_frame_interval_usec(&dev);
	dev.frame_period_usec           = (uint32_t)((p_com_para->continuous_mode_bool == 1U) ? temp_frame_period : (p_com_para->frame_time_sec * 1000000.0f));
	if (dev.frame_period_usec < temp_frame_period)
		dev.frame_period_usec = temp_frame_period;

	ds_set_device_parameter_changed();

	error += ds_algo_check_settings(&algo);
	error += ds_dev_check_settings(&dev);

	if (error == 0)
	{
		memcpy(p_algo, &algo, sizeof(algo));
		memcpy(p_dev, &dev, sizeof(dev));

		return 1;
	}
	return 0;
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
