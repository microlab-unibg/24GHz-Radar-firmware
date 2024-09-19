/**
 * \file EndPointRadarBaseBoard.c
 *
 * \brief This file implements the API to communicate with Radar Baseboard
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndpointRadarBaseBoard.h for more information.
 */

/* ===========================================================================
** Copyright (C) 2017-2021 Infineon Technologies AG
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
#include "EndpointRadarBaseBoard.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "EndpointRadarErrorCodes.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/**
 * \internal
 * \defgroup EndpointRadarIndustrialCommandCodes
 *
 * \brief The radar industrial endpoint defined the following commands. 
 *
 * Each payload message of the radar industrial endpoint starts with one of these
 * command codes.
 *
 * @{
 */

#define MSG_SET_DRIVER_VERSION    		0x21 /**< A message containing device information. */
#define MSG_GET_DEVICE_INFO       		0x22 /**< A message to query device information. */

#define MSG_CONSUMPTION_DEF_REQ			0xD0
#define MSG_CONSUMPTION_DEF_CNF			0xD1

#define MSG_CONSUMPTION_REQ		    	0xD2
#define MSG_CONSUMPTION_CNF			    0xD3

#define MSG_BOARD_INFO_REQ				0xE0
#define MSG_BOARD_INFO_CNF				0xE1
#define MSG_PARA_STORE_REQ				0xE2
#define MSG_PARA_STORE_CNF				0xE3
#define MSG_PARA_FACTORY_RESET_REQ		0xE4
#define MSG_PARA_FACTORY_RESET_CNF		0xE5

#define MSG_ERROR_IND					0xF0

/** @} */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/
/**
 * \internal
 * See module \ref EndpointRadarErrorCodes.c for information.
 */
