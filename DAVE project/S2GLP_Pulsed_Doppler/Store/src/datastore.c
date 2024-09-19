/**
    @file  datastore.c
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
#include "datastore.h"
#include "datastore_eep.h"
#include "datastore_internal.h"
#include "radar_device_status.h"
#if defined(BUILD_FOR_ARDUINO)
#include "..\src\arduino_adapt_internal.h"
#endif

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

#define RF_SHIELD_UNSUPPORTED 		0x3000
#define RF_SHIELD_INVALID 			0x3001
#define RF_SHIELD_NOT_PROGRAMMED 	0x30002

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

static algo_settings_t ds_algo_settings;

static device_settings_t ds_shadow_device_settings;

/* The new device settings will be applied for the next frame  */
static device_settings_t ds_device_settings;

static acq_buf_obj s_acq_buf = { .status = not_initialized } ;

static algo_result_t *p_ds_algo_results;

static uint32_t device_parameter_changed = 0;

static uint32_t def_settings_changed = 0;

static uint32_t detected_board_variant = 0;

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

void ds_set_def_settings_changed(void)
{
	def_settings_changed = 1;
}

//============================================================================

void ds_clear_def_settings_changed(void)
{
	def_settings_changed = 0;
}

//============================================================================

uint32_t ds_is_def_settings_changed(void)
{
	return def_settings_changed;
}

uint32_t ds_get_board_variant( void )
{
	return detected_board_variant;
}

//============================================================================

uint32_t calc_settings_crc(void *p_settings, uint32_t struct_size)
{
	uint32_t crc32 = 0;
	CRC_SW_CalculateCRC(&CRC_SW_EEPROM, p_settings, struct_size);
	crc32 = CRC_SW_GetCRCResult(&CRC_SW_EEPROM);
	return crc32;
}

//============================================================================

/**
 * return 0, if no changed, 1 if default settings has been changed
 */
uint32_t ds_default_settings_check(void)
{
	uint32_t retval = 0;
	EEPROM_STATUS_t eep_status = EEPROM_STATUS_SUCCESS;
	uint32_t def_dev_setting_crc = 0;
	uint32_t def_algo_setting_crc = 0;

	eep_default_settings_t eep_def_settings;

	def_dev_setting_crc = calc_settings_crc((void *)&ds_default_device_settings, sizeof(ds_default_device_settings));
	def_algo_setting_crc = calc_settings_crc((void *)&ds_default_algo_settings, sizeof(ds_default_algo_settings));

	eep_status = ds_eeprom_read_settings(&eep_def_settings, sizeof(eep_def_settings), DS_EEPROM_ADDRESS_DEFAULT_SETTING);

	if (eep_status == EEPROM_STATUS_SUCCESS)
	{
		if (def_dev_setting_crc 	!= eep_def_settings.def_dev_settings_crc ||
			def_algo_setting_crc 	!= eep_def_settings.def_dev_settings_crc)
		{
			/* set the flag */
			ds_set_def_settings_changed();

			/* write changed def settings check struct to EEPROM */
			eep_def_settings.def_dev_settings_crc 	= def_dev_setting_crc;
			eep_def_settings.def_algo_settings_crc 	= def_algo_setting_crc;
			eep_status = ds_eeprom_write_settings(&eep_def_settings, sizeof(eep_def_settings), DS_EEPROM_ADDRESS_DEFAULT_SETTING);
			retval = 1;
		}
	}
	else if (eep_status == EEPROM_STATUS_CRC_MISMATCH)
	{
		/* if we are here that means the CRC value of eep_default_settings_t mismatched,
		 * that means also that this device has no eep_default_settings_t
		 * so we need to write initial def settings check struct to EEPROM */

		eep_def_settings.def_dev_settings_crc 	= def_dev_setting_crc;
		eep_def_settings.def_algo_settings_crc 	= def_algo_setting_crc;
		eep_status = ds_eeprom_write_settings(&eep_def_settings, sizeof(eep_def_settings), DS_EEPROM_ADDRESS_DEFAULT_SETTING);
		retval = 1;
	}
	else
	{
		retval = -1;
	}
	return retval;
}

//============================================================================

void ds_store_algo_result_ptr(algo_result_t *p_result)
{
	p_ds_algo_results = p_result;
}

algo_result_t *ds_get_algo_result_ptr(void)
{
	return p_ds_algo_results;
}

//============================================================================

void ds_algo_store_settings(algo_settings_t *p_algo_settings)
{
  memcpy(&ds_algo_settings, p_algo_settings, sizeof(ds_algo_settings));

}

//============================================================================

