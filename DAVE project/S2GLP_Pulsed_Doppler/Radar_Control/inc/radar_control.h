/**
   @file: radar_control.h
   @brief: Common API definition valid for all radar operations related to MCU, BGT, PLL
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

#ifndef RADAR_CONTROL_H_
#define RADAR_CONTROL_H_

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

#include "acq_buffer.h"
#include "datastore.h"
  
/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/**
 * \brief Defines radar function callback ID.
 * @{
 */
typedef enum
{
  RADAR_ACQUISITION_STARTED_CB_ID   = 1U,    /**< Start of data acquisition callback ID */
  RADAR_ACQUISITION_DONE_CB_ID      = 2U     /**< End of data acquisition callback ID */
} Radar_Callback_ID_t;
/** @} */

/**
 * \brief Defines radar data acquisition modes. Manual, Automatic or Test mode.
 * @{
 */
typedef enum
{
  RADAR_MANUAL_TRIGGER_MODE    = 0U,   /**< Manual triggering data acquisition */
  RADAR_AUTO_TRIGGER_MODE      = 1U,   /**< Automatic triggering data acquisition */
  RADAR_DATA_CONTINUOUS_MODE   = 2U,   /**< Get data in continuous acquisition mode */
  RADAR_TEST_MODE              = 3U    /**< Test mode, not used yet */
} Radar_Data_Acq_Mode_t;
/** @} */

typedef  void(*algorithm)(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result);

typedef struct device_settings_TAG device_settings_t ;

typedef void* Radar_Handle_t;

/** @} */

/*
==============================================================================
   4. DATA
==============================================================================
 */

extern volatile uint32_t g_adc_sampling_completed;	/**< Global variable to inform about the sampling done status, used by algorithm to start processing */

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief	This function registers user callback functions in BSP layer.
 *          It delegates user callback to the BSP low layer driver
 *
 * \param[in] 	cb_id		Radar callback ID \ref Radar_Callback_ID_t
 * \param[in] 	pcallback	pointer to user callback function
 *
 * \return One of the following status can be return:
 *   	   - \ref RADAR_STATUS_SUCCESS    Success
 *   	   - \ref RADAR_STATUS_FAIL       Fail
 *   	   - \ref RADAR_STATUS_NA         Feature not available
 */
uint16_t radar_register_callback(Radar_Callback_ID_t cb_id, void (*pcallback)(void *parameter));

/**
 * \brief	This function unregisters user callback functions from BSP low layer driver.
 *
 * \param[in] 	cb_id		Radar callback ID \ref Radar_Callback_ID_t
 *
 * \return One of the following status can be return:
 *   	   - \ref RADAR_STATUS_SUCCESS    Success
 *   	   - \ref RADAR_STATUS_FAIL       Fail
 *   	   - \ref RADAR_STATUS_NA         Feature not available
 */
uint16_t radar_unregister_callback(Radar_Callback_ID_t cb_id);

uint32_t radar_apply_hw_settings(Radar_Handle_t device, device_settings_t *p_hw_settings);

Radar_Data_Acq_Mode_t radar_get_data_acq_mode(void);

void radar_set_data_acq_mode(Radar_Data_Acq_Mode_t new_mode);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* RADAR_CONTROL_H_ */

/* --- End of File -------------------------------------------------------- */
