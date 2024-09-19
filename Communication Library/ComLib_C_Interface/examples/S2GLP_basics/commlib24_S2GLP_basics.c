/**
   @file: commlib24_S2GLP_basics.c

   This file implements a short test of the communication library. The library
   is used to connect to a sensor board, retrieve some raw radar data and
   print this data to the console. The raw radar data is printed to standard
   output. Information messages are printed to standard error output.

   If multiple boards are connected to the host, the this test connects to the
   board with the smallest COM port number.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "COMPort.h"
#include "Protocol.h"
#include "EndpointRadarBaseBoard.h"
#include "EndpointRadarS2GLP.h"

/*
==============================================================================
   2. DATA
==============================================================================
*/

typedef struct
{
	uint8_t used_rx[8];
	uint8_t used_rx_count;
} RX_Info_t;

typedef struct {
	bool show_raw_data_header;
	bool show_raw_data;
} Show_Raw_Data_Options_t;

static int32_t protocol_handle = -1;
static uint8_t endpoint_base_board = 0;
static uint8_t endpoint_s2glp_shield = 0;

/*
==============================================================================
   3. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * This function retrieves the list of available COM ports and tries to open
 * the COM ports one after the other until a compatible port has been found.
 *
 * @return If a compatible endpoint has been found, a protocol handle to that
 *         port is returned. Otherwise, a negative error code is returned.
 */
static int32_t connect_to_sensor(void);

/**
 * This function retrieves the list of available communication endpoints and
 * assigns them to the global handles.
 *
 * @return If all needed endpoints for this demo are found positive return
 * 		   code is returned. Otherwise, a negative number is returned.
 */
static int32_t query_communications_endpoints(void);

/**
 * This function configures the chip the needed configuration for this
 * demo. The settings are the power on reset default settings of the chip.
 *
 * @return If configuration is successful returned error code is equal to 0.
 * 		   Otherwise, a non-zero error code is returned.
 */
static int32_t configure_sensor(void);

/**
 * This function processes the RX info
 *
 * @param[in] rx_mask	A mask to enable RX antennas and demodulation
 *                      circuitry. Each bit of the mask represents an
 *                      available RX antenna. If a bit is set, the
 *                      according bit is activated.
 * @param[in] rx_info	Rx antenna info
 */
static void process_rx_info(uint8_t rx_mask, RX_Info_t *rx_info);

/**
 * This function prints the sampled data which can be found in frame_info->sample_data,
 * and is called every time ep_radar_base_get_frame_data() method is called to return 
 * measured time domain signals.
 *
 * @param[in] frame_info	This is the struct holding all information 
 *                          about a single frame of radar data.
 */
static void process_received_data(const get_raw_data_cnf_t* frame_info);

/**
 * This function prints an error message in case something goes wrong and
 * disconnects the sensor.
 *
 * @param[in] err_code	This is the error code received from the
 * 						operation that raised it.
 */
static void on_error(int32_t err_code);

/*
==============================================================================
   4. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t connect_to_sensor(void)
{
	char com_port_list[512];
	char *current_port;

	/* retrieve a list of all available com ports */
	com_port_list[0] = 0;
	com_get_port_list(com_port_list, sizeof(com_port_list));

	/* iterate over the port list */
	current_port = com_port_list;
	while (current_port[0])
	{
		int32_t protocol_handle;

		/*
         * find end of current port name in port list
         * (port name is delimited by other ';' or '\0'
         */
		char *next_port = current_port;
		while (next_port[0])
		{
			if (next_port[0] == ';')
			{
				/* replace ';' by '\0' to indicate end of current port name */
				*next_port = 0;

				/* after a ';' another port name follows */
				++next_port;
				break;
			}
			++next_port;
		}

		/* try to connect to current COM port */
		printf("Trying to open COM port: %s ... ", current_port);
		protocol_handle = protocol_connect(current_port);

		if (protocol_handle >= 0)
		{
			printf("OK.\n");
			return protocol_handle;
		}
		else
		{
			printf("Failed.\n");
		}

		/* prepare for next iteration */
		current_port = next_port;
	}

	return -1;
}

