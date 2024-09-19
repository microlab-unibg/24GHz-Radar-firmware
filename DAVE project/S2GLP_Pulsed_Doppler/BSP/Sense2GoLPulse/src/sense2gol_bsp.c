/**
    @file: sense2gol_bsp.c

    @brief: This file provides a set of firmware functions to manage BGT24LTR11
            on Sense2GoL Pulse board from Infineon.
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
#include "datastore.h"
#include "radar_api.h"
#include "radar_control.h"
#include "config.h"

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

/**
 * @brief Used to store the DMA source addresses.
 * This symbols needs to be global, to work together with the DAVE generated functions, where they are referenced.
 */
uint32_t* pDMA_src_G0 = NULL;
uint32_t* pDMA_src_G1 = NULL;
uint32_t* pDMA_src_G2 = NULL;
uint32_t* pDMA_src_G3 = NULL;

/**
 * @brief Used to store the DMA destination addresses.
 * This symbols needs to be global, to work together with the DAVE generated functions, where they are referenced.
 */
uint32_t* pDMA_dst_G0 = NULL;
uint32_t* pDMA_dst_G1 = NULL;
uint32_t* pDMA_dst_G2 = NULL;
uint32_t* pDMA_dst_G3 = NULL;

static uint16_t data_buffer[4][BSP_MAX_SAMPLE_SKIP_COUNT+BSP_MAX_NUM_SAMPLES_PER_CHIRP];

/**
 * @brief The frame count with current frame interval.
 */
static volatile uint32_t bsp_frame_counter = 0;

/**
 * @brief Global structure to store all data acquisition functions callback \ref Bsp_callback_Struct_t.
 */
static Bsp_callback_Struct_t bsp_cb =
{
  .bsp_acq_started_cb = NULL,
  .bsp_acq_done_cb = NULL
};

/**
 * @brief  The current watchdog status.
 */
static uint8_t  watchdog_status = 0;

/**
 * @brief  The ptat switching status, if 0 PATAT signal is not switch off after the frame, if 1 PTAT signal is switched
 */

static uint32_t ptat_switching_status = 0;

/**
 * @brief  The ptat switching delay, the time where the acquisition is delayed, after switching PTAT enable signal on.
 */

static uint32_t ptat_switch_delay_usec = 0;

uint32_t use_vptag_sh_duty_cycle = PTAT_S_H_SWITCHING;
uint32_t vptag_sh_release_delay_usec = PTAT_S_H_PULSE_WIDTH_USEC;
int32_t bb_pre_trigger_advance_usec = BB_PRE_TRIGGER_ADVANCE_USEC;

/* helper variables */
static uint32_t l_acq_started = 0;

bool pwm_bgt_enabled = true; // if set to false, PWM (= duty-cycle) of TX_ON and BGT_VCC_ENABLE is disabled, if true, standard duty-cycle is used!

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  This function is used to get the raw ADC data for Doppler or FMCW.
 *
 * This is called by \ref trigger_dma_start() or by automatic trigger set by \ref radar_set_automatic_frame_trigger().
 * First Doppler data is acquired and then afterwards FMCW data is acquired.
 * It checks if ADC timer is stopped and the time interval for ADC timer for Doppler data acquisition is set successfully.
 */
static void trigger_dma_start(void);

/**
 * \brief  This function enables the DMA to get further DMA data transactions.
 *         Need to call after reloading the DMA configurations.
 */
static void dma_enable(void);

/**
 * \brief  This function disables the DMA to avoid further DMA data transactions.
 *	   Need to call before reloading the DMA configurations or when data acquisition is finished.
 */
static void dma_disable(void);

/**
 * \brief  This function resets the DMA channel DLR to kill the last over sampled sample.
 */
static void dma_reset_dlr(void);

/**
 * \brief  This function assigns memory sections to the source pointers defined in the DMA app.
 */
static void dma_set_source_addr(void);

static void helper_switch_pwm_off(PWM_CCU8_t* handle_ptr);
static void helper_switch_pwm_on(PWM_CCU8_t* handle_ptr);

static void switch_active_PWM_GPIO(PWM_CCU8_t* handle_ptr);
static void switch_passive_PWM_GPIO(PWM_CCU8_t* handle_ptr);

