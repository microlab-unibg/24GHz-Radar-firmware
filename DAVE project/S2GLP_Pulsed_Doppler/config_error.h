/**
    @file: config_error.h

    @brief: Compilation errors check for configuration file values defined by user.
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

#ifndef CONFIG_ERROR_H_
#define CONFIG_ERROR_H_

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

#include "config.h"
#include "sense2gol_bsp.h"
#include "sense2gol_bsp_limits.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

//===================== General settings error check =======================//

/* Check valid range of Number of chirps */
#ifdef NUM_OF_CHIRPS
	#if (NUM_OF_CHIRPS <= 0) || (NUM_OF_CHIRPS > 1)
		#error "NUM_OF_CHIRPS in config.h file is out of range [1 , 64]"
	#endif
#endif

/* Check valid range of samples per chirp */
#ifdef SAMPLES_PER_CHIRP
	#if (SAMPLES_PER_CHIRP < BSP_MIN_NUM_SAMPLES_PER_CHIRP) || (SAMPLES_PER_CHIRP > BSP_MAX_NUM_SAMPLES_PER_CHIRP)
		#error "SAMPLES_PER_CHIRP in config.h file is out of range"
	#endif
#endif

//======================== DSP settings error check ========================//

/* Check valid range of RX antenna selection */
#ifdef GAIN_SELECTOR_DOPPLER
	#if (GAIN_SELECTOR_DOPPLER < 0) || (GAIN_SELECTOR_DOPPLER > 1)
		#error "GAIN_SELECTOR in config.h file is out of range [0 , 1]"
	#endif
#endif

//===================== Doppler settings error check =======================//

/* Check valid range of Doppler Sampling frequency in Hz */
#ifdef DOPPLER_SAMPLING_FREQ_HZ
	#if (DOPPLER_SAMPLING_FREQ_HZ < 0) || (DOPPLER_SAMPLING_FREQ_HZ < BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ)
		#error "DOPPLER_SAMPLING_FREQ_HZ in config.h file should be non-negative value and it should not be less than BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ"
	#endif
	#if (DOPPLER_SAMPLING_FREQ_HZ > BSP_MAX_DOPPLER_SAMPLING_FREQ_HZ)
		#error "DOPPLER_SAMPLING_FREQ_HZ in config.h file is set too high, it should be less than 100kHz"
	#endif
#endif

/* Check valid range of Minimum speed to be filtered out by Doppler algorithm */
#if defined (MIN_SPEED_KMPH) && defined (MAX_SPEED_KMPH)
	#if (MIN_SPEED_KMPH < 0)
		#error "MIN_SPEED_KMPH in config.h file should be non-negative value"
	#endif
	#if (MIN_SPEED_KMPH > (BSP_MAX_SPEED_KMPH*SPEED_DIVIDER)/BSP_SPEED_DIVIDER)
		#error "MIN_SPEED_KMPH in config.h file should not be larger than BSP_MAX_SPEED_KMPH"
	#endif
	#if (MIN_SPEED_KMPH < (BSP_MIN_SPEED_KMPH*SPEED_DIVIDER)/BSP_SPEED_DIVIDER)
		#error "MIN_SPEED_KMPH in config.h file should be smaller than BSP_MIN_SPEED_KMPH"
	#endif
	#if (MIN_SPEED_KMPH > MAX_SPEED_KMPH)
		#error "MIN_SPEED_KMPH in config.h file should not be larger than MAX_SPEED_KMPH"
	#endif
#endif

/* Check valid range of Maximum speed to be filtered out by Doppler algorithm */
#if defined (MAX_SPEED_KMPH) && defined (MIN_SPEED_KMPH)
	#if (MAX_SPEED_KMPH < 0)
		#error "MAX_SPEED_KMPH in config.h file should be non-negative value"
	#endif
	#if (MAX_SPEED_KMPH < (BSP_MIN_SPEED_KMPH*SPEED_DIVIDER)/BSP_SPEED_DIVIDER)
		#error "MAX_SPEED_KMPH in config.h file should not be smaller than BSP_MIN_SPEED_KMPH"
	#endif
	#if (MAX_SPEED_KMPH > (BSP_MAX_SPEED_KMPH*SPEED_DIVIDER)/BSP_SPEED_DIVIDER)
		#error "MAX_SPEED_KMPH in config.h file should not be larger than BSP_MAX_SPEED_KMPH"
	#endif
