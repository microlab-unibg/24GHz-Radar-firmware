/**
 * \file EndpointMcuStatus.c
 *
 * \brief This file implements the API to communicate with Radar Base
 *        Endpoints in Infineon sensor devices.
 *
 * See header \ref EndpointRadarBase.h for more information.
 */

/* ===========================================================================
** Copyright (C) 2017 - 2019 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include "EndpointRadarDeviceStatus.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include <stdlib.h>
#include <string.h>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/
/**
 * \internal
 * \defgroup EndpointRadarBaseCommandCodes
 *
 * \brief These are the command codes to identify the payload type.
 *
 * Each payload message of the supported endpoint starts with one of these
 * commend codes.
 *
 * @{
 */
#define MSG_STATUS_REQ_RES          0x4A /**< A message containing status request */
#define MSG_STATUS_REQ_IND          0x4B /**< A message to retrieve status request */

/** @} */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * \internal
 * \brief This function is called to parse a payload message.
 *
 * Whenever a connected device sends a payload message from an endpoint that
 * is of type and version compatible to the implementation is this module,
 * this function is invoked to parse the content of that message.
 *
 * \param[in] protocol_handle  The handle to the connection, the message was
 *                             received through.
 * \param[in] endpoint         The endpoint the message was sent from.
 * \param[in] payload          A pointer to the buffer containing the payload
 *                             to parse.
 * \param[in] payload_size     The number of bytes in payload.
 */
static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size);

/**
 * \internal
 * See module \ref EndpointRadarErrorCodes.c for information.
 */
extern const char* ep_radar_get_error_code_description(uint16_t error_code);

static int32_t parse_frame_info(int32_t protocol_handle, uint8_t endpoint,
                                const uint8_t* payload,
                                uint16_t payload_size);

/*
==============================================================================
   4. DATA
==============================================================================
*/

/**
 * \internal
 * \brief This structure contains information about this module needed by the
 *        module Protocol.c
 *
 * The structure contains the endpoint type and version range that is
 * supported by this module. The number identifying the endpoint type is the
 * ASCII code for 'MCUS' (= MCU Status).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and retrieve human readable status messages. Those function are
 * called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_RadarDeviceStatus_definition =
{
    /*.type             =*/ 0x44455653,
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadarDeviceStatus",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointDeviceStatusRegisteredCallbacks
 *
 * \brief The registered callback function are stored here.
 *
 * For each message type that is expected from the connected device a callback
 * function cas be registered. That registered callback function is stored
 * here along with a data pointer that is forwarded the callback function each
 * time it is issued.
 *
 * @{
 */

/**
 * \brief The callback function to handle radar frame data messages.
 */
static Callback_Device_Status_t callback_device_status = NULL;

/**
 * \brief The context data pointer for the radar frame data message callback
 *        function.
 */
static void* context_device_status = NULL;

/** @} */

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful
     */
    if (parse_frame_info(protocol_handle, endpoint,
                         payload, payload_size))
    {
        return;
    }
}

static int32_t parse_frame_info(int32_t protocol_handle, uint8_t endpoint,
                                const uint8_t* payload,
                                uint16_t payload_size)
{
    const uint16_t header_size = 9;

    if ((protocol_read_payload_uint8(payload, 0) == MSG_STATUS_REQ_RES) &&
        (payload_size >= header_size))
    {
        if (callback_device_status)
        {
            
            device_status_response_t mcu_status;

            mcu_status.error_type = protocol_read_payload_uint8(payload,  1);
            mcu_status.error_code = protocol_read_payload_uint16(payload,  2);
            mcu_status.frame_count = protocol_read_payload_uint32(payload,  4);
            mcu_status.error_msg_size = protocol_read_payload_uint16(payload,  8);
            char *msg = malloc(mcu_status.error_msg_size * sizeof(char));
            for(uint16_t i = 0; i < mcu_status.error_msg_size; i++)
            {
                msg[i] = protocol_read_payload_uint8(payload, 10 + i);
            }
            mcu_status.message = msg;

            /* send frame info to callback */
            callback_device_status(context_device_status, protocol_handle,
                                endpoint, &mcu_status);
            free(msg);
        

            return 1;
        }
    }

    return 0;
}


/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_device_status_is_compatible_endpoint(int32_t protocol_handle,
                                             uint8_t endpoint)
{
    return protocol_is_endpoint_compatible(protocol_handle, endpoint,
                                           &ep_RadarDeviceStatus_definition);
}

void ep_device_status_set_callback_mcu_status(Callback_Device_Status_t callback, void* context)
{
    callback_device_status = callback;
    context_device_status = context;
}

int32_t ep_device_status_get_status(int32_t protocol_handle, uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_STATUS_REQ_IND);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_RadarDeviceStatus_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
