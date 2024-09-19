/**
 * @file: EndpointRadarMcuBase.c
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
#include "EndpointRadarMcuBase.h"
#include "EndpointRadarChangeCodes.h"
#include "PayloadHelper.h"
#include "EndpointRadarMcubase.h"
#include "radar_mcubase.h"
#include "datastore_eep.h"
#include "radar_pulse.h"

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
 
#define MSG_CONSUMPTION_DEF_IND			0xD0
#define MSG_CONSUMPTION_DEF_RES			0xD1	/**< A message containing the parameters variation */
#define MSG_CONSUMPTION_IND		    	0xD2    /**< A message to request the consumption */
#define MSG_CONSUMPTION_RES			    0xD3    /**< A message to provide the consumption */

#define MSG_RF_SHIELD_ID_IND			0xE0	// to be implemented
#define MSG_RF_SHIELD_ID_RES			0xE1	// to be implemented

#define MSG_STORE_PARAMETER_IND			0xE2	// to be implemented
#define MSG_STORE_PARAMETER_RES			0xE3	// to be implemented
#define MSG_PARA_FACTORY_RESET_IND		0xE4	// to be implemented
#define MSG_PARA_FACTORY_RESET_RES		0xE5	// to be implemented

#define MSG_ERROR_REQ					0xF0 	// to be implemented

// TODO: move to a common place
#define NB_OF_ELEMENTS(a)  (sizeof(a)/sizeof(a[0]))

static uint8_t mcu_base_endpoint = 0;

/** @} */

/*  helper */

uint16_t send_consumption_response(uint8_t entries, consumption_response_t *pData )
{
	uint8_t message[256];
	int16_t offset = 0;
	uint8_t endpoint = mcu_base_endpoint;

	offset += wr_payload_u8(message, offset, MSG_CONSUMPTION_RES);
	offset += sizeof(uint16_t); // reserve space for the total message size and write it later
	offset += wr_payload_u8(message, offset, entries);
	for(int i = 0; i < entries; i++) {
		offset += wr_payload_u8(message, offset, pData[i].index);
		offset += wr_payload_float(message, offset, pData[i].value);

		uint8_t len = (uint8_t) strlen(pData[i].p_unit);
		offset += wr_payload_u8(message, offset, len);
		for(int k = 0; k < len; k++)
		{
			offset += wr_payload_u8(message, offset, pData[i].p_unit[k]);
		}
	}
	wr_payload_u16(message,1,offset);

	/* send message */
	protocol_send_header(endpoint, offset);
	protocol_send_payload(message, offset);
	protocol_send_tail();

	return RADAR_ERR_OK;
}

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

/*
 *  TODO: take this out of the store:
 */

uint32_t g_detected_rf_shield_id = 0;

uint16_t ep_radar_mcu_base_handle_message(uint8_t endpoint, uint8_t *message_data,
											uint16_t num_bytes,	void* context)
{
	mcu_base_endpoint = endpoint;

	switch (message_data[0])
	{

	case MSG_CONSUMPTION_DEF_IND:
		if (num_bytes == 1)
		{
			static uint8_t message[512]; // allocate statically, rather then on the call stack!
			memset(message, 0, sizeof(message));

			uint32_t offset = 0;
			para_consumption_def_t *p_consumption_def_list = radar_get_consumption_labels();
			if (p_consumption_def_list == NULL)
			{
				para_consumption_def_t empty_def = {.num_labels = 0};
				p_consumption_def_list = &empty_def;
			}

			offset += wr_payload_u8(message, 0, MSG_CONSUMPTION_DEF_RES);
			offset += sizeof(uint16_t); // size if uint16

			offset += wr_payload_helper_string_list(message, offset, p_consumption_def_list->num_labels, p_consumption_def_list->p_consumption_labels);

			// write size values of the message!
			wr_payload_u16(message, 1, (uint16_t)offset);

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();

			return RADAR_ERR_OK;
		}
		break;
	case MSG_CONSUMPTION_IND:
		if (num_bytes == 5)
		{
			uint16_t in_offset = 1;
			uint32_t repeat_count = rd_payload_u32(message_data, in_offset);
			set_consumption_repeat_count(repeat_count);
			return RADAR_ERR_OK;
		}
		break;

	case MSG_RF_SHIELD_ID_IND:
		if (num_bytes == 1)
		{
			/* we received a request to provide the detected RF shield id */

			static uint8_t message[200]; // allocate statically, rather then on the call stack!
			memset(message, 0, sizeof(message));

			/* get rf board id variant */
			uint32_t index = ds_get_board_variant();

			if (index == 0 || index > sizeof(rf_shield_info) / sizeof(rf_shield_info[0]) + 1 )
			{
			index = 1;
			}
			pulse_rf_shield_info_t *p_info = &rf_shield_info[index - 1];

			board_info_cnf_t board_info = { .major_version = 2, .minor_version = 0, .description = "XMC4700 Radar baseboard V2.0" };

			board_info_cnf_t *p_board_info = &board_info;

			uint32_t offset = 0;
			offset += wr_payload_u8(message, offset, MSG_RF_SHIELD_ID_RES);
			offset += wr_payload_u8(message, offset, p_board_info->major_version);
			offset += wr_payload_u8(message, offset, p_board_info->minor_version);
			offset += wr_payload_string(message, offset, p_info->rf_shield_board_id);
			offset += wr_payload_string(message, offset, p_board_info->description);

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();
			return RADAR_ERR_OK;
		}
		break;

	case MSG_STORE_PARAMETER_IND:
		if (num_bytes == 1)
		{
			/* we received a request to store the Parameter persistent */
			static uint8_t message[10]; // allocate statically, rather then on the call stack!
			memset(message, 0, sizeof(message));

			/* store the parameters persistent */
			ds_hw_para_store_persistent();
			ds_algo_para_store_persistent();

			uint16_t offset = 0;
			offset += wr_payload_u8(message, 0, MSG_STORE_PARAMETER_RES);

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();
			return RADAR_ERR_OK;
		}
		break;

	case MSG_PARA_FACTORY_RESET_IND:
		if (num_bytes == 1)
		{
			/* we received a request to store the Parameter persistent */
			static uint8_t message[10]; // allocate statically, rather then on the call stack!
			memset(message, 0, sizeof(message));

			uint16_t offset = 0;
			offset += wr_payload_u8(message, 0, MSG_PARA_FACTORY_RESET_RES);

			/* restore parameter from factory settings! */
			ds_hw_para_factory_reset();
			ds_algo_para_factory_reset();

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();
			return RADAR_ERR_OK;
		}
		break;

	default:
		break;
	}

	return PROTOCOL_STATUS_INVALID_PAYLOAD;
}

//============================================================================

void ep_radar_mcu_base_handle_change(uint8_t endpoint, void* context, uint32_t what)
{
	Radar_Handle_t radar_driver = (Radar_Handle_t)context;

	(void)radar_driver; /* Suppress compiler warnings of unused variable */

	switch (what)
	{

	case EP_RADAR_CHNG_CURRENT_CONSUMPTION:
		inc_consumption_repeat_count(1);
		mcu_base_updates();
		break;
	default:
		break;
	}
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