void ds_algo_fetch_settings(algo_settings_t *p_algo_settings)
{
  /* the data structures are aligned, so no need to do a assignment by member */
  memcpy(p_algo_settings, &ds_algo_settings, sizeof(ds_algo_settings));
}

//============================================================================

algo_settings_t *ds_algo_get_settings(void)
{
  return &ds_algo_settings;
}

//============================================================================

/* if staged Algo settings are with-in range 0 is return, otherwise non-zero
 * the return value could be interpreted as bit field to identify the failure, if needed!
 */
uint32_t ds_algo_check_settings(algo_settings_t *ptr)
{
  uint32_t retval = 0;

  if (!((ptr->speed_detection_threshold >= BSP_MIN_SPEED_DETECTION_THRESHOLD) && (ptr->speed_detection_threshold <= BSP_MAX_SPEED_DETECTION_THRESHOLD)))
  {
    retval += 1;
  }
  if (!(ptr->max_speed_kmph > ptr->min_speed_kmph) && (ptr->max_speed_kmph <= BSP_MAX_SPEED_KMPH))
  {
    retval += 2;
  }
  if (ptr->min_speed_kmph > ptr->max_speed_kmph)
  {
    retval += 4;
  }
  if (!(ptr->motion_detection_threshold >= BSP_MIN_MOTION_DETECTION_THRESHOLD) && (ptr->motion_detection_threshold <= BSP_MAX_MOTION_DETECTION_THRESHOLD))
  {
	 retval += 8;
  }
  
  return retval;
}

//============================================================================

/* if staged Algo settings are with-in range 0 is return, otherwise non-zero
 * the return value could be interpreted as bit field to identify the failure, if needed!
 */
uint32_t ds_dev_check_settings(device_settings_t *ptr)
{
  uint32_t retval = 0;

  if (!((ptr->num_sample_skip_count >= BSP_MIN_SAMPLE_SKIP_COUNT) && (ptr->num_sample_skip_count <= BSP_MAX_SAMPLE_SKIP_COUNT)))
  {
    retval += 1;
  }
  if (!(ptr->adc_sampling_freq_Hz >= BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ && (ptr->adc_sampling_freq_Hz <= BSP_MAX_DOPPLER_SAMPLING_FREQ_HZ)))
  {
    retval += 2;
  }
  if (!(ptr->frame_period_usec >= bsp_calc_min_frame_interval_usec(ptr)) && (ptr->frame_period_usec*1000 <= BSP_MAX_FRAME_PERIOD_MSEC))
  {
    retval += 4;
  }
  if (!(ptr->num_samples_per_chirp >= BSP_MIN_NUM_SAMPLES_PER_CHIRP) && (ptr->num_samples_per_chirp <= BSP_MAX_NUM_SAMPLES_PER_CHIRP))
  {
	 retval += 8;
  }
  if (ptr->adc_resolution != 12)
  {
	  retval += 16;
  }
  return retval;
}

//============================================================================

device_settings_t *ds_device_get_settings(void)
{
  return &ds_device_settings;
}

//============================================================================

device_settings_t *ds_device_get_shadow_settings(void)
{
  return &ds_shadow_device_settings;
}

//============================================================================

/* handling for staged setting */
uint32_t ds_is_device_settings_updated(void)
{
  int32_t retval = 0;
  
  /* if both structs are identical, retval is 0, otherwise != 0 */
  retval = memcmp(&ds_shadow_device_settings,&ds_device_settings, sizeof(ds_shadow_device_settings));
  
  // result of memcmp could be negative!
  return (retval == 0) ? 0U : 1U;
}

//============================================================================

void ds_device_settings_copy_to_shadow(void)
{
  memcpy(&ds_shadow_device_settings, &ds_device_settings, sizeof(ds_shadow_device_settings));
}

//============================================================================

void ds_device_settings_revert_from_shadow(void)
{
  memcpy(&ds_device_settings, &ds_shadow_device_settings,  sizeof(ds_shadow_device_settings));
}

//============================================================================

static char rf_board_id_1[] = RF_BOARD_ID;
static char rf_board_id_2[] = "RBBPDV10";  // Check for previous compatible board-id

uint32_t ds_init(void)
{
  /* this function could be used to pre-init the settings store, e.g. with persistent value, that are stored in some persistent memory region */
  uint32_t retval = 0;
  uint32_t board_variant = 2;

#ifdef DS_USE_EEPROM
  RF_SHIELD_STATUS_t shield_status = RF_SHIELD_BOARD_NOT_CONNECTED;
  EEPROM_STATUS_t ret_status = EEPROM_STATUS_FAILURE;

  /* Read RF board ID, check if a valid board ID is written in EEPROM */
  shield_status = ds_eeprom_read_board_id(rf_board_id_1, sizeof(rf_board_id_1));
  if (shield_status == RF_SHIELD_BOARD_UNSUPPORTED)
  {
	  /* Read RF board ID, check if a valid board ID is written in EEPROM */
	  shield_status = ds_eeprom_read_board_id(rf_board_id_2, sizeof(rf_board_id_2));
	  if (shield_status == RF_SHIELD_BOARD_SUPPORTED)
		  board_variant = 1;
  }

  if (shield_status != RF_SHIELD_BOARD_SUPPORTED)
  {
	/* Unsupported RF board !!!!!! */
#if !defined(BUILD_FOR_ARDUINO)
	bsp_led_green_off();
    bsp_led_blue_off();
	bsp_led_red_off();

	/*  TODO: Remove before the release!! */
	if (shield_status == RF_SHIELD_BOARD_NOT_PROGRAMMED)
	{
		//static uint8_t ds_eeprom_write_inital_board_id(void);
		if (ds_eeprom_write_inital_board_id() == EEPROM_STATUS_SUCCESS)
		{
			bsp_led_blue_on();
			
			TIMER_SetTimeInterval(&TIMER_ERROR_TOGGLE, 200 * 1000U * 100U);
			bsp_timer_start(&TIMER_ERROR_TOGGLE);
	    	device_status_set_error(FATAL_ERROR_BOARD_NOT_PROGRAMMED);
			return RF_SHIELD_NOT_PROGRAMMED;
		}
	}
    else if (shield_status == RF_SHIELD_BOARD_UNSUPPORTED)
      {
        /* RF shield board is not compatible and is not supported in the current Firmware.
    	   Shield programmed with the wrong ID, For example RBBxxxxxx */

		TIMER_SetTimeInterval(&TIMER_ERROR_TOGGLE, 400 * 1000U * 100U);
		bsp_timer_start(&TIMER_ERROR_TOGGLE);
    	device_status_set_error(FATAL_ERROR_BOARD_NOT_SUPPORTED);
    	return RF_SHIELD_UNSUPPORTED;
      }
      else
      {
        /* Invalid RF shield board:
    	   1- RF shield is not programmed or it contains invalid data
    	   2- Or RF shield board is not connected */
    	TIMER_SetTimeInterval(&TIMER_ERROR_TOGGLE, 100 * 1000U * 100U);
    	bsp_timer_start(&TIMER_ERROR_TOGGLE);

  	    device_status_set_error(FATAL_ERROR_INVALID_RF_SHIELD);
  	    return RF_SHIELD_INVALID;
      }
	
#else
	/* TODO: need error define here! */
	call_user_error_handler(0xfe00 + (uint32_t) shield_status);
	return 0xfe00 + (uint32_t shield_status;
#endif
  }
  /* 1.1 Valid board ID, check if default settings in FW has changed, reset if FW default settings are changed*/

  uint32_t def_settings_check = ds_default_settings_check();

  if (def_settings_check == 1)
  {
	  ds_hw_para_factory_reset();
	  ds_algo_para_factory_reset();
  }

  /* 2- Valid board ID, Read hardware settings from EEPROM */
  ret_status = EEPROM_STATUS_FAILURE;
  ret_status = ds_eeprom_read_settings(&ds_device_settings, sizeof(device_settings_t), DS_EEPROM_ADDRESS_HW_SETTING);

  /* 3- Check if a valid hardware settings are written in EEPROM */
  if ((ret_status != EEPROM_STATUS_SUCCESS) ||
	  (ds_device_settings.struct_version != DS_HW_SETTINGS_STRUCT_VERSION) ||
	  (ds_device_settings.struct_length != sizeof(device_settings_t)))
  {
	/* 4- Invalid hardware settings : */
    /*    Copy default hardware settings in the EEPROM at this address DS_EEPROM_ADDRESS_HW_SETTING */
	ds_hw_para_factory_reset();
  }

  detected_board_variant = board_variant;
  /* 4- Valid hardware settings: Use new settings from EEPROM */
  ds_device_settings_copy_to_shadow();

  /* 5- Check if a valid algorithm settings are written in EEPROM */
  ret_status = EEPROM_STATUS_FAILURE;
  ret_status = ds_eeprom_read_settings(&ds_algo_settings, sizeof(algo_settings_t), DS_EEPROM_ADDRESS_ALGO_SETTING);

  if ((ret_status != EEPROM_STATUS_SUCCESS) ||
	  (ds_algo_settings.struct_version != DS_ALGO_SETTINGS_STRUCT_VERSION) ||
	  (ds_algo_settings.struct_length != sizeof(algo_settings_t)))
  {
	/* 6- Invalid algorithm settings : */
    /*    Copy default algorithm settings in the EEPROM at this address DS_ALGO_SETTINGS_STRUCT_VERSION */
	ds_algo_para_factory_reset();
  }

  /* 6- Valid algorithm settings: Use new settings from EEPROM */
  retval += ds_algo_check_settings(&ds_algo_settings);
  retval += ds_dev_check_settings(&ds_device_settings);

#else
  memcpy(&ds_device_settings, &ds_default_device_settings, sizeof(device_settings_t));
  ds_device_settings_copy_to_shadow();

  memcpy(&ds_algo_settings, &ds_default_algo_settings, sizeof(algo_settings_t));
  retval += ds_algo_check_settings(&ds_algo_settings);
#endif /* DS_USE_EEPROM */
  
  return retval;
}

//============================================================================

acq_buf_obj *ds_get_active_acq_buf(void)
{
  return &s_acq_buf;
}

//============================================================================

void ds_set_active_acq_buf(const acq_buf_obj acq_buf)
{
  if (s_acq_buf.p_acq_buf != NULL)
  {
    delete_acq_buffer_obj(&s_acq_buf);
  }
  
  memcpy(&s_acq_buf, &acq_buf, sizeof(acq_buf_obj));
}

//============================================================================

/* Function to copy default algorithm settings into EEPROM */
void ds_algo_para_factory_reset(void)
{
#ifdef DS_USE_EEPROM
	ds_algo_settings = ds_default_algo_settings;
	if (ds_eeprom_write_settings(&ds_default_algo_settings, sizeof(algo_settings_t), DS_EEPROM_ADDRESS_ALGO_SETTING) != EEPROM_STATUS_SUCCESS)
	{
		XMC_DEBUG("writing EEPROM failed!\n");
		while (1);
	}
#endif /* DS_USE_EEPROM */
}

//============================================================================

/* Function to copy current algorithm settings into EEPROM */
void ds_algo_para_store_persistent(void)
{
#ifdef DS_USE_EEPROM
	algo_settings_t *pDev = ds_algo_get_settings();
	if (ds_eeprom_write_settings(pDev, sizeof(algo_settings_t), DS_EEPROM_ADDRESS_ALGO_SETTING) != EEPROM_STATUS_SUCCESS)
	{
		XMC_DEBUG("writing EEPROM failed!\n");
		while (1);
	}
#endif /* DS_USE_EEPROM */
}

//============================================================================

/* Function to copy default hardware settings into EEPROM */
void ds_hw_para_factory_reset(void)
{
#ifdef DS_USE_EEPROM
	ds_device_settings = ds_default_device_settings;
	if (ds_eeprom_write_settings(&ds_default_device_settings, sizeof(device_settings_t), DS_EEPROM_ADDRESS_HW_SETTING) != EEPROM_STATUS_SUCCESS)
	{
		XMC_DEBUG("writing EEPROM failed!\n");
		while (1);
	}
#endif /* DS_USE_EEPROM */
	ds_set_device_parameter_changed();
}

//============================================================================

/* Function to copy current hardware settings into EEPROM */
void ds_hw_para_store_persistent(void)
{
#ifdef DS_USE_EEPROM
	device_settings_t *pDev = ds_device_get_settings();
	if (ds_eeprom_write_settings(pDev, sizeof(device_settings_t), DS_EEPROM_ADDRESS_HW_SETTING) != EEPROM_STATUS_SUCCESS)
	{
		XMC_DEBUG("writing EEPROM failed!\n");
		while (1);
	}
#endif /* DS_USE_EEPROM */
}

//============================================================================

uint32_t ds_current_device_settings_is_default(void)
{
	if (memcmp(&ds_default_device_settings, &ds_device_settings, sizeof(ds_default_device_settings)) == 0)
		return 1;

	return 0;
}

//============================================================================

uint32_t ds_current_algo_settings_is_default(void)
{
	if (memcmp(&ds_default_algo_settings, &ds_algo_settings, sizeof(ds_default_algo_settings)) == 0)
		return 1;

	return 0;
}

//============================================================================

void ds_set_device_parameter_changed(void)
{
	device_parameter_changed = 1;
}

//============================================================================

void ds_clear_device_parameter_changed(void)
{
	device_parameter_changed = 0;
}

//============================================================================

uint32_t ds_is_device_parameter_changed(void)
{
	return device_parameter_changed;
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