extern const char* ep_radar_get_error_code_description(uint16_t error_code);

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
 * ASCII code for 'RIND' (= Radar Industrial).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and to retrieve human readable status/error messages from status
 * and error codes. Those function are called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_RadarBaseBoard_definition =
{
    /*.type             =*/ 0x544A4241, /* ASCII code 'TJBA' = Tjuefire Mcu base */
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
	/*.description      =*/ "ifxRadar mcu base",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointRadarP2GRegisteredCallbacks
 *
 * \brief The registered callback function are stored here.
 *
 * For each message type that is expected from the connected device a callback
 * function can be registered. That registered callback function is stored
 * here along with a data pointer that is forwarded the callback function each
 * time it is issued.
 *
 * @{
 */

/**
* \brief The callback function to handle the parameter value indication.
*/

static consumption_def_cnf_callback_t consumption_def_cnf_cb = NULL;
static consumption_def_cnf_ctx_t *consumption_def_cnf_ctx = NULL;

static consumption_cnf_callback_t consumption_cnf_cb = NULL;
static consumption_cnf_ctx_t *consumption_cnf_ctx = NULL;

static board_info_cnf_callback_t board_info_cnf_cb = NULL;
static board_info_cnf_ctx_t *board_info_cnf_ctx = NULL;

static para_factory_reset_cnf_callback_t para_factory_reset_cnf_cb = NULL;
static para_factory_reset_cnf_ctx_t *para_factory_reset_cnf_ctx = NULL;

static para_store_cnf_callback_t para_store_cnf_cb = NULL;
static para_store_cnf_ctx_t *para_store_cnf_ctx = NULL;


/** @} */

/*
 *  The max str. len that is transmitted is only 255 bytes (because a uint8_t is used for the size
 *
 */
static uint16_t protocol_read_helper_str_list(const uint8_t *payload, const size_t offset, size_t *p_elements, char ***ptr  )
{
	uint16_t increment = 0;
	uint8_t elements = protocol_read_payload_uint8( &payload[0], (uint16_t)offset );
	increment += 1;
	if( p_elements != NULL)
		*p_elements = (size_t) elements;

	if( ptr == NULL)
		return 0;
	char **pData = (char **) malloc(sizeof(char**) * elements );
	for( int i = 0; i < elements; i++)
	{
		uint8_t len = protocol_read_payload_uint8( &payload[0], (uint16_t)offset + increment );
		increment += 1;
		char* pTemp = (char *) malloc(sizeof(char) * ( len + 1 ) );
		memcpy(pTemp, &payload[offset + increment], len);
		*(pTemp + len) = 0; 			// zero terminate the string!
		pData[i] = pTemp;
		increment += len;
	}
	*ptr = pData;
	return increment;
}


/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t parse_para_values_cnf(int32_t protocol_handle, uint8_t endpoint,
								const uint8_t* payload, uint16_t payload_size)
{
	uint8_t payloadId = protocol_read_payload_uint8(payload, 0);

	if ((payloadId == MSG_CONSUMPTION_DEF_CNF) &&
			(payload_size > 1))
		{
			if (consumption_def_cnf_cb)
			{
				// TODO: move this out into a seperate function!
				consumption_def_host_t consumption_def;
				uint16_t offset = 1; // skip message id

				uint16_t size = protocol_read_payload_uint16(&payload[0], offset);
				offset += 2;

				offset += protocol_read_helper_str_list( &payload[0], offset, &consumption_def.consumption_label_elements, &consumption_def.pp_consumption_labels);

				// check if offset and size matches!!
				if( size != offset )
					return EP_RADAR_ERR_MSG_SIZE_CORRUPT;

				/* send frame format to callback */
				consumption_def_cnf_cb(consumption_def_cnf_ctx, protocol_handle,
										   endpoint, &consumption_def);

				// free-up memory!!
				for( size_t i = 0; i < consumption_def.consumption_label_elements; i++)
				{
					free(consumption_def.pp_consumption_labels[i]);
				}
				free(consumption_def.pp_consumption_labels);


			}
			return 1;
		}
		else  if ((payloadId == MSG_CONSUMPTION_CNF) &&
				(payload_size > 1))
		{
			if (consumption_cnf_cb)
			{

				consumption_cnf_t consumption;

				uint16_t offset = 1;
				size_t elements = 0;
				uint16_t size = protocol_read_payload_uint16(&payload[0], offset);
				offset += sizeof(uint16_t);

				elements = (size_t) protocol_read_payload_uint8(&payload[0], offset);
				offset += sizeof(uint8_t);

				consumption_t *pp_temp = (consumption_t *) malloc( elements *sizeof(consumption_t) );

				for(uint32_t i = 0; i < elements; i++)
				{
					consumption_t *pData = (pp_temp+i);
					pData->index = protocol_read_payload_uint8(&payload[0], offset);
					offset += 1;
					uint32_t temp;
					temp = protocol_read_payload_uint32(&payload[0], offset);
					memcpy(&pData->value, &temp, sizeof(float));
					offset += sizeof(uint32_t);
					size_t len = (size_t) protocol_read_payload_uint8(&payload[0], offset);
					offset += 1;

					pData->p_unit = (char *)malloc(len + 1);
					memcpy(pData->p_unit, &payload[offset], len);
					pData->p_unit[len] = 0;

					offset += (uint16_t)len;

				}
				consumption.elements = elements;
				consumption.p_consumption = pp_temp;


				if (size != offset)
					return EP_RADAR_ERR_MSG_SIZE_CORRUPT;

				/* send result to callback */
				consumption_cnf_cb(consumption_cnf_ctx, protocol_handle,
										   endpoint, &consumption);

				// free the memory again:
				for( uint32_t i=0; i< elements; i++)
				{
					consumption_t *pData = consumption.p_consumption+i;
					free(pData->p_unit);
				}
				free(consumption.p_consumption);

			}
			return 1;
		}
		else if ((payloadId == MSG_BOARD_INFO_CNF) &&
				(payload_size > 1)) 
        {
			if (board_info_cnf_cb)
			{
				board_info_cnf_t board_info;
				uint16_t offset = 1;
				board_info.major_version = protocol_read_payload_uint8(&payload[0], offset);
				offset += 1;
				board_info.minor_version = protocol_read_payload_uint8(&payload[0], offset);
				offset += 1;

				uint8_t len = protocol_read_payload_uint8(&payload[0], offset);
				offset += 1;
				board_info.rf_shield_type_id = (char *)malloc(len+1);
				memcpy(board_info.rf_shield_type_id, &payload[offset], len);
				offset += len;
				board_info.rf_shield_type_id[len] = 0;

				len = protocol_read_payload_uint8(&payload[0], offset);
				offset += 1;
				board_info.description = (char *)malloc(len+1);
				memcpy(board_info.description, &payload[offset], len);
				board_info.description[len] = 0;
				offset += len;

				/* send result to callback */
				board_info_cnf_cb(board_info_cnf_ctx, protocol_handle,
														   endpoint, &board_info);

				free( board_info.rf_shield_type_id);
				free( board_info.description );
			}

		}
		else if ((payloadId == MSG_PARA_STORE_CNF) &&
						(payload_size == 1)) 
        {
				if (para_store_cnf_cb)
				{
					para_store_cnf_t data;
					/* send result to callback */
					para_store_cnf_cb(para_store_cnf_ctx, protocol_handle, endpoint, &data);
				}
	    }

		else if ((payloadId == MSG_PARA_FACTORY_RESET_CNF) &&
					(payload_size == 1)) 
        {
			if (para_factory_reset_cnf_cb)
			{
				para_factory_reset_t data;
				/* send result to callback */
				para_factory_reset_cnf_cb(para_factory_reset_cnf_ctx, protocol_handle, endpoint, &data);
			}
		}

		else
		{
			printf("tjba: message id not found\n");
		}

	return 0;
}


static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful
     */
	if (parse_para_values_cnf(protocol_handle, endpoint,
		payload, payload_size))
		return;
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_radar_base_board_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_RadarBaseBoard_definition);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------





int32_t ep_radar_base_board_para_factory_reset_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_FACTORY_RESET_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarBaseBoard_definition,
									cmd_message, sizeof(cmd_message));
}


