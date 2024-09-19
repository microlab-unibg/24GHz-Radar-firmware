/**
    @file: doppler.c

    @brief: This file contains basic doppler algorithm using Complex FFT for
            calculating target speed.
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

#include "sense2gol_bsp_limits.h"
#include "doppler.h"

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

/* Allocate in static memory, the pointer is used in results passed to application */
/* 2 time FFT_SIZE elements is needed, because we first calculate the complex spectrum */
static float   doppler_spectrum[2*FFT_SIZE];
static float*  doppler_fft_signal = doppler_spectrum;

static float   if_scale = (float)DOPPLER_IQ_SCALE * 3.3f / 4095.0f;

/* FFT window */
static float   fft_fast_window_buffer[FFT_SIZE];
static uint32_t fft_fast_window_flag = 0;
static FFT_Window_Struct_t fft_fast_time_window =
{
  .fft_window_type = FFT_WINDOW_CHEBYSHEV,       /* Type of window defined by \ref FFT_Window_Type_t */
  .fft_size = FFT_SIZE_256,                      /* FFT size \ref FFT_Size_t  */
  .fft_window_length = 0,                        /* Number of elements in the FFT window  */
  //TODO: To be checked with CW the value of AT
  .fft_chebyshev_at_dB = 60,                     /* Attenuation parameter needed in case of Chebyshev window */
  .fft_time_twiddle_table = NULL,                /* Pointer to twiddle factor table for FFT windowing over time */
  .fft_window_buffer = fft_fast_window_buffer    /* Pointer to floating point memory containing data values for FFT windowing over time */
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

void doppler_do(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings,
				const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result)
{
	/* allocated as static memory, not on call stack */
	static float raw_data_i1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];
	static float raw_data_q1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];

	uint16_t *p_temp[BSP_NUM_OF_ADC_CHANNELS];
	uint16_t Ns = p_acq_buf->params.num_of_samples_per_chirp;

	/* Generate FFT window, it will be used for FFT calculation */
	if (fft_fast_window_flag != Ns)
	{
		fft_fast_time_window.fft_window_length = Ns; /* Number of elements in the FFT window  */
		if (fft_window_gen(&fft_fast_time_window) == 0)
		{
			XMC_DEBUG("Invalid FFT window parameters !\n");
			while (1);
		}
		fft_fast_window_flag = Ns;
	}

	/* clear array values that are not acquired */
	memset(raw_data_i1, 0, sizeof(raw_data_i1));
	memset(raw_data_q1, 0, sizeof(raw_data_q1));

	if (cp_algo_settings->use_high_gain_doppler)
	{
		/* Only one chirp for Doppler */
		p_temp[0] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 0, 0);
		p_temp[1] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 1, 0);
	}
	else
	{
		/* Only one chirp for Doppler */
		p_temp[0] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 2, 0);
		p_temp[1] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 3, 0);
	}

	for (uint32_t i = 0; i < Ns; i++)
	{
		raw_data_i1[i] = (float)((p_temp[0])[i]) * if_scale;
		raw_data_q1[i] = (float)((p_temp[1])[i]) * if_scale;
	}

	doppler_calc_speed(fft_fast_time_window, raw_data_i1, raw_data_q1, Ns, cp_algo_settings, cp_dev_settings, p_algo_result);

	/* indicated, that the results are taken from dedicated frame */
	p_algo_result->frame_counter = p_acq_buf->frame_counter;
}

//============================================================================

void doppler_calc_speed(FFT_Window_Struct_t fft_window, float* if1_i, float* if1_q, uint16_t number_samples,
		                const algo_settings_t  *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result)
{
	uint32_t maxBin = 0;
	float maxVal = 0;
	float doppler_Hz_per_bin;
	float if1_real, if1_imag;

	uint8_t motion_detected    = 0U;
	uint8_t motion_departing   = 0U;
	uint8_t motion_approaching = 0U;
	float   doppler_frequency  = 0.0f;
	float   doppler_velocity   = 0.0f;
	float   doppler_level = 0.0f;

	compute_fft_signal(fft_window, if1_i, if1_q, number_samples, FFT_SIZE, 1.0,
			           FFT_INPUT_COMPLEX, &if1_real, &if1_imag, doppler_fft_signal);

	compute_fft_spectrum(doppler_fft_signal, FFT_SIZE, doppler_spectrum);

	doppler_spectrum[0] = 0; // remove DC from spectrum
	doppler_spectrum[FFT_SIZE/2] = 0; // remove DC from middle bin

	uint32_t fft_min_check = (uint32_t) ceilf(cp_algo_settings->min_speed_kmph*44.4f*FFT_SIZE/cp_dev_settings->adc_sampling_freq_Hz);

	uint32_t size_check = FFT_SIZE - 2*fft_min_check + 1;

	/* Calculates maxValue and returns corresponding BIN value */
	arm_max_f32(&doppler_spectrum[fft_min_check], size_check, &maxVal, &maxBin);
	maxBin += fft_min_check;

	doppler_Hz_per_bin =  cp_dev_settings->adc_sampling_freq_Hz / (float)FFT_SIZE;

	doppler_level = maxVal;

	if (maxVal > (float) cp_algo_settings->speed_detection_threshold) // if the threshold is exceeded, we have a doppler event
	{
		if (maxBin < FFT_SIZE / 2)
	    {
			motion_departing = 1;
			motion_approaching = 0;
	    }
	    else
	    {
	    	motion_departing = 0;
	    	motion_approaching = 1;

	    	maxBin = FFT_SIZE - maxBin;
	    }

		doppler_frequency =  maxBin * doppler_Hz_per_bin;

		doppler_velocity  =  doppler_frequency / 44.4f;

		if (doppler_velocity < cp_algo_settings->min_speed_kmph || doppler_velocity > cp_algo_settings->max_speed_kmph)
		{
			doppler_frequency = 0.0;
			doppler_velocity = 0.0;
			motion_departing = 0;
			motion_approaching = 0;
		}

		if (motion_departing == 1 && doppler_velocity > 0)
		{
			doppler_velocity  =  -doppler_velocity; // negate  speed for departing targets
		}
	}
	else
	{
		motion_departing = 0;
		motion_approaching = 0;

		doppler_frequency = 0.0;
		doppler_velocity = 0.0;
	}

	if (maxVal > (float) cp_algo_settings->motion_detection_threshold) // if the threshold is exceeded, we have a doppler event
	{
		motion_detected = 1;
	}

	if (p_algo_result)
	{
		p_algo_result->motion_detected = motion_detected;
		/* Store processing results into result structure */

		if (p_algo_result->p_doppler_spectrum)
		{
			if (maxBin > FFT_SIZE / 2) {
				/* prepare spectrum to be copied to results */
				for (uint32_t idx = 0; idx < FFT_SIZE/2; idx++)
				{
					doppler_spectrum[idx] = doppler_spectrum[FFT_SIZE - 1 - idx];
				}
			}

			/* transfer spectrum to result structure, if there is memory allocated for it */
			memcpy(p_algo_result->p_doppler_spectrum, doppler_spectrum, p_algo_result->doppler_spectrum_elems*sizeof(float));
		}

		p_algo_result->level 					= doppler_level;
		p_algo_result->doppler_frequency_hz		= doppler_frequency;

		p_algo_result->velocity_kmph 			= doppler_velocity;
		p_algo_result->target_departing 		= motion_departing;
		p_algo_result->target_approaching   	= motion_approaching;
	}
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File ------------------------------------------------ */
