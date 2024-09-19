/**
 * \file EndpointRadars2glpulse.c
 *
 * \brief Radar endpoint containing functionality pertinent to Sense2GoL Pulse
 *        Industrial radar demo kit.
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
#include "sense2gol_bsp_limits.h"
#include "EndpointRadars2glpulse.h"
#include "radar_pulse.h"
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
 * \brief This end point knows these message types.
 *
 * The first payload byte of every message is one of these codes.
 *
 * @{
 */

/** @} */

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

/*static */ pulse_para_values_t  g_parameter_pulse;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

static size_t packing_helper_12Bit(uint8_t *p_dest, uint16_t *p_src, size_t numSamples);

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

void send_result_response(uint8_t endpoint, pulse_result_response_t *p_pulse_result)
{
	static uint32_t result_cnt = 0; // zero init at startup!
	static uint8_t message[sizeof(pulse_result_response_t)+20];
	int16_t offset = 0;

	offset += wr_payload_u8(message, offset, MSG_RESULT_RES);
	offset += wr_payload_u32(message, offset, result_cnt);
	offset += wr_payload_u32(message, offset, p_pulse_result->frame_count);
	offset += wr_payload_float(message, offset, p_pulse_result->velocity_mps);
	offset += wr_payload_float(message, offset, p_pulse_result->level_nu);
	offset += wr_payload_u8(message, offset, p_pulse_result->target_departing_bool);
	offset += wr_payload_u8(message, offset, p_pulse_result->target_approaching_bool);
	offset += wr_payload_u8(message, offset, p_pulse_result->motion_detected_bool);
	offset += wr_payload_float(message, offset, p_pulse_result->doppler_frequency_hz);

	/* send message */
	protocol_send_header(endpoint, offset);
	protocol_send_payload(message, offset);
	protocol_send_tail();

	result_cnt += 1;
}

//============================================================================


void send_parameter_values(uint8_t endpoint, pulse_para_values_t *p_para)
{
	size_t offset = 0;
	static uint8_t message[256];

	offset += wr_payload_u8(message,    offset, MSG_PARA_VALUES_RES);
	offset += wr_payload_float(message, offset, p_para->max_speed_mps);
	offset += wr_payload_float(message, offset, p_para->min_speed_mps);
	offset += wr_payload_float(message, offset, p_para->frame_time_sec);

	offset += wr_payload_u16(message,   offset, p_para->number_of_samples_nu);
	offset += wr_payload_float(message, offset, p_para->sampling_freq_hz);
	offset += wr_payload_float(message, offset, p_para->doppler_sensitivity_nu);
	offset += wr_payload_float(message, offset, p_para->motion_sensitivity_nu);

	offset += wr_payload_u8(message,    offset, p_para->use_high_gain_doppler_bool);
	offset += wr_payload_u8(message,    offset, p_para->continuous_mode_bool);
	offset += wr_payload_u16(message,   offset, p_para->number_of_skip_samples_nu);
	offset += wr_payload_u32(message,   offset, p_para->pulse_width_usec);

	/* send message */
	protocol_send_header(endpoint, offset);
	protocol_send_payload(message, offset);
	protocol_send_tail();
}

//============================================================================

