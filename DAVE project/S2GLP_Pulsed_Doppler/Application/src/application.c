/**
    @file: application.c

    @brief: This file includes the functions for the Sense2GoL Pulse
            RangeDoppler Application Layer.
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

#include "sense2gol_usb.h"
#include "EndpointRadarDeviceStatus.h"
#include "radar_device_status.h"
#include "application.h"
#include "datastore.h"
#include "acq_buffer.h"
#include "radar_pulse.h"
#include "radar_mcubase.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

/* Define the number of supported endpoints */
#define NUMBER_SUPPORTED_ENDPOINTS         (3U)

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

/* Firmware version declaration */
Firmware_Information_t firmware_information =
{
	"Sense2GoL Pulse BGT24LTR11 Demo Radar Board",
	FW_VERSION_MAJOR,
	FW_VERSION_MINOR,
	FW_VERSION_REVISION
};

/* Radar handler declaration */
Radar_Handle_t h_radar_device = NULL;

/* A global buffer to hold all endpoints used by Host Communication library */
Endpoint_t endpoint_list[NUMBER_SUPPORTED_ENDPOINTS];

/* Global variable to store the state if USB initialization, used with radar GUI */
volatile uint8_t g_usb_init = false;

/* Radar application demo global state */
static Radar_App_State_t radar_app_state = APP_IDLE;

static uint32_t acq_completed = 0;
static uint32_t algo_completed = 0;
static uint32_t acq_started = 0;
static uint32_t app_initalized = 0;

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

void acq_completed_cb(void);

void algo_completed_cb(void);

/* This callback function will be executed by radar control when starting data acquisition process
 *   it can be used to get information about the beginning of data acquisition
 */
void app_data_acq_started_cb(void *parameter);

/* This callback function will be executed by radar control at the end of data acquisition,
 * it is used to inform upper layers that data sampling is done and data is ready in user buffer
 */
void app_data_acq_done_cb(void *parameter);

/* This function will be executed at the end of data acquisition,
 * This function pointer should be set to the assigned algorithm before starting data processing
 */
static  algorithm  algo_processing_function = NULL;

#if defined(FMCW_SUPPORTED) || defined(DOPPLER_SUPPORTED)
  static void do_algo_helper(void);
#endif

/*
==============================================================================
   6. EXPORTED FUNCTIONS
==============================================================================
 */

/* Initialize all firmware and algorithm parameters and start communication protocol */
void app_init(void)
{
  /* Initialize USB */
  if (bsp_usb_init(BSP_USB_TIMEOUT) == 0)
  {
    g_usb_init = true;
  }

  /* Register all endpoints to the communication protocol */
  Endpoint_t ep_list[NUMBER_SUPPORTED_ENDPOINTS] =
  {
    EP_RADAR_MCUBASE_LIST_ENTRY(h_radar_device),
	EP_RADAR_S2GLPULSE_LIST_ENTRY(h_radar_device),
  	EP_RADAR_DEVICE_STATUS_LIST_ENTRY(h_radar_device)
  };

  /* Register all endpoints in case of init error to the communication protocol */
  Endpoint_t ep_err_list[2] =
  {
    EP_RADAR_MCUBASE_LIST_ENTRY(h_radar_device),
  	EP_RADAR_DEVICE_STATUS_LIST_ENTRY(h_radar_device)
  };

  app_initalized = 0;
  uint8_t num_of_ep = sizeof(ep_err_list)/sizeof(ep_err_list[0]);
  Endpoint_t *pEndPoints = &ep_err_list[0];

  /* Open the available radar device */
  if (radar_open_device(0, &h_radar_device) == RADAR_ERR_OK)
  {
	  app_initalized = 1;
	  /* Register user callback functions */
	  radar_register_callback(RADAR_ACQUISITION_STARTED_CB_ID, app_data_acq_started_cb);
	  radar_register_callback(RADAR_ACQUISITION_DONE_CB_ID, app_data_acq_done_cb);

	  num_of_ep = sizeof(ep_list)/sizeof(ep_list[0]);
	  pEndPoints = &ep_list[0];
  }
  memcpy(endpoint_list, pEndPoints, num_of_ep * sizeof(Endpoint_t));

  /* Initialize the internal protocol state and the communication interface */
  protocol_init(endpoint_list, num_of_ep, NULL, BSP_HOST_TIMEOUT_USEC, system_reset);

  /* Initialize the internal protocol state and the communication interface */
  if (app_initalized)
  {
	  /* Start data acquisition at the defined frame interval */
	  if (radar_start_acquisition(h_radar_device) != RADAR_ERR_OK)
	  {
		XMC_DEBUG("Starting Radar data acquisition failed\n");
		while (1U);
	  }
  }
}