static int32_t query_communications_endpoints(void)
{
	int32_t num_endpoints;
	uint8_t i;

	/* check number of endpoints */
	num_endpoints = protocol_get_num_endpoints(protocol_handle);
	if (num_endpoints < 0)
	{
		printf("Error: Could not find any endpoint.\n");
		return -1;
	}

	printf("Number of detected endpoints: %d\n", num_endpoints);
	/* search for endpoint radar base */
	printf("Searching for endpoint \"Radar Base\" ... ");
	for (i = 1; i <= num_endpoints; ++i)
	{
		if (ep_radar_base_board_is_compatible_endpoint(protocol_handle, i) == 0)
		{
			endpoint_base_board = i;
			break;
		}
	}
	printf("%s\n", (endpoint_base_board != 0) ? "OK." : "Failed!");

	/* search for endpoint radar base */
	printf("Searching for endpoint \"%s\" ... ", "S2GLP-Endpoint");
	for (i = 1; i <= num_endpoints; ++i)
	{
		if (ep_radar_s2glp_pulse_is_compatible_endpoint(protocol_handle, i) == 0)
		{
			endpoint_s2glp_shield = i;
			break;
		}
	}
	printf("%s\n", (endpoint_s2glp_shield != 0) ? "OK." : "Failed!");

	if (endpoint_base_board == 0 || endpoint_s2glp_shield == 0)
	{
		printf("Error: Not all required communication endpoints found for this demo\n");
		return -1;
	}
	else
	{
		return 1;
	}
}

static int32_t configure_sensor(void)
{
	int32_t return_code = 0;

	return return_code;
}

static void process_rx_info(uint8_t rx_mask, RX_Info_t *rx_info)
{
	int i = 0;
	while (i < 8)
	{
		if (((rx_mask >> i) & 0x01) == 1)
		{
			rx_info->used_rx[rx_info->used_rx_count] = i + 1;
			rx_info->used_rx_count++;
		}
		i++;
	}
}

static void process_received_data(const get_raw_data_cnf_t* frame_info)
{
	/**
	* for complex data format (default):
	* 		-----------------------------------------------------------------------------------------------------------------
	* 		|			ADC 1			|			ADC 2			|			ADC 1			|			ADC 2			|
	* 		-----------------------------------------------------------------------------------------------------------------
	* 		|							RX 1						|							RX 2						|
	* 		-----------------------------------------------------------------------------------------------------------------
	* 		|				One Chirp data size  = Sample per Chirp * Num of RX Antennas * Number of channels				|
	* 	 	-----------------------------------------------------------------------------------------------------------------
	* */
	RX_Info_t rx_info = {{0}, 0};

	process_rx_info(frame_info->rx_mask, &rx_info);

	uint32_t num_chan_per_ant = (frame_info->data_format == EP_RADAR_BASE_RX_DATA_REAL ? 1 : 2);
	uint32_t chirp_inc = frame_info->num_samples_per_chirp * num_chan_per_ant * frame_info->num_rx_antennas;
	uint32_t total_sample_count = frame_info->num_chirps *
								  frame_info->num_samples_per_chirp *
								  frame_info->num_rx_antennas *
								  num_chan_per_ant;
	uint8_t chirp_nr = 0;
	uint8_t rx_nr = 0;
	uint8_t adc_nr = 0;
	uint8_t sample_nr = 0;

	for (uint32_t i = 0; i < total_sample_count; i++)
	{
		chirp_nr = (i / chirp_inc);
		rx_nr = ((i % chirp_inc) / (num_chan_per_ant * frame_info->num_samples_per_chirp));
		adc_nr = ((i % (chirp_inc / rx_info.used_rx_count)) / (frame_info->num_samples_per_chirp));
		sample_nr = (i % frame_info->num_samples_per_chirp);
		printf("\t\t");
		printf("%d, ", frame_info->frame_number);
		printf("%d, ", chirp_nr);
		printf("%d, ", rx_info.used_rx[rx_nr]);
		printf("%d, ", adc_nr);
		printf("%d, ", sample_nr);
		printf("%f\n", frame_info->sample_data[i]);
	}
	return;
}

