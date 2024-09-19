/**
    @file: sense2gol_bsp_limits.h

    @brief: This file contains constants declarations and functions prototypes for
            Sense2GoL Pulse board hardware resources.
 */

/* ===========================================================================
** Copyright (C) 2019-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

#ifndef SENSE2GOL_BSP_LIMITS_H_
#define SENSE2GOL_BSP_LIMITS_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/**
  * @brief  Define for Sense2GoL Pulse board
  */
#if !defined (USE_S2GLPULSE_BOARD)
 #define USE_S2GLPULSE_BOARD
#endif

/**
 * \addtogroup BGT24LTR1x sensor definitions
 * @{
 */
#define BSP_NUM_TX_ANTENNAS                (uint8_t)1U /**< TX antennas in BGT24LTR1x */

#define BSP_NUM_RX_ANTENNAS                (uint8_t)2U /**< RX antennas in BGT24LTR1x */

#define BSP_MAX_NUM_ADC_CHANNELS           (uint8_t)4U /**< The maximum number of ADC channels that are captured */

#define BSP_NUM_TEMP_SENSORS               (uint8_t)1U /**< Number of temperature sensor in BGT24MTR12 */

#define BSP_MAX_TX_POWER_LEVEL             (uint8_t)1U /**< Maximum BGT24LTR1x TX output power */

#define BSP_MIN_RF_FREQUENCY_KHZ           (uint32_t)(24025000U) /**< Minimum RF frequency supported by BGT24LTR1x in kHz */

#define BSP_MAX_RF_FREQUENCY_KHZ           (uint32_t)(24225000U) /**< Maximum RF frequency supported by BGT24LTR1x in kHz */

/*
 * @} 
 */

/**
 * \addtogroup Sense2GoL Pulse Hardware specific constants
 * @{
 */

#define BSP_NUM_OF_ADC_CHANNELS            (uint32_t)2U       /**< Number of ADC channels to be acquired in RAM */

#define BSP_NUM_OF_AVAILABLE_RADAR_DEVICES (uint8_t)1U        /**< The number of available radar devices */

#define BSP_DOPPLER_BASE_FREQ_MHZ          (uint32_t)24050U   /**< Base frequency for Doppler, is fixed in HW and can NOT be changed by SW! */

#define BSP_MAX_ADC_BUFFER_SIZE_BYTES      (uint32_t)(BSP_NUM_OF_ADC_CHANNELS * 2048U) /**< Maximum size of buffer to hold the raw ADC samples in bytes */

#define BSP_MAX_NUM_CHIRPS_PER_FRAME       (uint32_t)1U       /**< Maximum supports up to 16 ramps, but a single cycle uses up to 2 ramp sections */

#define BSP_MIN_NUM_SAMPLES_PER_CHIRP      32U                /**< Minimum Samples per chirp supported */

#define BSP_MAX_NUM_SAMPLES_PER_CHIRP      256U               /**< Maximum Samples per chirp supported */

#define BSP_RX_ANTENNA_MASK                (uint8_t)0x03U     /**< Selection of RX Antenna */

#define BSP_ADC_RESOLUTION                 (uint8_t)12U       /**< Number of bits per ADC data sample */

#define BSP_MAX_ADC_FREQ_HZ                (uint32_t)1500000U /**< Maximum sampling frequency of the XMC ADC (in units of Hz) */

#define BSP_MAX_DMA_BLOCK_SIZE             (uint32_t)4095U    /**< Maximum block size defined by the XMC DMA */

#define BSP_MAX_SPEED_KMPH                 (1080U)            /**< Maximum speed supported (units in km/h), 20km/h */

#define BSP_MIN_SPEED_KMPH                 (36U)              /**< Minimum speed supported (units in km/h), 0km/h */

#define BSP_SPEED_DIVIDER                  (100U)

#define BSP_MAX_SPEED_DETECTION_THRESHOLD  (2000U)            /**< Maximum speed detection threshold in terms of absolute amplitude FFT spectrum */

#define BSP_MIN_SPEED_DETECTION_THRESHOLD  (1U)               /**< Minimum speed detection threshold in terms of absolute amplitude FFT spectrum */

#define BSP_MAX_DOPPLER_SAMPLING_FREQ_HZ   (5000U)            /**< Maximum doppler sampling freq (in units of Hz) */

#define BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ   (1000U)            /**< Minimum doppler sampling freq (in units of Hz) */

#define BSP_MAX_MOTION_DETECTION_THRESHOLD (2000U)            /**< Maximum motion detection threshold */

#define BSP_MIN_MOTION_DETECTION_THRESHOLD (1)                /**< Minimum motion detection threshold */

#define BSP_MAX_SAMPLE_SKIP_COUNT          (1000U)            /**< Maximum number of samples skipped for signal processing at beginning of frame */

#define BSP_MIN_SAMPLE_SKIP_COUNT          (0U)               /**< Minimum number of samples skipped for signal processing at beginning of frame */

#define BSP_MAX_PULSE_WIDTH_USEC           10U                /**< Max on time for the Gate of BGT VCC input, ((1% * FRAME_PERIOD_USEC)/100) */

#define BSP_MIN_PULSE_WIDTH_USEC           4U                 /**< Min on time for the Gate of BGT VCC input, ((1% * FRAME_PERIOD_USEC)/100) */

#define BSP_MAX_FRAME_PERIOD_MSEC          2000U              /**< Max frame period (in units of ms) */

/* Current consumptions, based on measurements */
#define BSP_BGT_ON_CURRENT_mA              (45.0F)

#define BSP_PTAT_ON_CURRENT_mA             (1.5F)

#define BSP_BBF_ON_CURRENT_mA              (0.3F + 0.035F)

/*
 * @} 
 */

/**
 * \addtogroup Timeouts Definitions
 * @{
 */
#define BSP_HOST_TIMEOUT_USEC (uint32_t)(10 * 1000000U) /**< 10s timeout period for Host Application (in units of us) */

#define BSP_USB_TIMEOUT       (uint32_t)0x0FFFU         /**< Maximum attempt count to connect with the Host USB */

/*
 * @} 
 */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* SENSE2GOL_BSP_LIMITS_H_ */

/* --- End of File -------------------------------------------------------- */
