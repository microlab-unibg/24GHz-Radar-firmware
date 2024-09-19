/**
 * \file EndpointDeviceStatus.h
 *
 * \brief This file defines the API to communicate with Radar Base Endpoints
 *        in Infineon sensor devices.
 *
 * The module EndpointDeviceStatus.c provides functions to communicate with
 * Infineon radar sensor devices. The user must use the function provided by
 * the module \ref Protocol.c to open a connection to a radar sensor device
 * and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_base_is_compatible_endpoint can be used to check
 * each endpoint if it is compatible with this API. If it is the types and
 * functions in from this module can be used to send parameters to the
 * endpoint and query the current settings and radar frame data.
 *
 * All data received from the connected device is returned through callbacks.
 * For each message type that can be received from the device, a separate
 * callback function type is defined. The user must register the callback
 * function for each message type. If no callback function is registered the
 * received message is ignored. Additionally the user can provide a data
 * context pointer for each callback function that will be forwarded with any
 * call to that function.
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

#ifndef ENDPOINTDEVICESTATUS_H_INCLUDED
#define ENDPOINTDEVICESTATUS_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>
#include "EndpointRadarErrorCodes.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \defgroup EndpointMcuStatus EndpointMcuStatus
 *
 * \brief EndpointMcuStatus documentation.
 *
 * @{
 */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

typedef struct {
	uint8_t error_type;   		// type of error message: info, warning, error
	uint16_t error_code;		// error code
	uint32_t frame_count;
	uint16_t error_msg_size;	// the size of error msg
	char *message;				// error msg
} device_status_response_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a MCU Status
*        endpoint.
*
* This function checks type and version of the specified endpoint in a
* connected device and returns a code that indicates if that endpoint is
* compatible to the radar endpoint implementation in this module.
*
* \param[in] protocol_handle  A handle to the connection to the sensor
*                             device.
* \param[in] endpoint         The endpoint in the connected device to be
*                             checked for compatibility.
*
* \return If the specified endpoint is compatible to this implementation the
*         function returns 0. If the endpoint is not compatible, a negative
*         error code is returned.
*/
int32_t ep_device_status_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint);

typedef void(*Callback_Device_Status_t)(void* context,
                                     int32_t protocol_handle,
                                     uint8_t endpoint,
                                     const device_status_response_t *pStatus);

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

/**
 * \brief This functions registers a callback device status
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Chirp_Duration_t.
 *
 * \param[in] callback  The function to be called when a device statusrequest
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_device_status_set_callback_mcu_status(Callback_Device_Status_t callback,
                                             void* context);


int32_t ep_device_status_get_status(int32_t protocol_handle, uint8_t endpoint);



/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTDEVICESTATUS_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
