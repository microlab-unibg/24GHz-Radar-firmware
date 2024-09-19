/**
 * \file EndpointRadarMcuBase.h
 *
 * \brief Radar endpoint containing functionality pertinent to Industrial Radar Demo kits like D2G and P2G etc.
 *
 */

/* ===========================================================================
** Copyright (C) 2020-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_ENDPOINTRADARMCUBASE_H_
#define HOSTCOMMUNICATION_ENDPOINTRADARMCUBASE_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include "Protocol.h"
#include "ifxRadar.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define EP_RADAR_MCUBASE_LIST_ENTRY(context) \
{ \
    /*.endpoint_type    = */ 0x544A4241, /* ASCII code 'TJBA' */ \
    									 /*  = Tjuefire base */ \
    /*.endpoint_version = */ 1, \
    /*.handle_message   = */ ep_radar_mcu_base_handle_message, \
    /*.context          = */ context, \
    /*.handle_change    = */ ep_radar_mcu_base_handle_change \
}

typedef struct {
	size_t num_labels;
	char **p_consumption_labels;
} para_consumption_def_t;

typedef struct {
	int8_t index;   // the index marks the position, within the list of "consumptions" (see c_para_list.p_consumption_labels)
	float value;	// the value of the consumption
	char *p_unit;	// the unit string of the consumption
} consumption_response_t;

typedef struct {
	uint8_t major_version;
	uint8_t minor_version;
	char *rf_shield_type_id;
	char *description;

} board_info_cnf_t;

#define MSG_CONSUMPTION_DEF_IND			0xD0
#define MSG_CONSUMPTION_DEF_RES			0xD1	/**< A message containing the parameters variation */
#define MSG_CONSUMPTION_IND		    	0xD2    /**< A message to request the consumption */
#define MSG_CONSUMPTION_RES			    0xD3    /**< A message to provide the consumption */

#define MSG_RF_SHIELD_ID_IND			0xE0	/**< A message to request the rf shield information */
#define MSG_RF_SHIELD_ID_RES			0xE1	/**< A message to provide the rf shield information */

#define MSG_STORE_PARAMETER_IND			0xE2	/**< A message to store the parameter persistent */
#define MSG_STORE_PARAMETER_RES			0xE3	/**< A message to confirm that parameter are stored persistent */
#define MSG_PARA_FACTORY_RESET_IND		0xE4	/**< A message to reset parameters to default */
#define MSG_PARA_FACTORY_RESET_RES		0xE5

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

uint16_t send_consumption_response(uint8_t entries, consumption_response_t *pData);

consumption_response_t *get_consumption(void);

uint16_t ep_radar_mcu_base_handle_message(uint8_t endpoint, uint8_t *message_data,
										  uint16_t num_bytes, void* context);

void ep_radar_mcu_base_handle_change(uint8_t endpoint, void* context, uint32_t what);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_ENDPOINTRADARMCUBASE_H_ */

/* --- End of File -------------------------------------------------------- */