static void on_error(int32_t err_code)
{
	printf("Error Code 0x%06x: %s\n", err_code,
		   protocol_get_status_code_description(protocol_handle,
												err_code));

	/* Close the connection */
	/* ----------------------------- */
	printf("Connection is closed now.\n ");
	if (protocol_handle >= 0)
	{
		protocol_disconnect(protocol_handle);
	}
}

/*
==============================================================================
   5. USER CALLBACK FUNCTIONS
==============================================================================
 */

void mcubase_consumption_cnf_callback(consumption_cnf_ctx_t *p_context, int32_t protocol_handle,
									  uint8_t endpoint, consumption_cnf_t *p_consumption)
{
	printf("mcubase_consumption_cnf_callback called\n");
	printf("\telements: %d\n", p_consumption->elements);
	for (size_t i = 0; i < p_consumption->elements; i++)
	{
		consumption_t *pData = (p_consumption->p_consumption + i);
		printf("\t\t%d: ", pData->index);
		printf("%f %s\n", pData->value, pData->p_unit);
	}
	fflush(stdout);
}

void mcubase_consumption_def_cnf_callback(consumption_def_cnf_ctx_t *p_context, int32_t protocol_handle,
										  uint8_t endpoint, consumption_def_host_t *p_consumption)
{
	printf("mcubase_consumption_def_cnf_callback called\n");
	printf("\telements: %d\n", p_consumption->consumption_label_elements);
	for (size_t i = 0; i < p_consumption->consumption_label_elements; i++)
	{
		printf("\t\t%s\n", &p_consumption->pp_consumption_labels[i][0]);
	}
	fflush(stdout);
}

void mcubase_para_store_callback(para_store_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, para_store_cnf_t *p_data)
{
	printf("mcubase_para_store_callback called\n");
	fflush(stdout);
}

void mcubase_para_factory_reset_callback(para_factory_reset_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, para_factory_reset_t *p_data)
{
	printf("mcubase_para_factory_reset_callback called\n");
	fflush(stdout);
}

void mcubase_board_info_callback(board_info_cnf_ctx_t *p_context, int32_t protocol_handle, uint8_t endpoint, board_info_cnf_t *p_data)
{
	printf("mcubase_board_info_callback called\n");
	printf("\tmajor version: %d\n", p_data->major_version);
	printf("\tminor version: %d\n", p_data->minor_version);
	printf("\trf shield type id: %s\n", p_data->rf_shield_type_id);
	printf("\tdescription: %s\n", p_data->description);
	fflush(stdout);
}

