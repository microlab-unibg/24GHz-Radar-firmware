/**
    @file: radar_base.c

    @brief: This file contains the basic driver API for Infineon radar sensors.
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
#include "version.h"
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

// declare it global, to be known by bsp, for now!

size_t frame_size = 0;

/* Global Data Buffer used to store the raw IQ data samples for FMCW chirps,
   first (SAMPLES_PER_CHIRP * 2) samples are for I&Q data for RX1,
   followed by (SAMPLES_PER_CHIRP * 2) samples for I&Q data for RX2 */
uint32_t data_fifo[BSP_MAX_ADC_BUFFER_SIZE_BYTES/sizeof(uint32_t)];  
                                   					 	 	 	 	 	

static const Driver_Version_t driver_version_s =
{
  /*.uMajor    =*/ FW_VERSION_MAJOR,
  /*.uMinor    =*/ FW_VERSION_MINOR,
  /*.uRevision =*/ FW_VERSION_REVISION
};

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

const Driver_Version_t* radar_get_driver_version(void)
{
  return (&driver_version_s);
}

//============================================================================

uint8_t radar_get_number_of_available_devices(void)
{
  return bsp_get_number_of_available_radar_devices();
}

//============================================================================

uint16_t radar_open_device(int32_t device_number, Radar_Handle_t* device)
{
  XMC_UNUSED_ARG(device_number); /* Suppress compiler warnings of unused variable */
  
  Frame_Format_t  frame_format;
  uint16_t retval = RADAR_ERR_OK;
  int32_t status = 0;
  acq_buf_obj acq_buf = { .status = not_initialized } ;

  /* Initialize the driver instance with NULL, until it was successfully initialized */
  if (device)
	  (*device = NULL);

  /* Initialize driver data structure */
  /* -------------------------------- */
  status = ds_init();
  if (status > 0)
  {
    return (RADAR_ERR_INIT_ERROR);
  }

  device_settings_t *pDev = ds_device_get_settings();
  
  //========================================================================
  //------------------------- Frame Format configurations ------------------
  //========================================================================
  ds_ep_fetch_frame_format(&frame_format);
  


  //=========================================================================
  //------------------------- General configurations ------------------------
  //=========================================================================
  radar_set_data_acq_mode(RADAR_MANUAL_TRIGGER_MODE);
  
  /* If the frame format contains a 0, this makes no sense. */
  if ((frame_format.rx_mask == 0) ||
      (frame_format.num_chirps_per_frame  == 0) ||
        (frame_format.num_samples_per_chirp == 0) ||
          (frame_format.num_chirps_per_frame  > BSP_MAX_NUM_CHIRPS_PER_FRAME) ||
            (frame_format.num_samples_per_chirp > BSP_MAX_NUM_SAMPLES_PER_CHIRP) ||
			(frame_format.num_samples_per_chirp < BSP_MIN_NUM_SAMPLES_PER_CHIRP)
			)
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* Check frame memory limits */
  /* ------------------------- */
  frame_size = frame_format.num_chirps_per_frame * frame_format.num_samples_per_chirp *
    ds_ep_get_device_info()->num_rx_antennas * 2U * sizeof(uint16_t); // complex IQ samples per antenna
  
  if (frame_size > BSP_MAX_ADC_BUFFER_SIZE_BYTES)
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* Initialize sensor hardware */
  /* -------------------------- */
  acq_buf = create_acq_data_buffer_obj((uint8_t*) data_fifo, BSP_MAX_ADC_BUFFER_SIZE_BYTES,
                                       frame_format.num_chirps_per_frame,
                                       frame_format.num_samples_per_chirp, BSP_MAX_NUM_ADC_CHANNELS, pDev->adc_resolution, pDev->num_sample_skip_count);
  
  ds_set_active_acq_buf(acq_buf);
 
  
  /* BGT, DMA, Timers initialization */
  /* -------------------------------------------------- */
  retval = radar_init(device);
  
  g_adc_sampling_completed = false;

  return (retval);
}

//============================================================================

void radar_close_device(Radar_Handle_t device)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  bsp_bgt_shut_down();
}

//============================================================================

