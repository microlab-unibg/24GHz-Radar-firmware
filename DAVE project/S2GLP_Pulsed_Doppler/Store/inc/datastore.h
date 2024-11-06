/**
    @file  datastore.h
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

#ifndef DATASTORE_H_
#define DATASTORE_H_

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
#include "acq_buffer.h"


/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(test_for_true) _Static_assert((test_for_true), "(" #test_for_true ") failed")
#endif

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

typedef struct default_settings_check
{
	uint8_t def_dev_settings_crc;
	uint8_t def_algo_settings_crc;
}eep_default_settings_t;

/*
 *  Hardware configuration structure
 */
typedef struct device_settings_TAG
{
  uint32_t        struct_length;  /* length of this structure */
  uint32_t        struct_version; /* Hardware structure version */

  /* Frequency Settings */
  uint32_t        pll_chirp_time_usec;    //keep for current communication with base endpoint!
  float           rf_frequency_KHz;
  uint32_t        pll_modulation_mode; /* 0: Constant frequency (doppler), 1: FMCW-modulation */
  
  /* ADC Settings */
  uint32_t        adc_sampling_freq_Hz;
  uint8_t         adc_resolution;
  uint8_t         adc_use_post_calibration;

  /* Frame Settings */
  uint32_t        num_of_chirps_per_frame;  /* For Doppler it is set to 1 */
  uint32_t        frame_period_usec;
  uint32_t        num_samples_per_chirp;    /* For Doppler it is the number of samples per frame */
  uint8_t         rx_antenna_mask;
  uint32_t 	      num_sample_skip_count;	/* The skip count that is applied, before actual samples are taken */

  /* Pulse mode settings */
  uint32_t 		  pulse_width_usec;		 /* The On time of BGT VCC */
  uint32_t        sample_hold_on_time_nsec;		 /* The start time of sample and hold: delay time after starting the BGT */
  uint32_t		  ptat_is_switched;
  uint32_t		  ptat_delay_usec;

} device_settings_t;

#define     DS_HW_SETTINGS_STRUCT_VERSION        200222U      /* device settings structure version --> date of last change */

typedef struct
{
  float     velocity_kmph;     						/**< velocity of target detected in units of km/h (with sign!) */
  float     level;  							/**< Absolute magnitude FFT spectrum values calculated by Doppler algorithm */
  uint8_t 	target_departing;   				/**< set to 1 if target is departing */
  uint8_t 	target_approaching; 				/**< set to 1 if target is approaching */
  uint8_t   motion_detected;					/**< set to 1 if motion is detected */
  uint8_t   breath_detected;              /**< set to 1 if breath is detected */
  uint8_t   hearthbeat_detected;          /**< set to 1 if motion is detected */
  float		doppler_frequency_hz;  				/**< doppler frequency, detected by algorithm */
  float     *p_doppler_spectrum; 				/**< pointer to doppler spectrum */
  uint32_t  doppler_spectrum_elems;				/**< number of spectrum elements */
  uint32_t  frame_counter;
} algo_result_t;

/*
 * Algorithm settings structure
 */
typedef struct algo_settings_TAG
{
  uint32_t struct_length;
  uint32_t struct_version;
  float    speed_detection_threshold;
  float    min_speed_kmph;
  float    max_speed_kmph;
  float    min_breath;
  float    max_breath;
  float    min_hearthbeat;
  float    max_hearthbeat;
  uint32_t use_high_gain_doppler;
  float    motion_detection_threshold;
} algo_settings_t;

#define     DS_ALGO_SETTINGS_STRUCT_VERSION      190918U      /* algo settings structure version --> date of last change */

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

void ds_algo_store_settings(algo_settings_t *p_algo_settings);

void ds_algo_fetch_settings(algo_settings_t *p_algo_settings);

uint8_t ds_algo_apply_staged_settings(void);

algo_settings_t *ds_algo_get_settings(void);

uint32_t ds_algo_check_settings(algo_settings_t *ptr);

uint32_t ds_dev_check_settings(device_settings_t *ptr);

uint32_t ds_init(void);

device_settings_t *ds_device_get_settings(void);

device_settings_t *ds_device_get_shadow_settings(void);

uint8_t ds_device_apply_staged_settings(void);

uint32_t ds_is_device_settings_updated(void);

void ds_device_settings_copy_to_shadow(void);

void ds_device_settings_revert_from_shadow(void);

acq_buf_obj *ds_get_active_acq_buf(void);

void ds_set_active_acq_buf(const acq_buf_obj acq_buf);

/*
 * setter and getter for algo results
 */
void ds_store_algo_result_ptr(algo_result_t *p_result);

algo_result_t *ds_get_algo_result_ptr(void);

uint32_t ds_get_board_variant( void );

/*
 * trigger a factory reset of the parameters
 */
void ds_hw_para_factory_reset(void);

void ds_hw_para_store_persistent(void);

void ds_algo_para_factory_reset(void);

void ds_algo_para_store_persistent(void);

uint32_t ds_current_device_settings_is_default(void);

uint32_t ds_current_algo_settings_is_default(void);

void ds_set_device_parameter_changed(void);

void ds_clear_device_parameter_changed(void);

uint32_t ds_is_device_parameter_changed(void);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DATASTORE_H_ */

/* --- End of File -------------------------------------------------------- */
