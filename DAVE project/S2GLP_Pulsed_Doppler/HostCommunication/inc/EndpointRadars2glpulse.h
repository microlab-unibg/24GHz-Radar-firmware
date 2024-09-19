/**
 * \file EndpointRadars2glpulse.h
 *
 * \brief Radar endpoint containing functionality pertinent to Sense2GoL Pulse
 *        Industrial radar demo kit.
 *
 */

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_ENDPOINTRADAR_S2GLPULSE_H_
#define HOSTCOMMUNICATION_ENDPOINTRADAR_S2GLPULSE_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "Protocol.h"
#include "ifxRadar.h"
#include "radar_pulse.h"

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

#define EP_RADAR_S2GLPULSE_LIST_ENTRY(context) \
{ \
    /*.endpoint_type    = */ 0x544A5055, /* ASCII code 'TJPU' */ \
    									 /*  = S2GPULSE */ \
    /*.endpoint_version = */ 1, \
    /*.handle_message   = */ ep_radar_s2glpulse_handle_message, \
    /*.context          = */ context, \
    /*.handle_change    = */ ep_radar_s2glpulse_handle_change \
}

#define MSG_PARA_VALUES_IND     0x30 /**< A message to request the parameter values. */
#define MSG_PARA_VALUES_RES     0x31 /**< A message to confirm the parameter values. */

#define MSG_PARA_VALUES_DEF_IND 0x32 /**< A message to request the default parameter list values */
#define MSG_PARA_VALUES_DEF_RES 0x33 /**< A message containing the parameters variation */
#define MSG_PARA_SET_VALUES_IND 0x34 /**< A message to set the parameter values */

#define MSG_RESULT_IND          0x35
#define MSG_RESULT_RES          0x36 /**< Result response, was started via MSG_RESULT_REQ_IND */

#define MSG_GET_SHIELD_INFO_IND 0x40
#define MSG_GET_SHIELD_INFO_RES 0x41

#define MSG_GET_RAW_DATA_IND    0x46
#define MSG_GET_RAW_DATA_RES    0x47

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

uint16_t ep_radar_s2glpulse_handle_message(uint8_t endpoint, uint8_t *message_data,
                                           uint16_t num_bytes, void* context);

void ep_radar_s2glpulse_handle_change(uint8_t endpoint, void* context, uint32_t what);

void send_parameter_values(uint8_t endpoint, pulse_para_values_t *p_para);

void send_pulse_frame_data(uint8_t endpoint, acq_buf_obj *p_acq_buf, device_settings_t *p_dev);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_ENDPOINTRADAR_S2GLPULSE_H_ */

/* --- End of File -------------------------------------------------------- */