uint16_t radar_get_device_info(Radar_Handle_t device, Device_Info_t* device_info)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */

  /* Get device information:
      - Sensor description
      - Hardware Version information
      - Sensor features
      - Data format
  */
  *device_info = *ds_ep_get_device_info();
   
  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_get_temperature(Radar_Handle_t device, uint8_t temp_sensor, int32_t* temperature_001C)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Check temperature sensor, only single sensor supported */
  if (temp_sensor == 0)
  {
    *temperature_001C = (int32_t) (bsp_bgt_get_temp_value() * 1000.0f);	// (expected value is actual temperature in C° x 1000)
    
    return (RADAR_ERR_OK);
  }
  else
  {
    return (RADAR_ERR_SENSOR_DOES_NOT_EXIST);
  }
}

//============================================================================

uint16_t radar_get_tx_power(Radar_Handle_t device, uint8_t tx_antenna, int32_t* tx_power_001dBm)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Check number of TX antenna */
  if (tx_antenna == 0)
  {
    *tx_power_001dBm = 0; // Not supported by BGTLTR TX power
    
    return (RADAR_ERR_OK);
  }
  
  return (RADAR_ERR_ANTENNA_DOES_NOT_EXIST);
}

//============================================================================

uint16_t radar_get_chirp_duration(Radar_Handle_t device, uint32_t* chirp_duration_nsec)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Calculate chirp timing from current settings */
  /* -------------------------------------------- */
  /* Unit is ns. (factor 10^9 is distributed to denominator and denominator for fixed point precision) */
  
  //*chirp_duration_nsec = (ds_device_get_settings()->pll_chirp_time_usec * 1000U);
  *chirp_duration_nsec = 1;
  
  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_get_min_frame_interval(Radar_Handle_t device, uint32_t* min_frame_interval_usec)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */

  device_settings_t *p_dev = ds_device_get_settings();

  *min_frame_interval_usec = bsp_calc_min_frame_interval_usec(p_dev);

  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_set_frame_format(Radar_Handle_t device, const Frame_Format_t* frame_format)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Configuration change is only allowed while automatic trigger is not working */
  if (radar_get_data_acq_mode() != RADAR_MANUAL_TRIGGER_MODE)
  {
    return (RADAR_ERR_BUSY);
  }
  
  /* Check if configuration is valid */
  /* ------------------------------- */
  
  /* This sensor can only acquire real signal data */
  if (frame_format->signal_part != RADAR_SIGNAL_I_AND_Q)
  {
    return (RADAR_ERR_UNAVAILABLE_SIGNAL_PART);
  }
  
  /* This sensor support RX1, RX2 and both at the same time antennas enabled */
  if (frame_format->rx_mask > BSP_RX_ANTENNA_MASK)
  {
    return (RADAR_ERR_ANTENNA_DOES_NOT_EXIST);
  }

  /* Only a limited number of ramps are supported */
  if ((frame_format->num_chirps_per_frame == 0) || (frame_format->num_chirps_per_frame > BSP_MAX_NUM_CHIRPS_PER_FRAME))
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }

  /* BGT can store only a limited number of samples per chirp */
  if ((frame_format->num_samples_per_chirp == 0) || (frame_format->num_samples_per_chirp > BSP_MAX_NUM_SAMPLES_PER_CHIRP))
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* For Doppler modulation mode the number of chirp must be 1 */
  if ((frame_format->num_chirps_per_frame != 1) && (ds_device_get_settings()->pll_modulation_mode == MODULATION_DOPPLER))
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* Check if samples per chirp are in [32,64,128,256] range */
  if ((frame_format->num_samples_per_chirp != 256U) &&
      (frame_format->num_samples_per_chirp != 128U) &&
        (frame_format->num_samples_per_chirp != 64U)  &&
          (frame_format->num_samples_per_chirp != 32U) &&
		  (frame_format->num_samples_per_chirp != 16U))
  {
    return (RADAR_ERR_SAMPLERATE_OUT_OF_RANGE);
  }
  
  /* Check frame memory limits */
  /* ------------------------- */
  frame_size = frame_format->num_chirps_per_frame * frame_format->num_samples_per_chirp * 2U *
  ds_ep_get_device_info()->num_rx_antennas * sizeof(uint16_t); // complex IQ samples per antenna
  
  if (frame_size > BSP_MAX_ADC_BUFFER_SIZE_BYTES)
  {
    return (RADAR_ERR_UNSUPPORTED_FRAME_FORMAT);
  }
  
  /* Store new frame format in store */
  ds_ep_store_frame_format(frame_format);
  
  g_adc_sampling_completed = false;
  
  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_get_frame_format(Radar_Handle_t device, Frame_Format_t* p_frame_format)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */
  
  /* Return current frame format of driver object */
  ds_ep_fetch_frame_format(p_frame_format);
  
  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_get_frame(Radar_Handle_t device, Frame_Info_t *frame_info, uint8_t wait_for_data)
{
  XMC_UNUSED_ARG(device); /* Suppress compiler warnings of unused variable */

  Frame_Format_t frame_format;

  ds_ep_fetch_frame_format(&frame_format);

  acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

  /* without automaticFrameTrigger */
  if ((radar_get_data_acq_mode() != RADAR_AUTO_TRIGGER_MODE) &&
      (radar_get_data_acq_mode() != RADAR_DATA_CONTINUOUS_MODE))
  {
    bsp_trigger_new_frame();
  }
  else /* with automaticFrameTrigger */
  {
    if ((!wait_for_data) && (!g_adc_sampling_completed))
    {
      return (RADAR_ERR_TIMEOUT);
    }
  }

  while (g_adc_sampling_completed == false); // block GetFrame if data is not ready

  /* Data acquired */
  /* ------------- */
  //memcpy(transport_buffer, p_acq_buf->p_acq_buf, frame_size);	// copy ADC buffer to be send to avoid data corruption by Automatic trigger

  frame_info->sample_data = p_acq_buf->p_acq_buf;
  frame_info->num_rx_antennas = ds_ep_get_device_info()->num_rx_antennas;
  frame_info->num_chirps = frame_format.num_chirps_per_frame;
  frame_info->rx_mask = frame_format.rx_mask;
  frame_info->adc_resolution = ds_device_get_settings()->adc_resolution;
  frame_info->interleaved_rx = 0;
  frame_info->frame_number = bsp_get_frame_counter();
  frame_info->data_format = RADAR_RX_DATA_COMPLEX;
  frame_info->temperature_001C = (int32_t)(bsp_bgt_get_temp_value() * 1000); // (expected value is actual temperature in C° x 1000)
  frame_info->num_samples_per_chirp = frame_format.num_samples_per_chirp;

  g_adc_sampling_completed = false;

  return (RADAR_ERR_OK);
}

