/**
 * \file EndpointRadarDeviceStatus.h
 *
 * \brief Radar endpoint containing functionality pertinent to Industrial Radar Demo kits like D2G and P2G etc.
 *
 */

/* ===========================================================================
** Copyright (C) 2020-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_ENDPOINTRADARDEVICESTATUS_H_
#define HOSTCOMMUNICATION_ENDPOINTRADARDEVICESTATUS_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "Protocol.h"
#include "ifxRadar.h"

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

#define EP_RADAR_DEVICE_STATUS_LIST_ENTRY(context) \
{ \
    /*.endpoint_type    = */ 0x44455653, /* ASCII code 'DEVS' */ \
    									 /*  = MCU Status */ \
    /*.endpoint_version = */ 1, \
    /*.handle_message   = */ ep_device_status_handle_message, \
    /*.context          = */ context, \
    /*.handle_change    = */ ep_device_status_handle_change \
}

typedef struct
{
   uint8_t error_type;      // type of error message: info, warning, error
   uint16_t error_code;     // error code
   uint32_t frame_count;
   uint16_t error_msg_size; // the size of error msg
   const char *error_msg;   // error msg
} device_status_response_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

uint16_t send_device_status_response();

uint16_t ep_device_status_handle_message(uint8_t endpoint, uint8_t *message_data,
                                         uint16_t num_bytes, void* context);

void ep_device_status_handle_change(uint8_t endpoint, void* context, uint32_t what);

#endif /* HOSTCOMMUNICATION_INC_ENDPOINTRADARDEVICESTATUS_H_ */
