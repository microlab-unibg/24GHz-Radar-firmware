/**
    @file  datastore_data.c
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

#include "sense2gol_bsp_limits.h"
#include "datastore.h"
#include "config.h"

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

/*--------------------------------------------
 * NOTE: it is assumed that the storage and the consumption of the data is executed in the same execution context.
 * (=Thread or task) otherwise counter measures against data corruption need to be considered!
 */

/* this would be the data structure that is persistent in flash memory! (done via scatter load linker file, once implemented! */
algo_settings_t ds_default_algo_settings = {
    .struct_length              = sizeof(algo_settings_t),
    .struct_version             = DS_ALGO_SETTINGS_STRUCT_VERSION,
    .speed_detection_threshold  = SPEED_DETECTION_THRESHOLD,
    .min_speed_kmph             = (float)MIN_SPEED_KMPH / (float)SPEED_DIVIDER,
    .max_speed_kmph             = (float)MAX_SPEED_KMPH / (float)SPEED_DIVIDER,
    .use_high_gain_doppler      = GAIN_SELECTOR_DOPPLER,
    .motion_detection_threshold = MOTION_DETECTION_THRESHOLD,
};

const device_settings_t ds_default_device_settings = {
    .struct_length            = sizeof(device_settings_t),
    .struct_version           = DS_HW_SETTINGS_STRUCT_VERSION,

    /* PLL Settings */
    .pll_chirp_time_usec      = 1,                                  // 300U, keep for current communication with base endpoint!
    .rf_frequency_KHz         = BSP_DOPPLER_BASE_FREQ_MHZ * 1000.0, // defined in HW on PCB level by a resistor between Rtune and ground (16 kOhm), not possible to set in SW!
    .pll_modulation_mode      = 0,                                  /* 0: Constant frequency (doppler), 1: FMCW-modulation */

    /* ADC Settings */
    .adc_sampling_freq_Hz     = DOPPLER_SAMPLING_FREQ_HZ,
    .adc_resolution           = 12,
    .adc_use_post_calibration = 0,

    /* Frame Settings */
    .num_of_chirps_per_frame  = 1, // only one frame is supported, we don't have chirps
    .frame_period_usec        = FRAME_PERIOD_MSEC * 1000,
    .num_samples_per_chirp    = SAMPLES_PER_CHIRP,
    .rx_antenna_mask          = BSP_RX_ANTENNA_MASK,
    .num_sample_skip_count    = SAMPLE_SKIP_COUNT,

    .pulse_width_usec         = PULSE_WIDTH_USEC,         /* The On time of BGT VCC */
    .sample_hold_on_time_nsec = SAMPLE_HOLD_ON_TIME_NSEC, /* The start time of sample and hold: delay time after starting the BGT */
    .ptat_is_switched         = PTAT_SIGNAL_SWITCHED,
    .ptat_delay_usec          = PTAT_DELAY_USEC,
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

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
