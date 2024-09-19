/**
    @file: config.h

    @brief: Configuration file for Sense2GoL Pulse project.
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

#ifndef CONFIG_H_
#define CONFIG_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

//============================= HW VERSTIONNING =============================//

#define HW_VERSION_MAJOR                  2U       /**< Hardware major version */
#define HW_VERSION_MINOR                  0U       /**< Hardware minor version */

//============================= GENERAL CONFIG =============================//

#define NUM_OF_CHIRPS                     (1U)     /**< Valid range of chirps in relation to the SAMPLES_PER_CHIRP, [1 - 16] */

#define SAMPLES_PER_CHIRP                 (128U)   /**< Number of samples per chirp/frame */ //S2GL

#define FRAME_PERIOD_MSEC                 (150U)   /**< Time period of one frame to capture data */

#define SAMPLE_SKIP_COUNT                 (40U)    /**< Number of samples skipped for signal processing at beginning of frame */

//========================== DOPPLER CONFIG ================================//

#define DOPPLER_SUPPORTED                          /**< Uncomment this macro, to disable Doppler support */

#define DOPPLER_SAMPLING_FREQ_HZ          (2000U)  /**< Sampling frequency for Doppler (units in Hz) */ //S2GL

#define MIN_SPEED_KMPH                    (72)     /**< Used in Doppler to exclude targets below this speed (units in km/h) divided by SPEED_DIVIDER */

#define MAX_SPEED_KMPH                    (1080)   /**< Used in Doppler to exclude targets above this speed (units in km/h) */

#define SPEED_DIVIDER                     (100)    /**< For software implementation purpose only to keep min. and max. speed within limits of 0 to 10km/h */

#define SPEED_DETECTION_THRESHOLD         (50U)    /**< FFT spectrum threshold to detect a target in Doppler */

#define MOTION_DETECTION_THRESHOLD        (10U)    /**< FFT spectrum threshold to detect a target in Doppler */

#define GAIN_SELECTOR_DOPPLER             (1U)     /**< Baseband gain selection 1 for high gain, 0 for low gain  */

#define GUARD_FRAME_TIME_USEC             (500U)   /**< Guard period, until the min and request frame time must match */

//============================= LOW LEVEL PULSED DOPPLER CONFIG =============================//

#define PULSE_WIDTH_USEC                  (5U)     /**< On time for the Gate of BGT VCC input, ((1% * FRAME_PERIOD_USEC)/100) */

#define SAMPLE_HOLD_ON_TIME_NSEC          (0U)     /**< Hold time for acquiring raw data (units in ns), max = 4520*/

#define SAMPLE_HOLD_ON_DELAY_TIME_NSEC    (480U)   /**< Delay time for holding signal after turning on BGT VCC signal */

#define SAMPLE_HOLD_START_DELAY_TIME_NSEC (400U)   /**< Slightly shift the start holding signal after turning on BGT VCC signal */

#define PTAT_DELAY_USEC                   (1000U)  /**< The delay between PTAT on signal and the start of the frame sampling */

#define PTAT_SIGNAL_SWITCHED              (1U)     /**< Set to 1 if PTAT signal should be switched, 0 avoids switching of PTAT signal */

#define BGT_TX_ON_DELAY_TIME_NSEC         (400U)   /**< Delay time for BGT TX signal after turning on BGT VCC signal */

#define BGT_TX_ON_START_DELAY_TIME_NSEC   (300U)   /**< Delay before to enable the BGT TX */

#define BB_PRE_TRIGGER_ADVANCE_USEC       (20000U) /**< Pre-trigger advance for Baseband enable */

#define PTAT_S_H_PULSE_WIDTH_USEC         (20U)    /**< PTAT S&H Pulse trigger */

#define PTAT_S_H_SWITCHING                (1U)     /**< PTAT VCC and S&H default behavior */

//============================ DSP CONFIG ==================================//

#define DOPPLER_FFT_SIZE                  (256U)   /**< Doppler FFT length, with zero padding */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CONFIG_H_ */

/* --- End of File -------------------------------------------------------- */