#endif

/* Check valid range of FFT spectrum threshold to detect a target in Doppler */
#ifdef SPEED_DETECTION_THRESHOLD
	#if (SPEED_DETECTION_THRESHOLD < 0) || (SPEED_DETECTION_THRESHOLD < BSP_MIN_SPEED_DETECTION_THRESHOLD)
		#error "SPEED_DETECTION_THRESHOLD in config.h file should be non-negative value and it should not be less than BSP_MIN_SPEED_THRESHOLD"
	#endif
	#if (SPEED_DETECTION_THRESHOLD > BSP_MAX_SPEED_DETECTION_THRESHOLD)
		#error "SPEED_DETECTION_THRESHOLD in config.h file is set too high, it should be less than BSP_MAX_SPEED_THRESHOLD"
	#endif
#endif

/* Check valid range for motion detection threshold */
#ifdef MOTION_DETECTION_THRESHOLD
	#if (MOTION_DETECTION_THRESHOLD < 0) || (MOTION_DETECTION_THRESHOLD < BSP_MIN_MOTION_DETECTION_THRESHOLD)
		#error "MOTION_DETECTION_THRESHOLD in config.h file should be non-negative value and it should not be less than BSP_MIN_MOTION_DETECTION_THRESHOLD"
	#endif
	#if (MOTION_DETECTION_THRESHOLD > BSP_MAX_MOTION_DETECTION_THRESHOLD)
		#error "MOTION_DETECTION_THRESHOLD in config.h file is set too high, it should be less than BSP_MAX_MOTION_THRESHOLD"
	#endif
#endif

/* Check valid range for sample skip count */
#ifdef SAMPLE_SKIP_COUNT
	#if (SAMPLE_SKIP_COUNT < 0) || (SAMPLE_SKIP_COUNT < BSP_MIN_SAMPLE_SKIP_COUNT)
		#error "SAMPLE_SKIP_COUNT in config.h file should be non-negative value and it should not be less than BSP_MIN_SAMPLE_SKIP_COUNT"
	#endif
	#if (SAMPLE_SKIP_COUNT > BSP_MAX_SAMPLE_SKIP_COUNT)
		#error "SAMPLE_SKIP_COUNT in config.h file should be non-negative value and it should be less than BSP_MAX_SAMPLE_SKIP_COUNT"
	#endif
#endif

/* Check valid range for duty cycle time */
#ifdef PULSE_WIDTH_USEC
	#if (PULSE_WIDTH_USEC < 0) || (PULSE_WIDTH_USEC < BSP_MIN_PULSE_WIDTH_USEC)
		#error "PULSE_WIDTH_USEC in config.h file should be non-negative value and it should not be less than BSP_MIN_PULSE_WIDTH_USEC"
	#endif
	#if (PULSE_WIDTH_USEC> BSP_MAX_PULSE_WIDTH_USEC)
		#error "PULSE_WIDTH_USEC in config.h file is set too high,  it should be less than BSP_MAX_PULSE_WIDTH_USEC"
	#endif
#endif

/* Check valid range for frame time */
#ifdef FRAME_PERIOD_MSEC
	#if (FRAME_PERIOD_USEC > BSP_MAX_FRAME_PERIOD_MSEC)
		#error "FRAME_PERIOD_MSEC in config.h is set too high,  it should be less than BSP_MAX_FRAME_PERIOD_MSEC"
	#endif
#endif

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CONFIG_ERROR_H_ */

/* --- End of File -------------------------------------------------------- */