/**
 * \brief 	This function power up the BGT and TX amplifier to start transmission.
 * \Note	This function is used in duty cycling mode.
 *          Before data acquisition this should be called for settling up BGT.
 * \param[in]  None
 * \return None
 */
static void bgt24ltr1x_start_up(void);

/**
 * \brief 	This function power down the BGT and stop TX transmission.
 * \Note	This function is used in duty cycling mode. After data acquisition this should be disabled.
 * \param[in]  None
 * \return None
 */
static void bgt24ltr1x_shut_down(void);

static void enable_Baseband(bool high);

/*
==============================================================================
    6. EXPORTED FUNCTIONS
==============================================================================
 */

uint8_t bsp_init(device_settings_t *p_dev)
{
	/* Set all data acquisition functions callback to NULL */
	bsp_cb.bsp_acq_started_cb = NULL;
	bsp_cb.bsp_acq_done_cb = NULL;

	/* Disable DMA */
	dma_disable();

	return bsp_update(p_dev);
}

//============================================================================

uint8_t bsp_update(device_settings_t *p_dev)
{
	uint32_t sample_and_hold_periodic_time_usec;
	uint32_t duty_ch1_bgt_vcc;								  /* BGT VCC */
	uint32_t bgt_tx_on_duty, bgt_tx_on_shift, bgt_tx_on_freq; /* BGT TX ON */
	uint32_t hold_duty, hold_shift, sample_hold_freq;		  /* Sample and hold */
	uint32_t sample_duty, sample_adc_freq;					  /* ADC sampling */

	uint32_t doppler_sampling_freq_hz = p_dev->adc_sampling_freq_Hz;
	uint32_t pulse_width_usec = p_dev->pulse_width_usec;
	uint32_t sample_hold_on_time_nsec = p_dev->sample_hold_on_time_nsec;

	if (doppler_sampling_freq_hz > BSP_MAX_DOPPLER_SAMPLING_FREQ_HZ || doppler_sampling_freq_hz < BSP_MIN_DOPPLER_SAMPLING_FREQ_HZ)
		return 1;

	if (pulse_width_usec > BSP_MAX_PULSE_WIDTH_USEC || pulse_width_usec < BSP_MIN_PULSE_WIDTH_USEC)
		return 1;

	// !!! Maximum value must be (1000 * PULSE_WIDTH_USEC - SAMPLE_HOLD_ON_DELAY_TIME_NSEC) */
	if (sample_hold_on_time_nsec > (1000 * pulse_width_usec - SAMPLE_HOLD_ON_DELAY_TIME_NSEC))
		return 1;

	/* Sample and hold's period in microsecond */
	sample_and_hold_periodic_time_usec = 1000000 / doppler_sampling_freq_hz;

	uint32_t min_int = bsp_calc_min_frame_interval_usec(p_dev);

	if (!pwm_bgt_enabled) // if false!
	{
		/* turn off duty cycling of TX_ON and BGT_VCC, and assign "active" level */
		helper_switch_pwm_off(&PWM_CCU8_BGT_TX_ON);
		helper_switch_pwm_off(&PWM_CCU8_BGT_VCC);

		switch_active_PWM_GPIO(&PWM_CCU8_BGT_VCC);
		switch_active_PWM_GPIO(&PWM_CCU8_BGT_TX_ON);
	}
	else // if true
	{
		/* turn on duty cycling of TX_ON and BGT_VCC, and assign "active" level */
		switch_passive_PWM_GPIO(&PWM_CCU8_BGT_TX_ON);
		switch_passive_PWM_GPIO(&PWM_CCU8_BGT_VCC);

		helper_switch_pwm_on(&PWM_CCU8_BGT_TX_ON);
		helper_switch_pwm_on(&PWM_CCU8_BGT_VCC);
	}

	Radar_Data_Acq_Mode_t mode = radar_get_data_acq_mode();
	if (mode != RADAR_DATA_CONTINUOUS_MODE)
	{
		use_vptag_sh_duty_cycle = PTAT_S_H_SWITCHING;
		if (p_dev->frame_period_usec <= min_int + 2 * p_dev->ptat_delay_usec + BB_PRE_TRIGGER_ADVANCE_USEC - (p_dev->num_sample_skip_count * sample_and_hold_periodic_time_usec))
		{
			bb_pre_trigger_advance_usec = 0;
		}
		else
		{
			bb_pre_trigger_advance_usec = BB_PRE_TRIGGER_ADVANCE_USEC - (p_dev->num_sample_skip_count * sample_and_hold_periodic_time_usec) + p_dev->ptat_delay_usec;
		}
	}
	else
	{
		bb_pre_trigger_advance_usec = 0;
	}

	if (p_dev->frame_period_usec <= min_int + 2 * p_dev->ptat_delay_usec)
	{
		ptat_switching_status = 0;
	}

	if (ptat_switching_status != 0)
	{
		// if ptag is switched on before the acquisition
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_PTAT);
		use_vptag_sh_duty_cycle = 1;
	}
	else
	{
		/* Enabled BBx_EN */
		enable_Baseband(true);
	}

	/* stop PWM timers for setting new parameters */
	PWM_CCU8_Stop(&PWM_CCU8_BGT_VCC);
	PWM_CCU8_Stop(&PWM_CCU8_BGT_TX_ON);
	PWM_CCU8_Stop(&PWM_CCU8_SAMPLE_HOLD);
	PWM_CCU8_Stop(&PWM_CCU8_ADC_SAMPLE);
	bsp_timer_stop_clear(&TIMER_PTAT_S_H_DELAY);
	bsp_timer_stop_clear(&TIMER_BB_PRE_TRIGGER);

	/* Example of PWM configuration:
    Set freq = 2000Hz, channel 1 duty = 50%, channel 2 duty = 20%
    PWM_CCU8_SetFreqAndDutyCycleSymmetric(&PWM_CCU8_BGT_VCC, 2000, 50 * 100, 20 * 100);
  */

	/* Configure PWM master for BGT VCC and BGT TX On control signals */
	/* For example, for 5us On time and a period of 500us the duty_ch1_bgt_vcc will be ((5us / 500us) * 100) * 100 */
	duty_ch1_bgt_vcc = ((100 * 100 * pulse_width_usec) / sample_and_hold_periodic_time_usec);

	//TODO: Check the returning error!
	/* Configure PWM BGT control signals */
	PWM_CCU8_SetFreqAndDutyCycleSymmetric(&PWM_CCU8_BGT_VCC, doppler_sampling_freq_hz,
										  duty_ch1_bgt_vcc, duty_ch1_bgt_vcc);

	/* Configure PWM of BGT TX ON */
	/* Slightly shift the start of BGT TX On signal versus the BGT VCC signal */
	bgt_tx_on_freq = (1000 * 1000) / pulse_width_usec; /* In Hz */
	bgt_tx_on_duty = ((100 * 100 * pulse_width_usec) - 10 * BGT_TX_ON_DELAY_TIME_NSEC) / pulse_width_usec;
	bgt_tx_on_shift = (10 * BGT_TX_ON_START_DELAY_TIME_NSEC) / pulse_width_usec;
	PWM_CCU8_SetFreqAndDutyCycleAsymmetric(&PWM_CCU8_BGT_TX_ON, bgt_tx_on_freq,
										   bgt_tx_on_shift, bgt_tx_on_duty);

	/* Configure PWM of start and holding time */
	/* Slightly shift the start of BGT TX On signal versus the BGT VCC signal */
	sample_hold_freq = (1000 * 1000) / pulse_width_usec; /* In Hz */

	hold_duty = ((100 * 100 * pulse_width_usec) - 10 * (SAMPLE_HOLD_ON_DELAY_TIME_NSEC + sample_hold_on_time_nsec)) / pulse_width_usec;
	hold_shift = (10 * SAMPLE_HOLD_START_DELAY_TIME_NSEC) / pulse_width_usec;
	PWM_CCU8_SetFreqAndDutyCycleAsymmetric(&PWM_CCU8_SAMPLE_HOLD, sample_hold_freq,
										   hold_shift, hold_duty);

	/* Configure start of sampling: trigger ADC to acquire I and Q signals data */
	sample_duty = 500;
	sample_adc_freq = (1000 * 1000) / ((float)pulse_width_usec + 0.4); /* In Hz */
	PWM_CCU8_SetFreqAndDutyCycleSymmetric(&PWM_CCU8_ADC_SAMPLE, sample_adc_freq,
										  sample_duty, sample_duty);

	/* Set additional timers: */
	uint32_t delay_usec = sample_and_hold_periodic_time_usec - vptag_sh_release_delay_usec;

	/* Ensure to set something reasonable! */
	if (vptag_sh_release_delay_usec > sample_and_hold_periodic_time_usec)
	{
		delay_usec = PTAT_S_H_PULSE_WIDTH_USEC;
	}

	TIMER_SetTimeInterval(&TIMER_PTAT_S_H_DELAY, delay_usec * 100U);

	if (bb_pre_trigger_advance_usec > 0)
	{
		if (TIMER_SetTimeInterval(&TIMER_BB_PRE_TRIGGER, bb_pre_trigger_advance_usec * 100U) != TIMER_STATUS_SUCCESS)
		{
			while (1);
		}
	}
	else if (bb_pre_trigger_advance_usec < 0)
	{

		uint32_t bb_delay_usec = abs(bb_pre_trigger_advance_usec) + p_dev->ptat_delay_usec;

		if (TIMER_SetTimeInterval(&TIMER_BB_PRE_TRIGGER, bb_delay_usec * 100U) != TIMER_STATUS_SUCCESS)
		{
			while (1);
		}
	}

	if (ptat_switch_delay_usec > 0)
	{
		if (TIMER_SetTimeInterval(&TIMER_PTAT_DELAY, ptat_switch_delay_usec * 100U) != TIMER_STATUS_SUCCESS)
		{
			while (1);
		}
	}

	/* TX ON: Get ready to wait master's trigger signal */
	PWM_CCU8_Start(&PWM_CCU8_BGT_TX_ON);

	/* Sample and hold: Get ready to wait master's trigger signal */
	PWM_CCU8_Start(&PWM_CCU8_SAMPLE_HOLD);

	/* ADC sampling: Get ready to wait master's trigger signal */
	PWM_CCU8_Start(&PWM_CCU8_ADC_SAMPLE);

	/* Reset the current frame counter */
	bsp_reset_frame_counter();

	return (0);
}

