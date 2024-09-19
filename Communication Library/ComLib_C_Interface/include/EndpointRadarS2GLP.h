/**
 * \file EndpointRadarS2GLP.h
 *
 * \brief This file contains the API methods applicable generically to 
 *         Industrial radar demo Sense2GoLPulse
 *
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

#ifndef ENDPOINTRADARS2GLP_H_INCLUDED
#define ENDPOINTRADARS2GLP_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>
#include "EndpointRadarErrorCodes.h"
#include "radar_common.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */		

/**
 * \defgroup EndpointRadarS2GLPulse EndpointRadarS2GLPulse
 *
 * \brief EndpointRadarS2GLPulse documentation.
 *
 * @{
 */				   
typedef struct {
	float 		max_speed_mps;
	float 		min_speed_mps;
	float 		frame_time_sec;
	uint16_t 	number_of_samples;
	float		sampling_freq_hz;
	float		doppler_sensitivity;
	float		motion_sensitivity;
    uint8_t 	use_high_gain_doppler_bool;
    uint8_t     equistantant_mode;
    uint16_t	number_of_skip_samples;
    uint32_t    pulse_width_usec;
} pulse_para_values_t;

typedef struct {
	void *pData;		// currently the is nothing special to the data
} pulse_para_values_cnf_ctx_t;


typedef struct
{
	uint16_t last_message_size;

	float max_speed_mps_lower_boundary;

	float max_speed_mps_upper_boundary;

	float min_speed_mps_lower_boundary;
	float min_speed_mps_upper_boundary;

	float frame_time_sec_lower_boundary;
	float frame_time_sec_upper_boundary;

	uint8_t   size_elems;
	uint16_t   *p_sample_list;

	float		sampling_freq_hz_lower_boundary;
	float		sampling_freq_hz_upper_boundary;

	float		doppler_sensitivity_lower_boundary;
	float		doppler_sensitivity_upper_boundary;

	float		motion_sensitivity_lower_boundary;
	float		motion_sensitivity_upper_boundary;

	uint32_t	number_of_skip_samples_lower_boundary;
	uint32_t	number_of_skip_samples_upper_boundary;

    uint32_t	pulse_width_usec_lower_boundary;
    uint32_t	pulse_width_usec_upper_boundary;

} pulse_para_value_def_host_t;

#define TARGET_SIZE_FLOAT (4)


typedef struct {
	void *pData;		// currently the is nothing special to the data
} pulse_para_values_def_cnf_ctx_t;


typedef struct {
	 uint32_t  result_cnt;
	 uint32_t  frame_number;
	 float     velocity_mps;    			/**< velocity of target detected in units of km/h (with sign!) */
	 float     level;  					/**< Absolute magnitude FFT spectrum values calculated by Doppler algorithm */
	 uint8_t   target_departing_bool;		/**< set to 1 if target is departing */
	 uint8_t   target_approaching_bool;		/**< set to 1 if target is approaching */
	 uint8_t   motion_detected_bool;		/**< set to 1 if motion is detected */
	 float	   doppler_frequency_hz;  		/**< doppler frequency, detected by algorithm */
} pulse_result_cnf_t;


typedef struct {
	void *pData;
} pulse_result_cnf_ctx_t;


typedef struct {
	uint32_t rf_frequency_khz;
	uint8_t  major_version;
	uint8_t  minor_version;
	uint8_t  num_tx_antenna;
	uint8_t  num_rx_antenna;

	char *rf_shield_type_id;
	char *description;
	char *modulation;
} pulse_get_shield_info_cnf_t;


typedef struct {
	void *pData;
} pulse_get_shield_info_cnf_ctx_t;

/* -----
 *
 */

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a Radar S2GL_Pulse
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
int32_t ep_radar_s2glp_pulse_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint);


/**
* \brief This is the callback type executed on a parameter value confirmation event,
*        triggered, by a parameter value request command.
*
* *
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] ps_para_value     pointer to static memory of type para_values_t, holding
* 							   the parameter values.
*/
typedef void(*pulse_para_values_cnf_callback_t)(pulse_para_values_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, pulse_para_values_t *ps_para_values);


/**
* \brief This is the callback type executed on a parameter value confirmation event,
*        triggered, by a parameter value request command.
*
* *
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] ps_para_value     pointer to static memory of type para_values_t, holding
* 							   the parameter values.
*/
typedef void(*pulse_para_values_def_cnf_callback_t)(pulse_para_values_def_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, pulse_para_value_def_host_t *ps_para_values);


typedef void(*pulse_result_cnf_callback_t)(pulse_result_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, pulse_result_cnf_t *ps_para_values);


typedef void(*pulse_get_shield_info_cnf_callback_t)(pulse_get_shield_info_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, pulse_get_shield_info_cnf_t *ps_temperature);






/**
* \brief This function is used to request the parameter values,
*
* results in callback of type para_values_cnf_callback_t
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
int32_t ep_radar_s2glp_pulse_para_value_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_s2glp_pulse_para_value_def_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_s2glp_pulse_para_set_value_req(int32_t protocol_handle, uint8_t endpoint, pulse_para_values_t data);

int32_t ep_radar_s2glp_pulse_result_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);

int32_t ep_radar_s2glp_pulse_get_shield_info_req(int32_t protocol_handle, uint8_t endpoint);

int32_t ep_radar_s2glp_get_temperature_req(int32_t protocol_handle, uint8_t endpoint, uint8_t index);

int32_t ep_radar_s2glp_get_tx_power_req(int32_t protocol_handle, uint8_t endpoint, uint8_t index);

int32_t ep_radar_s2glp_get_raw_data_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);

void ep_radar_s2glp_pulse_set_para_value_cb(pulse_para_values_cnf_callback_t callback,
								pulse_para_values_cnf_ctx_t* p_context);

void ep_radar_s2glp_pulse_set_para_def_cb(pulse_para_values_def_cnf_callback_t callback,
								pulse_para_values_def_cnf_ctx_t* p_context);

void ep_radar_s2glp_pulse_result_cb(pulse_result_cnf_callback_t callback,
								pulse_result_cnf_ctx_t* p_context);


void ep_radar_s2glp_pulse_get_shield_info_cb(pulse_get_shield_info_cnf_callback_t callback,
					pulse_get_shield_info_cnf_ctx_t* p_context);


void ep_radar_s2glp_pulse_get_raw_data_cb(get_raw_data_cnf_callback_t callback,
					void* p_context);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARS2GLP_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