//============================================================================

/* Register algorithm processing function */
void app_register_algo_process(algorithm algo_processor)
{
  /* Register algo processing function */
  algo_processing_function = algo_processor;
}

//============================================================================

float calcCurrentConsumption_mA(device_settings_t *pDev)
{
	float retval = 0.0F;
	extern bool pwm_bgt_enabled;

	if (!pwm_bgt_enabled)
		return 0.0F; /* invalid! */

	if (ds_get_board_variant() == 1)
	{
		float bgt_on_current_mA   = BSP_BGT_ON_CURRENT_mA;
		float ptat_on_current_mA  = BSP_PTAT_ON_CURRENT_mA;
		float bbf_on_current_mA   = BSP_BBF_ON_CURRENT_mA;

		float f_sample            = (float)pDev->adc_sampling_freq_Hz;
		float frame_period_sec    = (float)pDev->frame_period_usec / 1000000.0F;
		float pulse_width_sec     = (float)pDev->pulse_width_usec / 1000000.0F;
		float skipped_samples     = (float)pDev->num_sample_skip_count;
		float samples             = (float)pDev->num_samples_per_chirp;
		float ptat_delay_sec      = (float)pDev->ptat_delay_usec / 1000000.0F;

		float total_samples       = (skipped_samples + samples);

		float frame_on_time_sec   = (total_samples - 1) / f_sample + pulse_width_sec;
		float bgt_on_time_sec     = total_samples * pulse_width_sec;
		float ptat_on_time_sec    = frame_on_time_sec + ptat_delay_sec;
		float bbf_on_time_sec     = frame_period_sec; // baseband filter is not switched, but always on!

		float bgt_duty_cycle      = bgt_on_time_sec / frame_period_sec;
		float ptat_duty_cycle     = ptat_on_time_sec / frame_period_sec;
		float bbf_duty_cycle      = bbf_on_time_sec / frame_period_sec;

		retval = (bgt_duty_cycle * bgt_on_current_mA) + (ptat_duty_cycle * ptat_on_current_mA) + (bbf_duty_cycle * bbf_on_current_mA);
	}
	else
	{
		float bgt_on_current_mA   = BSP_BGT_ON_CURRENT_mA;
		float ptat_on_current_mA  = BSP_PTAT_ON_CURRENT_mA;
		float bbf_on_current_mA   = BSP_BBF_ON_CURRENT_mA;

		float f_sample            = (float)pDev->adc_sampling_freq_Hz;
		float frame_period_sec    = (float)pDev->frame_period_usec / 1000000.0F;
		float pulse_width_sec     = (float)pDev->pulse_width_usec / 1000000.0F;
		float skipped_samples     = (float)pDev->num_sample_skip_count;
		float samples             = (float)pDev->num_samples_per_chirp;
		float ptat_delay_sec      = (float)pDev->ptat_delay_usec / 1000000.0F;
		float ptat_pulse_time_sec = (float)PTAT_S_H_PULSE_WIDTH_USEC / 1000000.0F;
		float bbf_advance_sec     = (float)BB_PRE_TRIGGER_ADVANCE_USEC / 1000000.0F;

		float total_samples       = (skipped_samples + samples);

		if (fabs(samples / f_sample - frame_period_sec) < 0.001F) // equidistant mode
			ptat_delay_sec = 0.0F;

		float bgt_on_time_sec     = total_samples * pulse_width_sec;
		float ptat_on_time_sec    = total_samples * ptat_pulse_time_sec + ptat_delay_sec;

		float DMA_TRANSFER_SEC    = 2.0F / 1000.0F;
		float bbf_on_time_sec     = bbf_advance_sec + (samples - 1) / f_sample + DMA_TRANSFER_SEC; // baseband filter is now switched
		if (bbf_on_time_sec > frame_period_sec)
			bbf_on_time_sec = frame_period_sec;

		float bgt_duty_cycle  = bgt_on_time_sec / frame_period_sec;
		float ptat_duty_cycle = ptat_on_time_sec / frame_period_sec;
		float bbf_duty_cycle  = bbf_on_time_sec / frame_period_sec;

		retval = (bgt_duty_cycle * bgt_on_current_mA) + (ptat_duty_cycle * ptat_on_current_mA) + (bbf_duty_cycle * bbf_on_current_mA);
	}

	return retval;
}