//============================================================================

void triggerFrameISR(void)
{
	Radar_Data_Acq_Mode_t mode = radar_get_data_acq_mode();

	if (mode == RADAR_DATA_CONTINUOUS_MODE)
	{
		/* Enabled BBx_EN */
		enable_Baseband(true);

		/* enable PTAT */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);

		/* ensure V_PTAT_SH_EN is enabled */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_V_PTAT_SH_EN);

		bsp_trigger_new_frame();
	}
	else if (bb_pre_trigger_advance_usec > 0)
	{
		enable_Baseband(true);

		/* Enabled BBx_EN */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB1_EN);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB2_EN);

		bsp_timer_start(&TIMER_BB_PRE_TRIGGER);
	}
	else if (ptat_switching_status != 0 && ptat_switch_delay_usec > 0)
	{
		/* bsp_trigger_new_frame() is called in timer SW IRQ! */
		bsp_timer_start(&TIMER_PTAT_DELAY);

		/* enable PTAT module */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);

		/* ensure V_PTAT_SH_EN is enabled */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_V_PTAT_SH_EN);

		/* if bb_pre_trigger advance is negative or smaller then ptat_switch delay, then it is actually a delay */
		if (bb_pre_trigger_advance_usec < 0)
		{
			bsp_timer_start(&TIMER_BB_PRE_TRIGGER);
		}
		else
		{
			enable_Baseband(true);
		}
	}
	else
	{
		if (bb_pre_trigger_advance_usec < 0)
		{
			bsp_timer_start(&TIMER_BB_PRE_TRIGGER);
		}
		else
		{
			enable_Baseband(true);
		}

		/* ensure PTAT signal is set high */
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_PTAT);

		/* ensure V_PTAT_SH_EN is enabled */
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_V_PTAT_SH_EN);

		bsp_timer_start(&TIMER_PTAT_S_H_DELAY);

		bsp_trigger_new_frame();
	}
}

