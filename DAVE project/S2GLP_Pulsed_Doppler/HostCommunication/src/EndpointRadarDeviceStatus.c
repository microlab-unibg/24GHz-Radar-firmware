/**
 * @file: EndpointRadarDeviceStatus.c
 *
 * @brief: Radar endpoint containing functionality pertinent to Industrial
 *         radar demo kits.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "EndpointRadarDeviceStatus.h"
#include "radar_device_status.h"
#include "EndpointRadarChangeCodes.h"
#include "PayloadHelper.h"
/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */
/**
 * \defgroup MessageTypes
 *
 * @brief: This end point knows these message types.
 *
 * The first payload byte of every message is one of these codes.
 *
 * @{
 */
#define MSG_STATUS_REQ_RES          0x4A /**< A message containing status request */
#define MSG_STATUS_REQ_IND          0x4B /**< A message to retrieve status request */

/** @} */

static uint8_t device_status_endpoint = 0;

static void fill_status_response(device_status_response_t *p_status_response)
{
	Device_Status_t *device_status = NULL;
	device_status = device_status_get_handle(device_status);

	p_status_response->error_code = (uint16_t) device_status->latest_error;
	p_status_response->error_type = (uint8_t) device_status->state,
	p_status_response->frame_count = 0;
	if (device_status->state != STATE_NO_ERROR)
	{
		p_status_response->error_msg = device_status_get_error_description(device_status->latest_error);
		p_status_response->error_msg_size = strlen(p_status_response->error_msg) + 1;  // include the leading zero!
	}
	else
	{
		p_status_response->error_msg = "No Error";
		p_status_response->error_msg_size = strlen(p_status_response->error_msg) + 1; // include the leading zero!
	}
}

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

uint16_t send_device_status_response()
{
	if (device_status_endpoint != 0)
	{
		device_status_response_t status_response;

		/* fetch device info an fill status response */
		fill_status_response(&status_response);

		static uint8_t message[512];  // allocate statically, rather then on the call stack!
		memset(message, 0, sizeof(message));
		size_t offset = 0;

		offset += wr_payload_u8(message, offset, MSG_STATUS_REQ_RES);
		offset += wr_payload_u8(message, offset, status_response.error_type);		/*error type*/
		offset += wr_payload_u16(message, offset, status_response.error_code);		/*error code*/
		offset += wr_payload_u32(message, offset, status_response.frame_count);		/*frame count*/
		offset += wr_payload_u16(message, offset, status_response.error_msg_size);		/*frame count*/

		for(uint16_t i=0; i<status_response.error_msg_size; i++ )
		{
			offset += wr_payload_u8(message, offset, status_response.error_msg[i]);
		}

		/* send message */
		protocol_send_header(device_status_endpoint, offset);
		protocol_send_payload(message, offset);
		protocol_send_tail();

		/* clean error, if not fatal */
		if (status_response.error_type != 4)
			device_status_clean();
	}
	return RADAR_ERR_OK;
}

uint16_t ep_device_status_handle_message(uint8_t endpoint, uint8_t *message_data,
											uint16_t num_bytes,	void* context)
{
	/*store endpoint information*/

	device_status_endpoint = endpoint;

    switch (message_data[0])
    {
	case MSG_STATUS_REQ_IND:
		send_device_status_response();
		return RADAR_ERR_OK;
	default:
		break;
    }

    return PROTOCOL_STATUS_INVALID_PAYLOAD;
}

void ep_device_status_handle_change(uint8_t endpoint, void* context, uint32_t what)
{
	Radar_Handle_t radar_driver = (Radar_Handle_t)context;

	(void)radar_driver; /* Suppress compiler warnings of unused variable */

	switch (what)
	{
	default:
		break;
	}
}

/* --- End of File -------------------------------------------------------- */
