/**
 * \file EndPointRadarS2GLP.c
 *
 * \brief This file implements the API to communicate with radar Sense2GoL-Pulse
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndPointRadarS2GLP.h for more information.
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
#include "EndpointRadarS2GLP.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

#define MSG_PARA_VALUES_REQ 			0x30  	/**< A message to request the parameter values. */
#define MSG_PARA_VALUES_CNF		 		0x31  	/**< A message to request the parameter values. */
#define MSG_PARA_VALUES_DEF_REQ			0x32	/**< A message to request the default parameter list values */
#define MSG_PARA_VALUES_DEF_CNF			0x33	/**< A message containing the parameters variation */
#define MSG_PARA_SET_VALUES_REQ			0x34    /**< A message to set the parameter values */
#define MSG_RESULT_REQ			    	0x35    /**< A message to set the parameter values */
#define MSG_RESULT_CNF			    	0x36    /**< A message to set the parameter values */

#define PULSE_MSG_GET_SHIELD_INFO_REQ	0x40
#define PULSE_MSG_GET_SHIELD_INFO_CNF   0x41

#define MSG_GET_RAW_DATA_REQ			0x46
#define MSG_GET_RAW_DATA_CNF			0x47




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
const Endpoint_Definition_t ep_RadarS2GLP_definition =
{
    /*.type             =*/ 0x544A5055, /* ASCII code 'TJPU' = S2GL-Pulse FMCW (via SW-pll)*/
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar S2GL-Pulse",
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
static pulse_para_values_cnf_callback_t pulse_para_values_cnf_cb = NULL;
static pulse_para_values_cnf_ctx_t *pulse_para_values_cnf_ctx = NULL;

static pulse_para_values_def_cnf_callback_t pulse_para_values_def_cnf_cb = NULL;
static pulse_para_values_def_cnf_ctx_t *pulse_para_values_def_cnf_ctx = NULL;

static pulse_result_cnf_callback_t pulse_result_cnf_cb = NULL;
static pulse_result_cnf_ctx_t *pulse_result_cnf_ctx = NULL;

static pulse_get_shield_info_cnf_callback_t pulse_get_shield_info_cnf_cb = NULL;
static pulse_get_shield_info_cnf_ctx_t *pulse_get_shield_info_cnf_ctx = NULL;

static get_raw_data_cnf_callback_t get_raw_data_cnf_cb = NULL;
static void *get_raw_data_cnf_ctx = NULL;



/** @} */



/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t parse_para_values_cnf(int32_t protocol_handle, uint8_t endpoint,
								const uint8_t* payload, uint16_t payload_size)
{
	uint8_t payloadId = protocol_read_payload_uint8(payload, 0);

	if ((payloadId == MSG_PARA_VALUES_CNF) &&
		(payload_size == 35))  //6*float, 2xuint16, 2x uint8, 1x u32 plus msgid
	{
		if (pulse_para_values_cnf_cb)
		{
			pulse_para_values_t para_values;
			uint16_t offset = 1;

			para_values.max_speed_mps    	  	  = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.min_speed_mps       	  = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.frame_time_sec     		  = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.number_of_samples      = protocol_read_payload_uint16(&payload[0], offset);
			offset += sizeof(uint16_t);
			para_values.sampling_freq_hz    	  = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.doppler_sensitivity    = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);
			para_values.motion_sensitivity     = protocol_read_payload_float32(&payload[0], offset);
			offset += sizeof(float);

			para_values.use_high_gain_doppler_bool    = protocol_read_payload_uint8(&payload[0], offset);
			offset += sizeof(uint8_t);
			para_values.equistantant_mode     = protocol_read_payload_uint8(&payload[0], offset);
			offset += sizeof(uint8_t);
			
			para_values.number_of_skip_samples = protocol_read_payload_uint16(&payload[0], offset);
			offset += sizeof(uint16_t);

			para_values.pulse_width_usec = protocol_read_payload_uint32(&payload[0], offset);
			offset += sizeof(uint32_t);


			/* send frame format to callback */
			pulse_para_values_cnf_cb(pulse_para_values_cnf_ctx, protocol_handle,
									   endpoint, &para_values);
		}
		return 1;
	}
	else if ((payloadId == MSG_PARA_VALUES_DEF_CNF) &&
			(payload_size > 1))
		{
			if (pulse_para_values_def_cnf_cb)
			{

				pulse_para_value_def_host_t para_value_def;

				uint16_t offset = 1; // skip message id
				para_value_def.last_message_size = protocol_read_payload_uint16(&payload[0], offset);
				offset += sizeof(uint16_t);

				para_value_def.max_speed_mps_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.max_speed_mps_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.min_speed_mps_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.min_speed_mps_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.frame_time_sec_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.frame_time_sec_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.size_elems = protocol_read_payload_uint8( &payload[0], offset);
				offset += sizeof(uint8_t);
				para_value_def.p_sample_list = (uint16_t *)malloc(para_value_def.size_elems*sizeof(uint16_t));
				for(int n= 0; n < para_value_def.size_elems; n++)
				{
					para_value_def.p_sample_list[n] = protocol_read_payload_uint16( &payload[0], offset);
					offset += sizeof(uint16_t);
				}

				para_value_def.sampling_freq_hz_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.sampling_freq_hz_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.doppler_sensitivity_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.doppler_sensitivity_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.motion_sensitivity_lower_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;
				para_value_def.motion_sensitivity_upper_boundary = protocol_read_payload_float32( &payload[0], offset);
				offset += TARGET_SIZE_FLOAT;

				para_value_def.number_of_skip_samples_lower_boundary = protocol_read_payload_uint32( &payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.number_of_skip_samples_upper_boundary = protocol_read_payload_uint32( &payload[0], offset);
				offset += sizeof(uint32_t);

				para_value_def.pulse_width_usec_lower_boundary = protocol_read_payload_uint32( &payload[0], offset);
				offset += sizeof(uint32_t);
				para_value_def.pulse_width_usec_upper_boundary = protocol_read_payload_uint32( &payload[0], offset);
				offset += sizeof(uint32_t);

				// check if offset and size matches!!

				/* send frame format to callback */
				pulse_para_values_def_cnf_cb(pulse_para_values_def_cnf_ctx, protocol_handle,
										   endpoint, &para_value_def);

				// free-up memory!!
				free(para_value_def.p_sample_list);

			}
			return 1;
		}
		else if (payloadId == MSG_RESULT_CNF)
		{
			if( payload_size > 1)
			{

				if (pulse_result_cnf_cb)
				{
					pulse_result_cnf_t result;

					uint16_t offset = 1;
					result.result_cnt = protocol_read_payload_uint32(&payload[0], offset);
					offset += sizeof(uint32_t);
					result.frame_number = protocol_read_payload_uint32(&payload[0], offset);
					offset += sizeof(uint32_t);
					result.velocity_mps = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);
					result.level = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);
					result.target_departing_bool = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.target_approaching_bool = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.motion_detected_bool = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.doppler_frequency_hz = protocol_read_payload_float32(&payload[0], offset);
					offset += sizeof(float);


					/* send result to callback */
					pulse_result_cnf_cb(pulse_result_cnf_ctx, protocol_handle,
											   endpoint, &result);

				}
			}
			return 1;
		}
		else  if ((payloadId == PULSE_MSG_GET_SHIELD_INFO_CNF) &&
							(payload_size > 1))
			{
				if (pulse_get_shield_info_cnf_cb)
				{
					pulse_get_shield_info_cnf_t result;

					uint16_t offset = 1;

					result.rf_frequency_khz = protocol_read_payload_uint32(&payload[0], offset);
					offset += sizeof(uint32_t);
					result.major_version = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.minor_version = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.num_tx_antenna = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.num_rx_antenna = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);


					uint32_t len = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.rf_shield_type_id = (char *)malloc(len * sizeof(char) + 1);
					for(size_t i = 0; i < len; i++)
					{
						result.rf_shield_type_id[i] = (char) protocol_read_payload_uint8(&payload[0], offset);
						offset += 1;
					}
					result.rf_shield_type_id[len] = 0;

					len = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.description = (char *)malloc(len * sizeof(char) + 1);
					for(size_t i = 0; i < len; i++)
					{
						result.description[i] = (char) protocol_read_payload_uint8(&payload[0], offset);
						offset += 1;
					}
					result.description[len] = 0;


					len = protocol_read_payload_uint8(&payload[0], offset);
					offset += sizeof(uint8_t);
					result.modulation = (char *)malloc(len * sizeof(char) + 1);
					for(size_t i = 0; i < len; i++)
					{
						result.modulation[i] = (char) protocol_read_payload_uint8(&payload[0], offset);
						offset += 1;
					}
					result.modulation[len] = 0;

					/* send result to callback */
					pulse_get_shield_info_cnf_cb(pulse_get_shield_info_cnf_ctx, protocol_handle,
											   endpoint, &result);

					free(result.rf_shield_type_id);
					free(result.description);
					free(result.modulation);

				}
				return 1;
			}
		else  if ((payloadId == MSG_GET_RAW_DATA_CNF) &&
									(payload_size > 18))
			{
				const uint16_t header_size = 18;

				if (get_raw_data_cnf_cb)
				{
					get_raw_data_cnf_t frame_info;

					uint32_t total_samples;
					uint32_t expected_message_size;

					/* read header info from payload */
					frame_info.frame_number =
											protocol_read_payload_uint32(payload,  1);
					frame_info.num_chirps = protocol_read_payload_uint32(payload,  5);
					frame_info.num_rx_antennas =
											protocol_read_payload_uint8 (payload,  9);
					frame_info.num_samples_per_chirp =
											protocol_read_payload_uint32(payload, 10);
					frame_info.rx_mask =    protocol_read_payload_uint8 (payload, 14);
					frame_info.data_format =
						  (Rx_Data_Format_t)protocol_read_payload_uint8 (payload, 15);
					frame_info.adc_resolution =
											protocol_read_payload_uint8 (payload, 16);
					frame_info.interleaved_rx =
											protocol_read_payload_uint8 (payload, 17);

					/* calculate expected message size */
					total_samples = frame_info.num_chirps *
									frame_info.num_samples_per_chirp *
									frame_info.num_rx_antennas *
									(frame_info.data_format == EP_RADAR_BASE_RX_DATA_REAL ? 1 : 2);

					expected_message_size = total_samples * frame_info.adc_resolution;
					expected_message_size = header_size +
											(expected_message_size >> 3) +
											((expected_message_size & 0x07) ? 1 : 0);

					if (payload_size == expected_message_size)
					{
						uint32_t sample_bit_mask = (1 << frame_info.adc_resolution)
													- 1;
						const float norm_factor = 1.f / (float)sample_bit_mask;
						float* sample_data = (float*)malloc(total_samples *
															sizeof(float));
						float* write_ptr = sample_data;

						uint16_t read_idx = header_size;
						uint16_t read_bit_position = 0;

						while (total_samples)
						{
							uint32_t current_sample =
							  protocol_read_payload_uint16(payload, read_idx) >>
							  read_bit_position;
							*write_ptr++ = (current_sample & sample_bit_mask) *
											norm_factor;

							read_bit_position += frame_info.adc_resolution;
							read_idx += read_bit_position >> 3;
							read_bit_position &= 0x07;

							--total_samples;
						}

						/* send frame info to callback */
						frame_info.sample_data = sample_data;
						get_raw_data_cnf_cb(get_raw_data_cnf_ctx, protocol_handle,
											endpoint, &frame_info);
						free(sample_data);
					}
				}
				return 1;
			}
		else
		{
			printf(" tfpu: message id not found\n");
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

int32_t ep_radar_s2glp_pulse_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_RadarS2GLP_definition);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ep_radar_s2glp_pulse_set_para_value_cb(pulse_para_values_cnf_callback_t callback,
									pulse_para_values_cnf_ctx_t* p_context)
{
	pulse_para_values_cnf_cb 		 = callback;
	pulse_para_values_cnf_ctx 	     = p_context;
}