//============================================================================

void trigger_ptat_delay(void)
{
	bsp_timer_stop_clear(&TIMER_PTAT_DELAY);
	// ptat delay expired, start the actual acquisition
	bsp_trigger_new_frame();
}

//============================================================================

void INTERRUPT_BGT_VCC_ON_handler(void)
{
	if (l_acq_started)
	{
		if (use_vptag_sh_duty_cycle)
		{
			/* ensure V_PTAT_SH_EN is set to HOLD */
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_V_PTAT_SH_EN);
			/* Disable ptat straight after S&H device set to hold */
			DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_PTAT);

			bsp_timer_start(&TIMER_PTAT_S_H_DELAY);
		}
		else
		{
			/* Enable ptat supply */
			DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);
			/* ensure V_PTAT_SH_EN is enabled */
			DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_V_PTAT_SH_EN);
		}
	}
}

//============================================================================

void INTERRUPT_PTAT_S_H_DELAY_handler(void)
{
	if (l_acq_started)
	{
		/* Enable ptat supply */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);

		/* ensure V_PTAT_SH_EN is set to sample */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_V_PTAT_SH_EN);
	}
	bsp_timer_stop_clear(&TIMER_PTAT_S_H_DELAY);
}

//============================================================================

void INTERRUPT_BB_PRE_TRIGGER_handler(void)
{
	bsp_timer_stop_clear(&TIMER_BB_PRE_TRIGGER);

	if (ptat_switch_delay_usec > 0 && (bb_pre_trigger_advance_usec > (int32_t)ptat_switch_delay_usec))
	{
		bsp_timer_start(&TIMER_PTAT_DELAY);

		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BGT_VCC_PTAT);

		/* ensure V_PTAT_SH_EN is enabled */
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_V_PTAT_SH_EN);
	}
	else
	{
		/* Enabled BBx_EN */
		enable_Baseband(true);
	}
}

