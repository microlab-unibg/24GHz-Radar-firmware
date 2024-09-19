/**
    @file: radar_pulse.h
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

#ifndef RADAR_PULSE_H_
#define RADAR_PULSE_H_

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include <stdint.h>
#include "ifxRadar.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define NB_OF_ELEMENTS(a)  (sizeof(a)/sizeof(a[0]))

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/* postfix "nu" indicate "no unit", postfix "bool" indicate that attribute is either false (==0) or true (!=0, non-zero)*/

typedef struct
{
	float max_speed_mps;
	float min_speed_mps;
	float frame_time_sec;
	uint16_t number_of_samples_nu;
	float sampling_freq_hz;
	float doppler_sensitivity_nu;
	float motion_sensitivity_nu;
	uint8_t use_high_gain_doppler_bool;
	uint8_t continuous_mode_bool;
	uint16_t number_of_skip_samples_nu;
	uint32_t pulse_width_usec;
} pulse_para_values_t;

typedef struct
{
	float max_speed_mps_lower_boundary;
	float max_speed_mps_upper_boundary;

	float min_speed_mps_lower_boundary;
	float min_speed_mps_upper_boundary;

	float frame_time_sec_lower_boundary;
	float frame_time_sec_upper_boundary;

	uint16_t sample_list[4]; //could be adjusted, the protocol can handle variable entries!

	float sampling_freq_hz_lower_boundary;
	float sampling_freq_hz_upper_boundary;

	float doppler_sensitivity_nu_lower_boundary;
	float doppler_sensitivity_nu_upper_boundary;

	float motion_sensitivity_nu_lower_boundary;
	float motion_sensitivity_nu_upper_boundary;

	uint32_t number_of_skip_samples_nu_lower_boundary;
	uint32_t number_of_skip_samples_nu_upper_boundary;

	uint32_t pulse_width_usec_lower_boundary;
	uint32_t pulse_width_usec_upper_boundary;

} pulse_para_value_def_t;

typedef struct
{
	uint32_t frame_count;
	float velocity_mps;				 /**< velocity of target detected in units of km/h (with sign!) */
	float level_nu;					 /**< Absolute magnitude FFT spectrum values calculated by Doppler algorithm */
	uint8_t target_departing_bool;	 /**< set to 1 if target is departing */
	uint8_t target_approaching_bool; /**< set to 1 if target is approaching */
	uint8_t motion_detected_bool;	 /**< set to 1 if motion is detected */
	float doppler_frequency_hz;		 /**< doppler frequency, detected by algorithm */
} pulse_result_response_t;

typedef struct
{
	uint32_t rf_frequency_khz;
	uint8_t major_rf_shield_version;
	uint8_t minor_rf_shield_version;
	uint8_t num_tx_antennas;
	uint8_t num_rx_antennas;
	char *rf_shield_board_id;
	char *description;
	char *modulation;
} pulse_rf_shield_info_t;

/*
==============================================================================
   4. DATA
==============================================================================
 */

extern const pulse_para_value_def_t c_para_value_def_list;
extern pulse_rf_shield_info_t rf_shield_info[2];

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

uint32_t copyComPara2Store(pulse_para_values_t *p_com_para);

void copyStore2comPara(pulse_para_values_t *p_com_para);

void radar_pulse_raw_data(acq_buf_obj *p_acq_buf, device_settings_t *p_dev);

void radar_pulse_results_updates(algo_result_t *p_algo_result);

void set_result_repeat_count(uint8_t endpoint, size_t count);

void set_raw_data_repeat_count(uint8_t endpoint, size_t count);

void send_parameter_unsolicited(void);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RADAR_PULSE_H_ */

/* --- End of File -------------------------------------------------------- */