void send_pulse_frame_data(uint8_t endpoint, acq_buf_obj *p_acq_buf, device_settings_t *p_dev)
{
	/* send message containing frame data */
	uint32_t num_samples;
	uint32_t data_size;
	uint8_t message_header[18];

	uint32_t num_used_ant = 2;

	/* Global buffer to have a copy of fresh ADC data to avoid frame corruption at Host */
	static uint8_t transport_buffer[BSP_MAX_ADC_BUFFER_SIZE_BYTES];

	Frame_Info_t frame_info;

	frame_info.num_rx_antennas       = BSP_NUM_RX_ANTENNAS;
	frame_info.num_chirps            = p_acq_buf->params.num_of_chirps_per_frame;
	frame_info.rx_mask               = p_dev->rx_antenna_mask;
	frame_info.adc_resolution        = 12;
	frame_info.interleaved_rx        = 0;
	frame_info.frame_number          = p_acq_buf->frame_counter;
	frame_info.data_format           = RADAR_RX_DATA_COMPLEX;
	frame_info.num_samples_per_chirp = p_acq_buf->params.num_of_samples_per_chirp;

	/* Selection of Antenna to output is currently only supported for 12 Bit ADC resolution! */
	if (frame_info.rx_mask == 1)
	{
		num_used_ant = 1;
	}
	else if (frame_info.rx_mask == 2)
	{
		num_used_ant = 1;
	}
	else
		num_used_ant = 2;

	/* setup message header */
	wr_payload_u8(message_header, 0, MSG_GET_RAW_DATA_RES);
	wr_payload_u32(message_header, 1, frame_info.frame_number);
	wr_payload_u32(message_header, 5, frame_info.num_chirps);
	wr_payload_u8(message_header, 9, num_used_ant);
	wr_payload_u32(message_header, 10, frame_info.num_samples_per_chirp);
	wr_payload_u8(message_header, 14, frame_info.rx_mask);
	wr_payload_u8(message_header, 15, frame_info.data_format);
	wr_payload_u8(message_header, 16, frame_info.adc_resolution);
	wr_payload_u8(message_header, 17, frame_info.interleaved_rx);

	/* calculate size of data */
	/* calculate total number of bits and divide by 8 to get number of
	   bytes. Always round upwards */
	num_samples = frame_info.num_chirps *
				  frame_info.num_samples_per_chirp *
				  num_used_ant *
				  (frame_info.data_format == RADAR_RX_DATA_REAL ? 1 : 2);

	data_size = num_samples * frame_info.adc_resolution;
	data_size = (data_size >> 3) + ((data_size & 0x07) ? 1 : 0);

	/* send message header */
	protocol_send_header(endpoint, data_size + sizeof(message_header));
	protocol_send_payload(message_header, sizeof(message_header));

	uint8_t *destPtr = &transport_buffer[0];
	uint8_t *srcPtr = p_acq_buf->p_acq_buf;

	uint32_t channel_inc = frame_info.num_samples_per_chirp * sizeof(uint16_t);
	uint32_t chirp_inc = channel_inc * frame_info.num_rx_antennas * (frame_info.data_format == RADAR_RX_DATA_REAL ? 1 : 2);

	uint32_t processed_out_bytes = 0;

	for (uint32_t chirp = 0; chirp < frame_info.num_chirps; chirp++)
	{
		for (uint32_t ant_idx = 0; ant_idx < frame_info.num_rx_antennas; ant_idx++) // max Ant mask!
		{
			uint8_t mask = (1 << ant_idx);
			if ((mask & frame_info.rx_mask) != 0)
			{
				uint32_t num_chan_per_ant = (frame_info.data_format == RADAR_RX_DATA_REAL ? 1 : 2);
				for (uint32_t ch_idx = 0; ch_idx < num_chan_per_ant; ch_idx++)
				{
					uint32_t in_offset = (chirp_inc * chirp) + (channel_inc * (ant_idx * num_chan_per_ant + ch_idx));
					size_t out_offset = packing_helper_12Bit(destPtr, (uint16_t *)&srcPtr[in_offset], frame_info.num_samples_per_chirp);

					protocol_send_payload((uint8_t*)destPtr, out_offset);

					destPtr += out_offset;
					processed_out_bytes += out_offset;
				}
			}
		}
	}

	protocol_send_tail();
}

//============================================================================