static void pulse_para_value_callback(pulse_para_values_cnf_ctx_t *p_context, int32_t protocol_handle,
									  uint8_t endpoint, pulse_para_values_t *ps_para_values)
{
	pulse_para_values_t *pData = ps_para_values;

	printf("pulse_para_value_callback:\n");

	printf("\t: max_speed_mps:   %f\n", pData->max_speed_mps);
	printf("\t: min_speed_mps:   %f\n", pData->min_speed_mps);
	printf("\t: frame_time_sec: %f\n", pData->frame_time_sec);

	printf("\t: number_of_samples: %d\n", pData->number_of_samples);
	printf("\t: sampling_freq_hz:   %f\n", pData->sampling_freq_hz);
	printf("\t: doppler_sensitivity:   %f\n", pData->doppler_sensitivity);
	printf("\t: motion_sensitivity: %f\n", pData->motion_sensitivity);

	printf("\t: number_of_skip_samples: %d\n", pData->number_of_skip_samples);

	printf("\t: pulse_width_usec: %d\n", pData->pulse_width_usec);

	float rf_frequency_khz = 24050000.0; // from rf_shield_info message
	float phy_velocity_max_mps = (pData->sampling_freq_hz) * (1000.0f * 300.0f) / (4.0f * rf_frequency_khz);
	float phy_velocity_min_mps = (pData->sampling_freq_hz) * (1000.0f * 300.0f) / (pData->number_of_samples * 2.0f * rf_frequency_khz);

	printf("\t: rf_frequency: %.1f kHz \n", rf_frequency_khz);
	printf("\t: min. physical_speed_mps: %f m/s; %f km/h\n ", phy_velocity_min_mps, phy_velocity_min_mps * 3.6);
	printf("\t: max. physical_speed_mps: %f m/s; %f km/h\n ", phy_velocity_max_mps, phy_velocity_max_mps * 3.6);

	fflush(stdout);
}

static void pulse_para_value_def_callback(pulse_para_values_def_cnf_ctx_t *p_context, int32_t protocol_handle,
										  uint8_t endpoint, pulse_para_value_def_host_t *ps_para_values)
{
	pulse_para_value_def_host_t *pData = ps_para_values;

	printf("pulse_para_value_def_callback:\n");

	printf("\t: max_speed_mps_lower_boundary: %f \n", pData->max_speed_mps_lower_boundary);
	printf("\t: max_speed_mps_upper_boundary: %f \n", pData->max_speed_mps_upper_boundary);

	printf("\t: min_speed_mps_lower_boundary: %f \n", pData->max_speed_mps_lower_boundary);
	printf("\t: min_speed_mps_upper_boundary: %f \n", pData->min_speed_mps_upper_boundary);

	printf("\t: frame_time_sec_lower_boundary: %f \n", pData->frame_time_sec_lower_boundary);
	printf("\t: frame_time_sec_upper_boundary: %f \n", pData->frame_time_sec_upper_boundary);

	printf("\t: size_elems: %d \n", pData->size_elems);
	for (int i = 0; i < pData->size_elems; i++)
	{
		printf("\t\t%d\n", pData->p_sample_list[i]);
	}

	printf("\t: sampling_freq_hz_lower_boundary: %f \n", pData->sampling_freq_hz_lower_boundary);
	printf("\t: sampling_freq_hz_upper_boundary: %f \n", pData->sampling_freq_hz_upper_boundary);

	printf("\t: doppler_sensitivity_lower_boundary: %f \n", pData->doppler_sensitivity_lower_boundary);
	printf("\t: doppler_sensitivity_upper_boundary: %f \n", pData->doppler_sensitivity_upper_boundary);

	printf("\t: motion_sensitivity_lower_boundary: %f \n", pData->motion_sensitivity_lower_boundary);
	printf("\t: motion_sensitivity_upper_boundary: %f \n", pData->motion_sensitivity_upper_boundary);

	printf("\t: number_of_skip_samples_lower_boundary: %d \n", pData->number_of_skip_samples_lower_boundary);
	printf("\t: number_of_skip_samples_upper_boundary: %d \n", pData->number_of_skip_samples_upper_boundary);

	printf("\t: pulse_width_usec_lower_boundary: %d \n", pData->pulse_width_usec_lower_boundary);
	printf("\t: pulse_width_usec_upper_boundary: %d \n", pData->pulse_width_usec_upper_boundary);

	fflush(stdout);
}

