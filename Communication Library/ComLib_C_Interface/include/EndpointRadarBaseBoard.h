/**
 * \file EndpointRadarBaseboard.h
 *
 * \brief This file contains the API methods applicable generically to Industrial 
 *        radar Baseboard based Demos
 *
 */
 
/* ===========================================================================
 * Copyright (C) 2017-2021 Infineon Technologies AG
 * All rights reserved.
 * ===========================================================================
 *
 * ===========================================================================
 * This document contains proprietary information of Infineon Technologies AG.
 * Passing on and copying of this document, and communication of its contents
 * is not permitted without Infineon's prior written authorisation.
 * ===========================================================================
 */

#ifndef ENDPOINTRADARBASEBOARD_H_INCLUDED
#define ENDPOINTRADARBASEBOARD_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stddef.h> /* for size_t */
#include <stdint.h>
#include "EndpointRadarErrorCodes.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */		

/**
 * \defgroup EndpointRadarBaseBoard EndpointRadarBaseBoard
 *
 * \brief EndpointRadarBaseBoard documentation.
 *
 * @{
 */				   



typedef struct {
	size_t consumption_label_elements;
	char **pp_consumption_labels;
} consumption_def_host_t;


typedef struct {
	void *pData;		// currently the is nothing special to the data
} consumption_def_cnf_ctx_t;


typedef struct {
	int8_t index;   // the index marks the position, within the list of "consumptions" (see  fmcw_para_value_def_host_t.pp_consumption_labels)
	float value;	// the value of the consumption
	char *p_unit;	// the unit string of the consumption
} consumption_t;


typedef struct {
	size_t elements;
	consumption_t *p_consumption;
} consumption_cnf_t;


typedef struct {
	void *pData;
} consumption_cnf_ctx_t;

typedef struct {
	uint8_t major_version;
	uint8_t minor_version;
	char *rf_shield_type_id;
	char *description;
} board_info_cnf_t;


typedef struct {
	void *pData;
} board_info_cnf_ctx_t;

typedef struct {
	void* temp;
} para_store_cnf_t;


typedef struct {
	void *pData;
} para_store_cnf_ctx_t;

typedef struct {
	void* temp;
} para_factory_reset_t;


typedef struct {
	void *pData;
} para_factory_reset_cnf_ctx_t;


/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a Radar Baseboard
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
int32_t ep_radar_base_board_is_compatible_endpoint(int32_t protocol_handle,
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
* \param[in] p_data     		pointer to static memory of type consumption_def_host_t, holding
* 							   the consumption values.
*/
typedef void(*consumption_def_cnf_callback_t)(consumption_def_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, consumption_def_host_t *p_data);

typedef void(*consumption_cnf_callback_t)(consumption_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, consumption_cnf_t *p_data);

typedef void(*board_info_cnf_callback_t)(board_info_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, board_info_cnf_t *p_data);

typedef void(*para_store_cnf_callback_t)(para_store_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, para_store_cnf_t *p_data);

typedef void(*para_factory_reset_cnf_callback_t)(para_factory_reset_cnf_ctx_t *p_context, int32_t protocol_handle,
											uint8_t endpoint, para_factory_reset_t *p_data);
/**
* \brief This function is used to request the parameter values,
*
* results in callback of type fmcw_para_values_cnf_callback_t
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
int32_t ep_radar_base_board_consumption_def_req(int32_t protocol_handle, uint8_t endpoint);

void ep_radar_base_board_consumption_def_cb(consumption_def_cnf_callback_t callback,
											consumption_def_cnf_ctx_t *p_context);

int32_t ep_radar_base_board_consumption_req(int32_t protocol_handle, uint8_t endpoint, uint32_t repeat_count);

void ep_radar_base_board_consumption_cb(consumption_cnf_callback_t callback,
										consumption_cnf_ctx_t *p_context);

int32_t ep_radar_base_board_para_factory_reset_req(int32_t protocol_handle, uint8_t endpoint);

void ep_radar_base_board_para_factory_reset_cb(para_factory_reset_cnf_callback_t callback,
											   para_factory_reset_cnf_ctx_t *p_context);

int32_t ep_radar_base_board_para_store_req(int32_t protocol_handle, uint8_t endpoint);

void ep_radar_base_board_para_store_cb(para_store_cnf_callback_t callback,
									   para_store_cnf_ctx_t *p_context);

int32_t ep_radar_base_board_board_info_req(int32_t protocol_handle, uint8_t endpoint);

void ep_radar_base_board_board_info_cb(board_info_cnf_callback_t callback,
									   board_info_cnf_ctx_t *p_context);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARINDUSTRIAL_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