uint16_t ep_radar_s2glpulse_handle_message(uint8_t endpoint, uint8_t *message_data,
										   uint16_t num_bytes, void* context)
{
	switch (message_data[0])
	{
	case MSG_PARA_VALUES_IND:
		if (num_bytes == 1)
		{
			copyStore2comPara(&g_parameter_pulse);
			send_parameter_values(endpoint, &g_parameter_pulse);

			return RADAR_ERR_OK;
		}
		break;

	case MSG_PARA_VALUES_DEF_IND:
		if (num_bytes == 1)
		{
			static uint8_t message[512]; // allocate statically, rather then on the call stack!
			memset(message, 0, sizeof(message));

			size_t offset = 0;

			offset += wr_payload_u8(message, 0, MSG_PARA_VALUES_DEF_RES);
			offset += sizeof(uint16_t); // size if uint16

			offset += wr_payload_float(message, offset, c_para_value_def_list.max_speed_mps_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.max_speed_mps_upper_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.min_speed_mps_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.min_speed_mps_upper_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.frame_time_sec_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.frame_time_sec_upper_boundary);

			uint8_t size_elems = sizeof(c_para_value_def_list.sample_list) / sizeof(uint16_t);
			offset += wr_payload_u8(message, offset, size_elems);
			for (int n = 0; n < size_elems; n++)
			{
				offset += wr_payload_u16(message, offset, c_para_value_def_list.sample_list[n]);
			}

			offset += wr_payload_float(message, offset, c_para_value_def_list.sampling_freq_hz_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.sampling_freq_hz_upper_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.doppler_sensitivity_nu_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.doppler_sensitivity_nu_upper_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.motion_sensitivity_nu_lower_boundary);
			offset += wr_payload_float(message, offset, c_para_value_def_list.motion_sensitivity_nu_upper_boundary);

			offset += wr_payload_u32(message, offset, c_para_value_def_list.number_of_skip_samples_nu_lower_boundary);
			offset += wr_payload_u32(message, offset, c_para_value_def_list.number_of_skip_samples_nu_upper_boundary);

			offset += wr_payload_u32(message, offset, c_para_value_def_list.pulse_width_usec_lower_boundary);
			offset += wr_payload_u32(message, offset, c_para_value_def_list.pulse_width_usec_upper_boundary);

			// write size values of the message!
			wr_payload_u16(message, 1, (uint16_t)offset);

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();

			return RADAR_ERR_OK;
		}
		break;

	case MSG_PARA_SET_VALUES_IND:
		if (num_bytes == 35)
		{
			size_t offset = 1; // skip id

			g_parameter_pulse.max_speed_mps = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.min_speed_mps = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.frame_time_sec = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.number_of_samples_nu = rd_payload_u16(message_data, offset);
			offset += sizeof(uint16_t);
			g_parameter_pulse.sampling_freq_hz = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.doppler_sensitivity_nu = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.motion_sensitivity_nu = rd_payload_float(message_data, offset);
			offset += sizeof(float);
			g_parameter_pulse.use_high_gain_doppler_bool = rd_payload_u8(message_data, offset);
			offset += sizeof(uint8_t);
			g_parameter_pulse.continuous_mode_bool = rd_payload_u8(message_data, offset);
			offset += sizeof(uint8_t);
			g_parameter_pulse.number_of_skip_samples_nu = rd_payload_u16(message_data, offset);
			offset += sizeof(uint16_t);
			g_parameter_pulse.pulse_width_usec = rd_payload_u32(message_data, offset);
			offset += sizeof(uint32_t);

			uint32_t status = copyComPara2Store(&g_parameter_pulse);
			if (status)
			{
				/* send message */
				send_parameter_values(endpoint, &g_parameter_pulse);

				/* inform other endpoints of the change */
				Radar_Handle_t radar_driver = (Radar_Handle_t)context;

				protocol_broadcast_change(radar_driver, EP_RADAR_CHNG_CURRENT_CONSUMPTION);
				return RADAR_ERR_OK;
			}
			else
			{
				return RADAR_ERR_PARAMETER_OUT_OF_RANGE;
			}
		}
		/*
			else
				error("wrong message size!");
			*/
		break;

	case MSG_RESULT_IND:
		if (num_bytes == 5)
		{
			uint16_t in_offset = 1;
			uint32_t repeat_count = rd_payload_u32(message_data, in_offset);

			set_result_repeat_count(endpoint, repeat_count);

			return RADAR_ERR_OK;
		}
		break;

	case MSG_GET_SHIELD_INFO_IND:
		if (num_bytes == 1)
		{

			static uint8_t message[512];
			uint16_t offset = 0;

			uint32_t index = ds_get_board_variant();

			if (index == 0 || index > sizeof(rf_shield_info) / sizeof(rf_shield_info[0]) + 1)
			{
				index = 1;
			}

			pulse_rf_shield_info_t *p_info = &rf_shield_info[index - 1];

			offset += wr_payload_u8(message, offset, MSG_GET_SHIELD_INFO_RES);
			offset += wr_payload_u32(message, offset, p_info->rf_frequency_khz);
			offset += wr_payload_u8(message, offset, p_info->major_rf_shield_version);
			offset += wr_payload_u8(message, offset, p_info->minor_rf_shield_version);
			offset += wr_payload_u8(message, offset, p_info->num_tx_antennas);
			offset += wr_payload_u8(message, offset, p_info->num_rx_antennas);

			offset += wr_payload_string(message, offset, p_info->rf_shield_board_id);
			offset += wr_payload_string(message, offset, p_info->description);
			offset += wr_payload_string(message, offset, p_info->modulation);

			/* send message */
			protocol_send_header(endpoint, offset);
			protocol_send_payload(message, offset);
			protocol_send_tail();

			return RADAR_ERR_OK;
		}
		break;

	case MSG_GET_RAW_DATA_IND:
		if (num_bytes == 5)
		{
			uint16_t in_offset = 1;
			uint32_t repeat_count = rd_payload_u32(message_data, in_offset);

			set_raw_data_repeat_count(endpoint, repeat_count);

			return RADAR_ERR_OK;
		}
		break;

	default:
		break;
	}
	return PROTOCOL_STATUS_INVALID_PAYLOAD;
}

//============================================================================

void ep_radar_s2glpulse_handle_change(uint8_t endpoint, void* context, uint32_t what)
{
	Radar_Handle_t radar_driver = (Radar_Handle_t)context;

	(void)radar_driver; /* Suppress compiler warnings of unused variable */

	switch (what)
	{
	default:
		break;
	}
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

static size_t packing_helper_12Bit(uint8_t *p_dest, uint16_t *p_src, size_t numSamples)
{
	/* numBytes needs to be divided by 2 */
	if ((numSamples & 1) != 0)
		return -1;

	size_t   processed_output_bytes = 0;

	while (numSamples >= 2)
	{
		/* pack two samples into one 32 bit word */
		uint32_t packed_word;
		packed_word  = *p_src++;
		packed_word |= *p_src++ << 12;

		/* write packed word into buffer */
		wr_payload_u32(p_dest, 0, packed_word);
		p_dest += 3;

		numSamples -= 2;
		processed_output_bytes += 3;
	}

	return processed_output_bytes;
}
/* --- End of File -------------------------------------------------------- */