//============================================================================

void bsp_trigger_new_frame(void)
{
	/* Get the raw ADC data */
	trigger_dma_start();

	/* This callback is used to inform upper layers about the beginning of data acquisition process */
	if (bsp_cb.bsp_acq_started_cb != NULL)
	{
		bsp_cb.bsp_acq_started_cb(NULL);
	}
}

//============================================================================

void copyDataBuffer2AcqBuffer(acq_buf_obj *p_acq_buf, uint32_t frame_counter)
{
	  uint16_t *p_dst_Ch0 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 0, 0);
	  uint16_t *p_dst_Ch1 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 1, 0);
	  uint16_t *p_dst_Ch2 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 2, 0);
	  uint16_t *p_dst_Ch3 = (uint16_t*) get_buffer_address_by_chirp(p_acq_buf, 3, 0);

	  uint32_t skip_count = p_acq_buf->internals.skip_count;
	  uint32_t byte_count = p_acq_buf->params.num_of_samples_per_chirp*sizeof(uint16_t);

	  memcpy(p_dst_Ch0, &data_buffer[0][skip_count], byte_count);
	  memcpy(p_dst_Ch1, &data_buffer[1][skip_count], byte_count);
	  memcpy(p_dst_Ch2, &data_buffer[2][skip_count], byte_count);
	  memcpy(p_dst_Ch3, &data_buffer[3][skip_count], byte_count);

	  p_acq_buf->frame_counter = frame_counter;
}

//============================================================================

void DMA_Event_Handler(XMC_DMA_CH_EVENT_t dma_event)
{
	if ((dma_event & XMC_DMA_CH_EVENT_BLOCK_TRANSFER_COMPLETE) != 0)
	{
		/* In case of continuous mode */
		if (radar_get_data_acq_mode() == RADAR_DATA_CONTINUOUS_MODE)
		{
			dma_disable();

			/* Clear the request from source peripheral even though DMA was disabled by itself.
	         * We should consider the pending request from external peripheral. */
			dma_reset_dlr();

			acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

			/* copy data buffer into target buffer, if skip count is active*/
			copyDataBuffer2AcqBuffer(p_acq_buf, bsp_frame_counter);

			/* DMA Reconfiguration */
			bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);

			/* Enable DMA */
			dma_enable();

			bsp_frame_counter++;
		}
		else /* In normal mode */
		{

			l_acq_started = 0;

			bsp_timer_stop_clear(&TIMER_PTAT_S_H_DELAY);

			if (ptat_switching_status != 0)
			{
				/* ensure V_PTAT_SH_EN is disabled */
				DIGITAL_IO_SetOutputLow(&DIGITAL_IO_V_PTAT_SH_EN);

				/* Disable ptat straight after acquisition */
				DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BGT_VCC_PTAT);
			}

			if (bb_pre_trigger_advance_usec != 0)
			{
				/* Disable BB op-amps (Shutdown) */
				enable_Baseband(false);
			}

			/* Stop PWM for BGT */
			bsp_bgt_shut_down();

			dma_disable();

			/* Clear the request from source peripheral even though DMA was disabled by itself.
	         * We should consider the pending request from external peripheral. */
			dma_reset_dlr();

			acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

			/* copy data buffer into target buffer, if skip count is active*/
			copyDataBuffer2AcqBuffer(p_acq_buf, bsp_frame_counter);

			bsp_frame_counter++;
		}

		g_adc_sampling_completed = true;

		/* One frame is ready */
		/* This callback is used to inform upper layers that a valid data is available and ADC sampling is completed */
		if ((bsp_cb.bsp_acq_done_cb != NULL) & (watchdog_status == 0))
		{
			bsp_cb.bsp_acq_done_cb(NULL);
		}
	}
}