//============================================================================

/* Main application process */
void app_process(void)
{
  if (g_usb_init == true)
  {
    /* Run communication protocol */
    protocol_run();
  }
  else
  {
    /* Initialize the USB low layer driver and start USB data communication */
    if (bsp_usb_init(BSP_USB_TIMEOUT) == 0)
    {
      g_usb_init = true;
    }
  }
  /* Main radar application process */
  radar_app_process();
}

//============================================================================

static uint32_t helper_do_hw_settings(void)
{
	device_settings_t *p_dev_settings = ds_device_get_settings();

	uint32_t error = radar_apply_hw_settings(h_radar_device, p_dev_settings);
	if (error == RADAR_ERR_OK)
	{
		ds_device_settings_copy_to_shadow();
		/* add com-lib callback to send-out updated parameters!!! */
	}
	else
	{
		ds_device_settings_revert_from_shadow();
		// current and new parameter differs, if an error occurred, tell to the application
	}
	radar_app_state = APP_IDLE;
	ds_clear_device_parameter_changed();

	error = radar_set_automatic_frame_trigger(h_radar_device, p_dev_settings->frame_period_usec);
	return error;
}

//============================================================================

void IRQ_ERROR_TOGGLE_Handler(void)
{
	bsp_led_red_toggle();
	bsp_timer_start(&TIMER_ERROR_TOGGLE);
}

//============================================================================

static uint32_t app_connected = 0;
static uint32_t delayCount = 0;

void app_reset_protocol(void)
{
	delayCount = 0;
	app_connected = 0;
}

//============================================================================