int32_t ep_radar_base_board_para_store_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_STORE_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarBaseBoard_definition,
									cmd_message, sizeof(cmd_message));
}


int32_t ep_radar_base_board_board_info_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_BOARD_INFO_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarBaseBoard_definition,
									cmd_message, sizeof(cmd_message));
}

void ep_radar_base_board_board_info_cb(board_info_cnf_callback_t callback,
										board_info_cnf_ctx_t* p_context)
{
	board_info_cnf_cb 	 = callback;
	board_info_cnf_ctx 	 = p_context;
}

void ep_radar_base_board_para_factory_reset_cb(para_factory_reset_cnf_callback_t callback,
											para_factory_reset_cnf_ctx_t* p_context)
{
	para_factory_reset_cnf_cb 	 = callback;
	para_factory_reset_cnf_ctx 	 = p_context;
}

void ep_radar_base_board_para_store_cb(para_store_cnf_callback_t callback,
											para_store_cnf_ctx_t* p_context)
{
	para_store_cnf_cb 	 = callback;
	para_store_cnf_ctx 	 = p_context;
}


int32_t ep_radar_base_board_consumption_def_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_CONSUMPTION_DEF_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarBaseBoard_definition,
									cmd_message, sizeof(cmd_message));
}

void ep_radar_base_board_consumption_def_cb(consumption_def_cnf_callback_t callback,
								consumption_def_cnf_ctx_t* p_context)
{
	consumption_def_cnf_cb 		 = callback;
	consumption_def_cnf_ctx 	 = p_context;
}


/*
 *  if repeat_count is 0xFFFF.FFFF the results will be continuously provided, unless stopped (with repeat_count set to 0)
 *  if repeat_count is 1, only a single result is returned, and the next result needs to be retriggerd
 *  in case you like to avoid extra latency, by the round trip request, set the result cnt to  0xfffffffff, and a continuous stream of results will be received.
 */

int32_t ep_radar_base_board_consumption_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_CONSUMPTION_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarBaseBoard_definition,
									cmd_message, offset);
}

void ep_radar_base_board_consumption_cb(consumption_cnf_callback_t callback,
								consumption_cnf_ctx_t* p_context)
{
	consumption_cnf_cb 		 = callback;
	consumption_cnf_ctx 	 = p_context;
}








/* --- End of File -------------------------------------------------------- */