//============================================================================

void bsp_bgt_start_up(void)
{
	/* Start PWM to pulse BGT Enable Pin */
	bgt24ltr1x_start_up();
}

//============================================================================

void bsp_bgt_shut_down(void)
{
	/* Stop PWM pulsing BGT Enable Pin */
	bgt24ltr1x_shut_down();
}

//============================================================================

TIMER_STATUS_t bsp_timer_stop_clear(TIMER_t *const handle_timer)
{
	TIMER_STATUS_t status;

	status = TIMER_Stop(handle_timer);

	status |= TIMER_Clear(handle_timer);

	return (status);
}

//============================================================================

TIMER_STATUS_t bsp_timer_start(TIMER_t *const handle_timer)
{
	TIMER_STATUS_t status;

	status = TIMER_Start(handle_timer);

	return (status);
}

//============================================================================

void bsp_dma_set_adc_control_from_acq_buf_obj(acq_buf_obj *acq_buf)
{
	dma_set_source_addr();

	/* We use a 2nd buffer here, to be able to run the signal processing in parallel to the data acquisition */
	pDMA_dst_G0 = (uint32_t *)&data_buffer[0][0];
	pDMA_dst_G1 = (uint32_t *)&data_buffer[1][0];
	pDMA_dst_G2 = (uint32_t *)&data_buffer[2][0];
	pDMA_dst_G3 = (uint32_t *)&data_buffer[3][0];

	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num, (uint32_t)pDMA_dst_G0);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num, (uint32_t)pDMA_dst_G1);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num, (uint32_t)pDMA_dst_G2);
	XMC_DMA_CH_SetDestinationAddress(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num, (uint32_t)pDMA_dst_G3);

	uint32_t block_size = acq_buf->internals.skip_count + acq_buf->params.num_of_samples_per_chirp;
	DMA_CH_SetBlockSize(&DMA_CH_G0, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G1, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G2, block_size);
	DMA_CH_SetBlockSize(&DMA_CH_G3, block_size);
}

//============================================================================

uint8_t bsp_get_number_of_available_radar_devices(void)
{
	/* The number of available radar devices is defined in the BSP layer */
	return (BSP_NUM_OF_AVAILABLE_RADAR_DEVICES);
}

//============================================================================

float32_t bsp_bgt_get_temp_value(void)
{
	/* It is not supported in BGT24LTR11 */
	return 0;
}

//============================================================================

uint32_t bsp_get_frame_counter(void)
{
	return (bsp_frame_counter);
}

//============================================================================

void bsp_reset_frame_counter(void)
{
	bsp_frame_counter = 0;
}

//============================================================================

float bsp_get_vptat_voltage(void)
{
	uint32_t vptat_value;

	/* Read the voltage on V_PTAT pin */
	ADC_MEASUREMENT_ADV_SoftwareTrigger(&ADC_MEASUREMENT_SCAN);
	vptat_value = (ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_SCAN_BGT24_PTAT_handle));

	return (vptat_value * 3.3f / 4095.0f);
}

//============================================================================

uint8_t bsp_register_callback(Bsp_Callback_ID_t cb_id, void (*pcallback)(void *parameter))
{
	uint8_t status = 0;

	/* Check if he callback function is valid */
	if (pcallback == NULL)
	{
		return (1);
	}

	/* Register user callback function based on callback ID */
	switch (cb_id)
	{
	case BSP_ACQUISITION_STARTED_CB_ID:
		bsp_cb.bsp_acq_started_cb = pcallback;
		break;

	case BSP_ACQUISITION_DONE_CB_ID:
		bsp_cb.bsp_acq_done_cb = pcallback;
		break;

	default:
		/* Return error status */
		status = 1;
		break;
	}

	return (status);
}

