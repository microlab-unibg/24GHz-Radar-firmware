/**
    @file  ds_radar_interface.c

  	@brief This file includes the function to access data structure from data store.
*/

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
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

#include "sense2gol_bsp.h"
#include "sense2gol_bsp_limits.h"
#include "ds_radar_interface.h"
#include "version.h"
#include "datastore.h"

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

/*
==============================================================================
   4. DATA
==============================================================================
 */

/* ***************** Get data structures from datastore ******************* */
/* Global constant structure to store the device info, this only holds compile time defines, so no need to store it in RAM! */
const Device_Info_t cg_device_info_struct = {
	.description = DEMO_DESCRIPTION,
	.major_version_hw = HW_VERSION_MAJOR,
	.minor_version_hw = HW_VERSION_MINOR,
	.num_tx_antennas = BSP_NUM_TX_ANTENNAS,
	.num_rx_antennas = BSP_NUM_RX_ANTENNAS,
	.num_temp_sensors = BSP_NUM_TEMP_SENSORS,
	.max_tx_power = BSP_MAX_TX_POWER_LEVEL,
	.min_rf_frequency_kHz = BSP_MIN_RF_FREQUENCY_KHZ,
	.max_rf_frequency_kHz = BSP_MAX_RF_FREQUENCY_KHZ,
	.interleaved_rx = 0,
	.data_format = RADAR_RX_DATA_COMPLEX
	};

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

void ds_ep_fetch_adc_config(Adc_Configuration_t *ptr)
{
	device_settings_t *p_dev  = ds_device_get_settings();
	ptr->samplerate_Hz        = p_dev->adc_sampling_freq_Hz;
	ptr->resolution           = p_dev->adc_resolution;
	ptr->use_post_calibration = p_dev->adc_use_post_calibration;
}

//============================================================================

uint16_t ds_ep_store_adc_config(const Adc_Configuration_t *p_config)
{
	uint16_t status = RADAR_ERR_BUSY;

	if (p_config != NULL)
	{
		device_settings_t *p_dev        = ds_device_get_settings();
		p_dev->adc_sampling_freq_Hz     = p_config->samplerate_Hz;
		p_dev->adc_resolution           = p_config->resolution;
		p_dev->adc_use_post_calibration = p_config->use_post_calibration;

		status = RADAR_ERR_OK;
	}
	return (status);
}

//============================================================================

const Device_Info_t *ds_ep_get_device_info(void)
{
	return (&cg_device_info_struct);
}

//============================================================================

void ds_ep_fetch_frame_format(Frame_Format_t *p_frame_format)
{
	device_settings_t *p_dev              = ds_device_get_settings();
	p_frame_format->num_samples_per_chirp = p_dev->num_samples_per_chirp;
	p_frame_format->num_chirps_per_frame  = p_dev->num_of_chirps_per_frame;
	p_frame_format->rx_mask               = p_dev->rx_antenna_mask;
	p_frame_format->signal_part           = RADAR_SIGNAL_I_AND_Q;
}

//============================================================================

uint16_t ds_ep_store_frame_format(const Frame_Format_t *p_new_frame_format)
{
	uint16_t status = RADAR_ERR_BUSY;

	if (p_new_frame_format != NULL)
	{
		if (p_new_frame_format->signal_part != RADAR_SIGNAL_I_AND_Q)
		{
			status = RADAR_ERR_UNAVAILABLE_SIGNAL_PART;
		}
		else
		{
			device_settings_t *p_dev       = ds_device_get_settings();
			p_dev->num_samples_per_chirp   = p_new_frame_format->num_samples_per_chirp;
			p_dev->num_of_chirps_per_frame = p_new_frame_format->num_chirps_per_frame;
			p_dev->rx_antenna_mask         = p_new_frame_format->rx_mask;

			status = RADAR_ERR_OK;
		}
	}
	return (status);
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