void ep_radar_s2glp_pulse_set_para_def_cb(pulse_para_values_def_cnf_callback_t callback,
									pulse_para_values_def_cnf_ctx_t* p_context)
{
	pulse_para_values_def_cnf_cb 		 = callback;
	pulse_para_values_def_cnf_ctx 	 = p_context;
}

void ep_radar_s2glp_pulse_result_cb(pulse_result_cnf_callback_t callback,
									pulse_result_cnf_ctx_t* p_context)
{
	pulse_result_cnf_cb 	 = callback;
	pulse_result_cnf_ctx 	 = p_context;
}

void ep_radar_s2glp_pulse_get_shield_info_cb(pulse_get_shield_info_cnf_callback_t callback,
	pulse_get_shield_info_cnf_ctx_t* p_context)
{
	pulse_get_shield_info_cnf_cb 	 = callback;
	pulse_get_shield_info_cnf_ctx  = p_context;
}


void ep_radar_s2glp_pulse_get_raw_data_cb(get_raw_data_cnf_callback_t callback,
		void* p_context)
{
	get_raw_data_cnf_cb 	 = callback;
	get_raw_data_cnf_ctx 	 = p_context;
}



int32_t ep_radar_s2glp_pulse_para_value_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_VALUES_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_s2glp_pulse_para_value_def_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_PARA_VALUES_DEF_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_s2glp_pulse_para_set_value_req(int32_t protocol_handle, uint8_t endpoint, pulse_para_values_t data)
{
	/* build message to send */
	uint8_t cmd_message[256];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_PARA_SET_VALUES_REQ);
	offset += protocol_write_payload_float32(cmd_message, offset, data.max_speed_mps);
	offset += protocol_write_payload_float32(cmd_message, offset, data.min_speed_mps);
	offset += protocol_write_payload_float32(cmd_message, offset, data.frame_time_sec);

	offset += protocol_write_payload_uint16(cmd_message, offset, data.number_of_samples);
	offset += protocol_write_payload_float32(cmd_message, offset, data.sampling_freq_hz);
	offset += protocol_write_payload_float32(cmd_message, offset, data.doppler_sensitivity);
	offset += protocol_write_payload_float32(cmd_message, offset, data.motion_sensitivity);

	offset += protocol_write_payload_uint8(cmd_message, offset, data.use_high_gain_doppler_bool);
	offset += protocol_write_payload_uint8(cmd_message, offset, data.equistantant_mode);
	offset += protocol_write_payload_uint16(cmd_message, offset, data.number_of_skip_samples);
	offset += protocol_write_payload_uint32(cmd_message, offset, data.pulse_width_usec);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, offset);

}

/*
 *  if repeat_count is 0xFFFF.FFFF the results will be continuously provided, unless stopped (with repeat_count set to 0)
 *  if repeat_count is 1, only a single result is returned, and the next result needs to be retriggerd
 *  in case you like to avoid extra latency, by the round trip request, set the result cnt to  0xfffffffff, and a continuous stream of results will be received.
 */

int32_t ep_radar_s2glp_pulse_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_RESULT_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, offset);
}

int32_t ep_radar_s2glp_pulse_get_shield_info_req(int32_t protocol_handle, uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, PULSE_MSG_GET_SHIELD_INFO_REQ);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, offset);
}

int32_t ep_radar_s2glp_get_raw_data_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count)
{
	/* build message to send */
	uint8_t cmd_message[1+sizeof(uint32_t)];
	uint16_t offset = 0;

	offset += protocol_write_payload_uint8(cmd_message, offset, MSG_GET_RAW_DATA_REQ);
	offset += protocol_write_payload_uint32(cmd_message, offset, repeat_count);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_RadarS2GLP_definition,
									cmd_message, offset);
}

/* --- End of File -------------------------------------------------------- */