//============================================================================

uint8_t bsp_unregister_callback(Bsp_Callback_ID_t cb_id)
{
	uint8_t status = 0;

	/* Unregister user callback function based on callback ID */
	switch (cb_id)
	{
	case BSP_ACQUISITION_STARTED_CB_ID:
		bsp_cb.bsp_acq_started_cb = NULL;
		break;

	case BSP_ACQUISITION_DONE_CB_ID:
		bsp_cb.bsp_acq_done_cb = NULL;
		break;

	default:
		/* Return error status */
		status = 1;
		break;
	}

	return (status);
}

//============================================================================

uint32_t bsp_start_frame_intervall_timer(uint32_t frame_interval_usec)
{

	/* Update time period of Automatic Trigger */
	if (TIMER_SetTimeInterval(&TIMER_FRAME_TRIG, frame_interval_usec * 100U) != TIMER_STATUS_SUCCESS)
	{
		return 0;
	}
	return 1;
}

//============================================================================

uint32_t bsp_calc_min_frame_interval_usec(device_settings_t *p_dev)
{
	uint32_t min_frame_interval_usec = p_dev->num_of_chirps_per_frame * (((p_dev->num_samples_per_chirp + p_dev->num_sample_skip_count) * 1000U * 1000U) / p_dev->adc_sampling_freq_Hz); // units in us

	return min_frame_interval_usec;
}

//============================================================================

void bsp_set_ptat_switch_status(uint32_t new_status, uint32_t new_delay_usec)
{
	ptat_switching_status  = new_status;
	ptat_switch_delay_usec = new_delay_usec;
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

static void trigger_dma_start(void)
{
	watchdog_status = 0;

	/* DMA Reconfiguration */
	dma_disable();

	acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();

	bsp_dma_set_adc_control_from_acq_buf_obj(p_acq_buf);

	bsp_bgt_start_up();

	/* Enable DMA */
	dma_enable();

	l_acq_started = 1;
}

//============================================================================

static void dma_enable(void)
{
	DMA_CH_Enable(&DMA_CH_G0);
	DMA_CH_Enable(&DMA_CH_G1);
	DMA_CH_Enable(&DMA_CH_G2);
	DMA_CH_Enable(&DMA_CH_G3);
}

//============================================================================

static void dma_disable(void)
{
	DMA_CH_Disable(&DMA_CH_G0);
	DMA_CH_Disable(&DMA_CH_G1);
	DMA_CH_Disable(&DMA_CH_G2);
	DMA_CH_Disable(&DMA_CH_G3);
}

//============================================================================

static void dma_reset_dlr(void)
{
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num);
	XMC_DMA_CH_ClearSourcePeripheralRequest(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num);
}

//============================================================================

static void dma_set_source_addr(void)
{
	/* DMA Source/Destination Address setup */
	pDMA_src_G0 = (uint32_t *)&ADC_MEASUREMENT_ADV_G0_CH_G0_RES;
	pDMA_src_G1 = (uint32_t *)&ADC_MEASUREMENT_ADV_G1_CH_G1_RES;
	pDMA_src_G2 = (uint32_t *)&ADC_MEASUREMENT_ADV_G2_CH_G2_RES;
	pDMA_src_G3 = (uint32_t *)&ADC_MEASUREMENT_ADV_G3_CH_G3_RES;

	/* Set the source address pointers for each DMA transfer */
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G0.dma_global->dma, DMA_CH_G0.ch_num, (uint32_t)pDMA_src_G0);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G1.dma_global->dma, DMA_CH_G1.ch_num, (uint32_t)pDMA_src_G1);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G2.dma_global->dma, DMA_CH_G2.ch_num, (uint32_t)pDMA_src_G2);
	XMC_DMA_CH_SetSourceAddress(DMA_CH_G3.dma_global->dma, DMA_CH_G3.ch_num, (uint32_t)pDMA_src_G3);
}

//============================================================================