/* Main radar application process */
void radar_app_process(void)
{
	if (acq_started)
	{
		/* ... */
		acq_started = 0;
	}

	if (acq_completed)
	{

#if defined(FMCW_SUPPORTED) || defined(DOPPLER_SUPPORTED)
		/* Start the processing of the algorithm assigned in the function pointer 'algo_processor' */
		if (algo_processing_function != NULL)
		{
			do_algo_helper();
			algo_completed += 1;
		}
#endif
		acq_completed = 0;
	}

	if (algo_completed)
	{
		algo_completed_cb();
		algo_completed = 0;
	}

	if (app_initalized)
	{
		switch (radar_app_state)
		{
		case APP_IDLE:
			radar_app_state = APP_CHECK_FOR_SETTINGS_UPDATE;
			break;

		case APP_CHECK_FOR_SETTINGS_UPDATE:
			/* Check if a new hardware settings are required */
			if (ds_is_device_parameter_changed() || ds_is_device_settings_updated())
			{
				radar_app_state = APP_APPLY_DEVICE_SETTINGS;
			}
			else
			{
				radar_app_state = APP_CHECK_FOR_ERROR;
			}
			break;

		case APP_APPLY_DEVICE_SETTINGS:
			helper_do_hw_settings();
			radar_app_state = APP_CHECK_FOR_ERROR;
			break;

		case APP_CHECK_FOR_ERROR:
		{
			int status = device_status_check();
			if (status < 0)
			{
				radar_app_state = APP_HANDLE_ERROR;
			}
			else
			{
				radar_app_state = APP_IDLE;
			}
		}
		break;

		case APP_HANDLE_ERROR:
			send_device_status_response();
			radar_app_state = APP_IDLE;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (radar_app_state)
		{
		case APP_IDLE:
		case APP_CHECK_FOR_ERROR:
		{
			int status = device_status_check();
			if (status < 0)
			{
				radar_app_state = APP_HANDLE_ERROR;
			}
			else
			{
				radar_app_state = APP_IDLE;
			}
		}
		break;

		case APP_HANDLE_ERROR:
		{
			/* Send out error only once, if PC app is not requesting it! */
			if (!app_connected)
			{

				delayCount += 1;
				if (delayCount == 100)
				{
					send_device_status_response();
					app_connected = 1;
					delayCount = 0;
				}
				bsp_time_delay(20, BSP_TIMER_DELAY_MILLISECOND);
			}
			radar_app_state = APP_CHECK_FOR_ERROR;
		}
		break;

		default:
			break;
		}
	}
}

//============================================================================

/* De-initialize and free all resources used by the application */
void app_destructor(void)
{
	/* Unregister user callback functions */
	radar_unregister_callback(RADAR_ACQUISITION_STARTED_CB_ID);
	radar_unregister_callback(RADAR_ACQUISITION_DONE_CB_ID);
}

/*
==============================================================================
   6. USER CALLBACK FUNCTIONS
==============================================================================
 */

/* Here an example of user callback implementation */

/* This callback function will be executed by radar control when starting data acquisition process
 *   it can be used to get information about the beginning of data acquisition
 */
void app_data_acq_started_cb(void *parameter)
{
	acq_started += 1;
}

/* This callback function will be executed by radar control at the end of data acquisition,
 * it is used to inform upper layers that data sampling is done and data is ready in user buffer
 */
void app_data_acq_done_cb(void *parameter)
{
	acq_completed += 1;
	acq_completed_cb();
}

//============================================================================

void update_leds(algo_result_t *pResult)
{
	if (pResult->target_approaching)
	{
		/*bsp_led_red_off();
		bsp_led_blue_off();
		bsp_led_green_on();*/

		bsp_led_red_on();
		bsp_led_blue_off();
		bsp_led_green_off();

		/* Arduino status IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
	else if (pResult->target_departing)
	{
		/*bsp_led_red_on();
		bsp_led_blue_off();
		bsp_led_green_off();*/

		bsp_led_red_off();
		bsp_led_blue_off();
		bsp_led_green_on();

		/* Arduino IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D7);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
	}
	else if (pResult->motion_detected)
	{
		bsp_led_red_off();
		bsp_led_blue_on();
		bsp_led_green_off();

		/* Arduino IO */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
	else
	{
		bsp_led_red_off();
		bsp_led_blue_off();
		bsp_led_green_off();

		/* Arduino IO */
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D4);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D6);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_ARD_D7);
	}
}

//============================================================================

/* This function yet to be filled in case of software reset */
void system_reset(void)
{

}

//============================================================================

/* Run the selected algorithm */
#if defined(FMCW_SUPPORTED) || defined(DOPPLER_SUPPORTED)

static algo_result_t algo_result;

void do_algo_helper(void)
{
	acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();
	algo_settings_t *p_algo_settings = ds_algo_get_settings();
	device_settings_t *p_dev_settings = ds_device_get_shadow_settings();
	algo_result_t *p_algo_result = &algo_result;

	/* this is the async raw data streaming to host device */
	radar_pulse_raw_data(p_acq_buf, p_dev_settings);

	algo_processing_function(p_acq_buf, p_algo_settings, p_dev_settings, p_algo_result);

	ds_store_algo_result_ptr(p_algo_result);

	radar_pulse_results_updates(p_algo_result);

	mcu_base_updates();

	update_leds(p_algo_result);
}
#endif

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