//============================================================================

uint16_t radar_set_automatic_frame_trigger(Radar_Handle_t device, uint32_t frame_interval_usec)
{
  /* Stop trigger timer first  */
  /* ------------------------- */
  bsp_timer_stop_clear(&TIMER_FRAME_TRIG);

  radar_set_data_acq_mode(RADAR_MANUAL_TRIGGER_MODE);

  /* Restart trigger timer */
  /* --------------------- */
  if (frame_interval_usec > 0)
  {
    uint32_t min_frame_interval_usec;

    radar_get_min_frame_interval(device, &min_frame_interval_usec);

    if (frame_interval_usec < min_frame_interval_usec)
    {
      return (RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL);
    }

    device_settings_t *pDev = ds_device_get_settings();

    pDev->frame_period_usec = frame_interval_usec;

    /* Reset the current frame counter */
    bsp_reset_frame_counter();

    g_adc_sampling_completed = false;

    /* Get data in continuous mode */
    if ((abs(frame_interval_usec - min_frame_interval_usec) < GUARD_FRAME_TIME_USEC))
    {
      /* Set continuous mode */
      radar_set_data_acq_mode(RADAR_DATA_CONTINUOUS_MODE);

      bsp_set_ptat_switch_status(0, 0);

      /* Start data acquisition */
      bsp_trigger_new_frame();
    }
    else
    {
      radar_set_data_acq_mode(RADAR_AUTO_TRIGGER_MODE);
      pDev->frame_period_usec = frame_interval_usec;

      uint32_t temp_ptat_switch = pDev->ptat_is_switched;
      uint32_t temp_ptat_delay_usec = pDev->ptat_delay_usec;

      if (frame_interval_usec < min_frame_interval_usec + 2 * pDev->ptat_delay_usec)
        temp_ptat_switch = 0;
      else
      {
        float sample_interval_usec = 1000000.0 / pDev->adc_sampling_freq_Hz;
        if (pDev->ptat_delay_usec > (uint32_t)sample_interval_usec)
          temp_ptat_delay_usec = pDev->ptat_delay_usec - (uint32_t)sample_interval_usec;
        else
          temp_ptat_delay_usec = 0;
      }

      /* Update time period of Automatic Trigger */
      if (!bsp_start_frame_intervall_timer(frame_interval_usec))
      {
        return (RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL);
      }

      bsp_set_ptat_switch_status(temp_ptat_switch, temp_ptat_delay_usec);

      bsp_timer_start(&TIMER_FRAME_TRIG);
    }
  }

  return (RADAR_ERR_OK);
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