static void bgt24ltr1x_start_up(void)
{
	/* Start BGT */
	PWM_CCU8_Start(&PWM_CCU8_BGT_VCC);
}

//============================================================================

static void bgt24ltr1x_shut_down(void)
{
	/* Stop BGT */
	PWM_CCU8_Stop(&PWM_CCU8_BGT_VCC);
}

//============================================================================

static void helper_switch_pwm_off(PWM_CCU8_t *handle_ptr)
{
	XMC_GPIO_MODE_t new_mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;

	if ((bool)true == handle_ptr->config_ptr->gpio_ch1_out0_enable)
	{
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch1_out0_ptr, handle_ptr->config_ptr->gpio_ch1_out0_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch1_out1_enable)
	{
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch1_out1_ptr, handle_ptr->config_ptr->gpio_ch1_out1_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch2_out2_enable)
	{
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch2_out2_ptr, handle_ptr->config_ptr->gpio_ch2_out2_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch2_out3_enable)
	{
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch2_out3_ptr, handle_ptr->config_ptr->gpio_ch2_out3_pin, new_mode);
	}
}

//============================================================================

static void helper_switch_pwm_on(PWM_CCU8_t *handle_ptr)
{
	if ((bool)true == handle_ptr->config_ptr->gpio_ch1_out0_enable)
	{
		XMC_GPIO_MODE_t new_mode = handle_ptr->config_ptr->gpio_ch1_out0_config_ptr->mode;
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch1_out0_ptr, handle_ptr->config_ptr->gpio_ch1_out0_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch1_out1_enable)
	{
		XMC_GPIO_MODE_t new_mode = handle_ptr->config_ptr->gpio_ch1_out1_config_ptr->mode;
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch1_out1_ptr, handle_ptr->config_ptr->gpio_ch1_out1_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch2_out2_enable)
	{
		XMC_GPIO_MODE_t new_mode = handle_ptr->config_ptr->gpio_ch2_out2_config_ptr->mode;
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch2_out2_ptr, handle_ptr->config_ptr->gpio_ch2_out2_pin, new_mode);
	}

	if ((bool)true == handle_ptr->config_ptr->gpio_ch2_out3_enable)
	{
		XMC_GPIO_MODE_t new_mode = handle_ptr->config_ptr->gpio_ch2_out3_config_ptr->mode;
		XMC_GPIO_SetMode(handle_ptr->config_ptr->gpio_ch2_out3_ptr, handle_ptr->config_ptr->gpio_ch2_out3_pin, new_mode);
	}
}

//============================================================================

static void switch_active_PWM_GPIO(PWM_CCU8_t *handle_ptr)
{
	XMC_GPIO_OUTPUT_LEVEL_t output_level;

	output_level = (handle_ptr->config_ptr->ccu8_cc8_slice_timer_ptr->passive_level_out0 == (uint32_t)XMC_CCU8_SLICE_OUTPUT_PASSIVE_LEVEL_LOW)
					   ? XMC_GPIO_OUTPUT_LEVEL_HIGH
					   : XMC_GPIO_OUTPUT_LEVEL_LOW;

	XMC_GPIO_SetOutputLevel(handle_ptr->config_ptr->gpio_ch1_out0_ptr, handle_ptr->config_ptr->gpio_ch1_out0_pin, output_level);
}

//============================================================================

static void switch_passive_PWM_GPIO(PWM_CCU8_t *handle_ptr)
{
	XMC_GPIO_OUTPUT_LEVEL_t output_level;

	output_level = (handle_ptr->config_ptr->ccu8_cc8_slice_timer_ptr->passive_level_out0 == (uint32_t)XMC_CCU8_SLICE_OUTPUT_PASSIVE_LEVEL_LOW)
					   ? XMC_GPIO_OUTPUT_LEVEL_LOW
					   : XMC_GPIO_OUTPUT_LEVEL_HIGH;
	XMC_GPIO_SetOutputLevel(handle_ptr->config_ptr->gpio_ch1_out0_ptr, handle_ptr->config_ptr->gpio_ch1_out0_pin, output_level);
}

//============================================================================

static void enable_Baseband(bool high)
{
	if (high)
	{
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB1_EN);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BB2_EN);
	}
	else
	{
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BB1_EN);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BB2_EN);
	}
}

/* --- End of File -------------------------------------------------------- */