void pulse_result_cnf_callback(pulse_result_cnf_ctx_t *p_context, int32_t protocol_handle,
							   uint8_t endpoint, pulse_result_cnf_t *p_result)
{
	printf("pulse_result_cnf_callback called\n");

	printf("\tresult cnt: %d\n", p_result->result_cnt);
	printf("\tframe_number: %d\n", p_result->frame_number);

	printf("\tvelocity [m/s]: %f\n", p_result->velocity_mps);
	printf("\tlevel: %f\n", p_result->level);

	printf("\ttarget_departing: %s\n", p_result->target_departing_bool ? "true" : "false");
	printf("\ttarget_approaching: %s\n", p_result->target_approaching_bool ? "true" : "false");
	printf("\tmotion_detected: %s\n", p_result->motion_detected_bool ? "true" : "false");
	printf("\tdoppler_frequency [Hz]: %f\n", p_result->doppler_frequency_hz);

	fflush(stdout);
}

void pulse_result_dir_cnf_callback(pulse_result_cnf_ctx_t *p_context, int32_t protocol_handle,
								   uint8_t endpoint, pulse_result_cnf_t *p_result)
{
	if (p_result->motion_detected_bool)
		printf("motion\t");
	if (p_result->target_departing_bool)
		printf("departing\t");
	if (p_result->target_approaching_bool)
		printf("approaching\t");

	printf("\n");
	fflush(stdout);
}

void pulse_get_shield_info_cnf_callback(pulse_get_shield_info_cnf_ctx_t *p_context, int32_t protocol_handle,
										uint8_t endpoint, pulse_get_shield_info_cnf_t *p_result)
{
	printf("pulse_get_shield_info_cnf_callback called\n");
	printf("\trf_frequency: %d kHz\n", p_result->rf_frequency_khz);
	printf("\tmajor version: %d\n", p_result->major_version);
	printf("\tminor version: %d\n", p_result->minor_version);
	printf("\tnum_tx_antenna: %d\n", p_result->num_tx_antenna);
	printf("\tnum_rx_antenna: %d\n", p_result->num_rx_antenna);
	printf("\trf shield type id: %s\n", p_result->rf_shield_type_id);
	printf("\tdescription: %s\n", p_result->description);
	printf("\tmodulation: %s\n", p_result->modulation);

	fflush(stdout);
}

void pulse_get_raw_data_cnf_callback(void *p_context, int32_t protocol_handle,
									 uint8_t endpoint, get_raw_data_cnf_t *p_raw_data)
{
	static int count = 0;

	Show_Raw_Data_Options_t *p_show_raw_data_options = (Show_Raw_Data_Options_t *)p_context;

	printf("pulse_get_raw_data_cnf_callback called: %d, %d\r\n", count, p_raw_data->frame_number);

	if (p_show_raw_data_options && p_show_raw_data_options->show_raw_data_header)
	{

		printf("\tframe number: %d\n", p_raw_data->frame_number);
		printf("\tnum_chirps: %d\n", p_raw_data->num_chirps);
		printf("\tnum_samples_per_chirp: %d\n", p_raw_data->num_samples_per_chirp);
		printf("\trx_mask: %d\n", p_raw_data->rx_mask);
	}

	if (p_show_raw_data_options && p_show_raw_data_options->show_raw_data)
	{
		printf(" ----------------- raw data -------------------------\n");
		process_received_data(p_raw_data);
		printf(" ----------------------------------------------------\n");
	}
	count++;

	fflush(stdout);
}

/*
==============================================================================
   6. MAIN METHOD
==============================================================================
 */

int main()
{
	int32_t return_code = 0;

	/* connect to a sensor board */
	/* ------------------------- */
	protocol_handle = connect_to_sensor();
	if (protocol_handle < 0)
	{
		printf("Error: Could not find a sensor board.\n");
		return protocol_handle;
	}

	/* search for relevant endpoints */
	/* ----------------------------- */
	if (query_communications_endpoints() == 1)
	{
		/* configure chip to power on reset parameters */
		return_code = configure_sensor();
		if (return_code & 0xFFFF)
		{
			on_error(return_code);
			return (return_code & 0xFFFF);
		}

		printf("---- register callbacks ----\n");

		ep_radar_s2glp_pulse_get_shield_info_cb(pulse_get_shield_info_cnf_callback, NULL);
		ep_radar_s2glp_pulse_get_raw_data_cb(pulse_get_raw_data_cnf_callback, &(Show_Raw_Data_Options_t){.show_raw_data_header = true, .show_raw_data = true});

		ep_radar_s2glp_pulse_set_para_value_cb(pulse_para_value_callback, NULL);
		ep_radar_s2glp_pulse_set_para_def_cb(pulse_para_value_def_callback, NULL);
		ep_radar_s2glp_pulse_result_cb(pulse_result_cnf_callback, NULL);

		ep_radar_base_board_consumption_cb(mcubase_consumption_cnf_callback, NULL);
		ep_radar_base_board_consumption_def_cb(mcubase_consumption_def_cnf_callback, NULL);
		ep_radar_base_board_para_store_cb(mcubase_para_store_callback, NULL);
		ep_radar_base_board_para_factory_reset_cb(mcubase_para_factory_reset_callback, NULL);
		ep_radar_base_board_board_info_cb(mcubase_board_info_callback, NULL);

		{
			printf("Trying to get a device info ...");

			Firmware_Information_t fw_info;
			return_code = protocol_get_firmware_information(protocol_handle, &fw_info);

			printf("%s\n", ((return_code & 0xFFFF) == 0) ? "OK." : "Failed!");
			if ((return_code & 0xFFFF))
			{
				goto ON_ERROR;
			}

			printf("FW info: \"%s\": %d.%d.%d\n", fw_info.description, fw_info.version_major, fw_info.version_minor, fw_info.version_build);
			fflush(stdout);
		}

		// request new parameter values
		printf("---- pulse: request parameter values ----\n");
		ep_radar_s2glp_pulse_para_value_req(protocol_handle, endpoint_s2glp_shield);

		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- base board: factory reset ----\n");
		ep_radar_base_board_para_factory_reset_req(protocol_handle, endpoint_base_board);
		fflush(stdout);
		Sleep(500);
		protocol_poll_receive_once(protocol_handle);

		printf("---- pulse: get shield info ----\n");
		ep_radar_s2glp_pulse_get_shield_info_req(protocol_handle, endpoint_s2glp_shield);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- pulse: parameter definition ----\n");
		ep_radar_s2glp_pulse_para_value_def_req(protocol_handle, endpoint_s2glp_shield);
		fflush(stdout);
		Sleep(500);
		protocol_poll_receive_once(protocol_handle);

		// request parameter values
		printf("---- pulse: request parameter values ----\n");
		ep_radar_s2glp_pulse_para_value_req(protocol_handle, endpoint_s2glp_shield);
		Sleep(500);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		// set the new values
		{
			printf("---- pulse: set new values 64 ----\n");
			int32_t result = ep_radar_s2glp_pulse_para_set_value_req(protocol_handle, endpoint_s2glp_shield,
																	 (pulse_para_values_t){
																		 .max_speed_mps = 1.22F,
																		 .min_speed_mps = 1.0F,
																		 .frame_time_sec = 0.500F,
																		 .number_of_samples = 64,
																		 .sampling_freq_hz = 2000.0F,
																		 .doppler_sensitivity = 800,
																		 .motion_sensitivity = 400,
																		 .use_high_gain_doppler_bool = 1,
																		 .equistantant_mode = 0,
																		 .number_of_skip_samples = 100,
																		 .pulse_width_usec = 4});
			printf("req_result: 0x%04x, %s\n", result & 0xFFFF,
				   protocol_get_status_code_description(protocol_handle, result));
			Sleep(10);
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}

		/* set the new values */
		{
			printf("---- pulse: set new values 32 ----\n");
			int32_t result = ep_radar_s2glp_pulse_para_set_value_req(protocol_handle, endpoint_s2glp_shield,
																	 (pulse_para_values_t){
																		 .max_speed_mps = 1.22F,
																		 .min_speed_mps = 1.0F,
																		 .frame_time_sec = 0.500F,
																		 .number_of_samples = 32,
																		 .sampling_freq_hz = 2000.0F,
																		 .doppler_sensitivity = 800,
																		 .motion_sensitivity = 400,
																		 .use_high_gain_doppler_bool = 1,
																		 .equistantant_mode = 0,
																		 .number_of_skip_samples = 100,
																		 .pulse_width_usec = 4});
			printf("req_result: 0x%04x, %s\n", result & 0xFFFF,
				   protocol_get_status_code_description(protocol_handle, result));
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}

		/* request new parameter values */
		printf("---- pulse: request parameter values ----\n");
		ep_radar_s2glp_pulse_para_value_req(protocol_handle, endpoint_s2glp_shield);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- pulse: get 5x results ----\n");
		ep_radar_s2glp_pulse_result_req(protocol_handle, endpoint_s2glp_shield, 5);
		fflush(stdout);
		for (int i = 0; i < 5; i++)
		{
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}
		fflush(stdout);

		printf("---- pulse: get shield info ----\n");
		ep_radar_s2glp_pulse_get_shield_info_req(protocol_handle, endpoint_s2glp_shield);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		/* factory reset*/
		printf("---- set to default values again ----\n");
		ep_radar_base_board_para_factory_reset_req(protocol_handle, endpoint_base_board);

		ep_radar_s2glp_pulse_result_req(protocol_handle, endpoint_s2glp_shield, 30);
		printf("---- pulse: get raw data ----\n");
		ep_radar_s2glp_get_raw_data_req(protocol_handle, endpoint_s2glp_shield, 10);
		for (int i = 0; i < 20; i++)
		{
			protocol_poll_receive_once(protocol_handle);
			fflush(stdout);
		}
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- check base board endpoint ----\n");
		{
			/* get version of endpoint to understand, which structure elements are valid! */
			Endpoint_Info_t endpoint_info;

			if (protocol_get_endpoint_info(protocol_handle, endpoint_base_board, &endpoint_info) > 0)
			{
				/* error */
			}
			else
			{
				printf("endpoint version: %d\n", endpoint_info.version);
				printf("endpoint type: 0x%08x, (%c%c%c%c)\n", endpoint_info.type,
					   (endpoint_info.type & 0xFF000000) >> 24,
					   (endpoint_info.type & 0x00FF0000) >> 16,
					   (endpoint_info.type & 0x0000FF00) >> 8,
					   (endpoint_info.type & 0x000000FF));
				printf("endpoint description: %s\n", endpoint_info.description);
			}
		}

		printf("---- base board: rf_board_id ----\n");
		ep_radar_base_board_board_info_req(protocol_handle, endpoint_base_board);
		protocol_poll_receive_once(protocol_handle);

		printf("---- base board: Consumption definition ----\n");
		/* request consumption */
		ep_radar_base_board_consumption_def_req(protocol_handle, endpoint_base_board);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		/* request results */
		printf("---- base board: Consumption result ----\n");
		ep_radar_base_board_consumption_req(protocol_handle, endpoint_base_board, 5);

		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- base board: factory reset ----\n");
		ep_radar_base_board_para_factory_reset_req(protocol_handle, endpoint_base_board);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);

		printf("---- base board: parameter store ----\n");
		ep_radar_base_board_para_store_req(protocol_handle, endpoint_base_board);
		protocol_poll_receive_once(protocol_handle);
		fflush(stdout);
	}

ON_ERROR:
	if ((return_code & 0xFFFF))
	{
		printf("Error Code 0x%06x: %d, %s\n", return_code, return_code,
			   protocol_get_status_code_description(protocol_handle, return_code));
	}

	/* finally, close the connection */
	/* ----------------------------- */
	printf("Connection is closed now.\n ");
	protocol_disconnect(protocol_handle);

	return (return_code & 0xFFFF);
}
