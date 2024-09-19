/**
    @file: radar_control.c

    @brief: This file contains common API definition valid for all radar 
            operations related to MCU, BGT, PLL.
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

/*
==============================================================================
    1. INCLUDE FILES
==============================================================================
 */

#include "sense2gol_bsp.h"
#include "sense2gol_bsp_limits.h"
#include "radar_api.h"
#include "radar_control.h"
#include "ds_radar_interface.h"

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

volatile  uint32_t    g_adc_sampling_completed = false;

Radar_Data_Acq_Mode_t  s_data_acq_mode = RADAR_MANUAL_TRIGGER_MODE;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/*
==============================================================================
    6. EXPORTED FUNCTIONS
==============================================================================
 */

uint16_t radar_init(Radar_Handle_t device)
{
  uint16_t status = RADAR_ERR_OK;

  device_settings_t *p_Dev = ds_device_get_settings();
  
  /* USB and DMA Setup */
  acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

  bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);
  
  status |= (uint16_t) bsp_init(p_Dev);
  
  status |= radar_set_sampling_freq(device, p_Dev->pll_modulation_mode, p_Dev->adc_sampling_freq_Hz);
  
  return (status);
}

//============================================================================

uint16_t radar_start_acquisition(Radar_Handle_t device)
{
  uint16_t status = RADAR_ERR_BUSY;
  
  if (ds_device_get_settings()->frame_period_usec > 0)
  {
    status = radar_set_automatic_frame_trigger(device, ds_device_get_settings()->frame_period_usec);
  }
  return (status);
}

//============================================================================

uint16_t radar_stop_acquisition(Radar_Handle_t device)
{
  uint16_t status = RADAR_ERR_BUSY;
  
  status = radar_set_automatic_frame_trigger(device , 0);
  
  return (status);
}

//============================================================================


void radar_get_frame_period(Radar_Handle_t device, uint32_t* time_msec)
{
  XMC_UNUSED_ARG(device);
  
  *time_msec = (ds_device_get_settings()->frame_period_usec / 1000);
}

//============================================================================

uint16_t radar_set_sampling_freq(Radar_Handle_t device, Modulation_Type_t modulation_type, const float freq_hz)
{
  XMC_UNUSED_ARG(device);
  XMC_UNUSED_ARG(modulation_type);
  XMC_UNUSED_ARG(freq_hz);

  //FIXME: Add settings freq here

  if (((uint32_t)freq_hz <= 0) || ((uint32_t)freq_hz > BSP_MAX_ADC_FREQ_HZ))
  {
    return RADAR_ERR_BUSY;
  }

  return (RADAR_ERR_OK);
}

//============================================================================

void radar_get_sampling_freq(Radar_Handle_t device, float* freq_Hz)
{
  XMC_UNUSED_ARG(device);
  
  *freq_Hz = (1000.0f * 1000.0f) * 100.0f / ds_device_get_settings()->adc_sampling_freq_Hz;
}

//============================================================================

uint16_t radar_register_callback(Radar_Callback_ID_t cb_id, void (*pcallback)(void *parameter))
{
  uint16_t status = RADAR_ERR_OK;

  /* Check if the callback function is valid */
  if (pcallback == NULL)
  {
    return (RADAR_ERR_BUSY);
  }

  /* Register user callback function based on callback ID */
  switch (cb_id)
  {
  case RADAR_ACQUISITION_STARTED_CB_ID:
    /* Callback executed by BSP layer at the beginning of data acquisition */
    bsp_register_callback(BSP_ACQUISITION_STARTED_CB_ID, pcallback);
    break;

  case RADAR_ACQUISITION_DONE_CB_ID:
    /* Callback executed by BSP layer at the end of data acquisition process */
    bsp_register_callback(BSP_ACQUISITION_DONE_CB_ID, pcallback);
    break;

  default:
    /* Return error status */
    status = RADAR_ERR_BUSY;
    break;
  }

  return (status);
}

//============================================================================

uint16_t radar_unregister_callback(Radar_Callback_ID_t cb_id)
{
  uint16_t status = RADAR_ERR_OK;

  /* Unregister user callback function based on callback ID */
  switch (cb_id)
  {
  case RADAR_ACQUISITION_STARTED_CB_ID:
    bsp_unregister_callback(BSP_ACQUISITION_STARTED_CB_ID);
    break;

  case RADAR_ACQUISITION_DONE_CB_ID:
    bsp_unregister_callback(BSP_ACQUISITION_DONE_CB_ID);
    break;

  default:
    /* Return error status */
    status = RADAR_ERR_BUSY;
    break;
  }
  
  return (status);
}

//============================================================================

static uint32_t radar_apply_new_hw_setting(Radar_Handle_t device, device_settings_t *p_new)
{
  uint32_t retval = RADAR_ERR_OK;

  radar_stop_acquisition(device);

  bsp_bgt_shut_down();

  if (bsp_update(p_new) != 0)
  {
	  // invalid settings values not changed!
	  retval = RADAR_ERR_PARAMETER_OUT_OF_RANGE;
  }

  /* recreate acq-buffer object! */
  /* the buffer is statically allocated to the max supported buffer size, so it is save to re-use it!
   * checks should have been applied earlier! */
  acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

  uint8_t *p_temp_buf = p_acq_buf->p_acq_buf;
  uint32_t reserved_size = p_acq_buf->internals.size_of_acq_buffer;


  acq_buf_obj acq_buf = create_acq_data_buffer_obj(p_temp_buf, reserved_size,
	  											   p_new->num_of_chirps_per_frame,
												   p_new->num_samples_per_chirp, BSP_MAX_NUM_ADC_CHANNELS, p_new->adc_resolution, p_new->num_sample_skip_count);

  ds_set_active_acq_buf(acq_buf);
  
  return retval; // confirm that everything was o.k.
}

//============================================================================

uint32_t radar_apply_hw_settings(Radar_Handle_t device, device_settings_t *p_hw_settings)
{
  uint32_t retval = RADAR_ERR_OK;

  retval = radar_apply_new_hw_setting(device, p_hw_settings);
  
  return retval;
}

//============================================================================

Radar_Data_Acq_Mode_t radar_get_data_acq_mode(void)
{
  return s_data_acq_mode;
}

//============================================================================

void radar_set_data_acq_mode(Radar_Data_Acq_Mode_t new_mode)
{
  s_data_acq_mode = new_mode;
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */

