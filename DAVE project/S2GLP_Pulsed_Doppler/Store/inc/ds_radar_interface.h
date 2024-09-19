/**
    @file  ds_radar_interface.h

    @brief This file includes the function declarations for ds_radar_interface.c file
*/

/* ===========================================================================
** Copyright (C) 2018-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

#ifndef DS_RADAR_INTERFACE_H_
#define DS_RADAR_INTERFACE_H_

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "radar_api.h"
#include "datastore.h"

/*
==============================================================================
   2. TYPES
==============================================================================
 */

/*
==============================================================================
   3. EXPORTED FUNCTION PROTOTYPES
==============================================================================
 */

/* ------------- ADC Endpoint ------------- */
/**
 * \brief	This function is used to get the current ADC configuration from data store.
 */
void ds_ep_fetch_adc_config(Adc_Configuration_t *ptr);

/**
 * \brief	This function is used to store the new ADC configuration.
 */
uint16_t ds_ep_store_adc_config(const Adc_Configuration_t *new_adc_config);

/* ------------- Base Endpoint ------------- */
/**
 * \brief	This function is used to get the device information and its capabilities.
 */
const Device_Info_t *ds_ep_get_device_info(void);

/**
 * \brief	This function is used to get the current Frame format.
 */
void ds_ep_fetch_frame_format(Frame_Format_t *p_frame_format);

/**
 * \brief	This function is used to set the new frame format.
 */
uint16_t ds_ep_store_frame_format(const Frame_Format_t *new_frame_format);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DS_RADAR_INTERFACE_H_ */

/* --- End of File -------------------------------------------------------- */
