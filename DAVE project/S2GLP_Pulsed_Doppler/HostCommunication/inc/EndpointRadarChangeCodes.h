/**
 * \file EndpointRadarChangeCodes.h
 *
 */

/* ===========================================================================
** Copyright (C) 2016-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_ENDPOINTRADARCHANGECODES_H_
#define HOSTCOMMUNICATION_ENDPOINTRADARCHANGECODES_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define EP_RADAR_CHNG_FRAME_FORMAT           1
#define EP_RADAR_CHNG_FMCW_SETTINGS          2
#define EP_RADAR_CHNG_DOPPLER_SETTINGS       3
#define EP_RADAR_CHNG_ADC_SETTINGS           4

#define EP_RADAR_CHNG_TX_MODE                5
#define EP_RADAR_CHNG_BASEBAND_SETTINGS      6
#define EP_RADAR_CHNG_OSC_PHASE_SETTINGS     7
#define EP_RADAR_CHNG_BASEBAND_TEST_SETTINGS 9

#define EP_RADAR_CHNG_PLL_EXPERT_SETTINGS    8

#define EP_RADAR_CHNG_FRAME_SEQUENCE         10
#define EP_RADAR_CHNG_SELECTED_SHAPE         11
#define EP_RADAR_CHNG_CHIRP_TIMING           12
#define EP_RADAR_CHNG_STARTUP_TIMING         13
#define EP_RADAR_CHNG_CHIRP_END_DELAY        14
#define EP_RADAR_CHNG_IDLE_CONFIG            15
#define EP_RADAR_CHNG_DEEP_SLEEP_CONFIG      16
#define EP_RADAR_CHNG_DATA_SLICE_SIZE        17

#define EP_RADAR_CHNG_PARAMETER_LIMITS       18
#define EP_RADAR_CHNG_PARAMETER_VALUES       19
#define EP_RADAR_CHNG_CURRENT_CONSUMPTION    20

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_ENDPOINTRADARCHANGECODES_H_ */

/* --- End of File -------------------------------------------------------- */
