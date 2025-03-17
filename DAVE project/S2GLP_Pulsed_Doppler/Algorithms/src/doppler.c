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
** algoritmo modificato per analizzare battito e respiro.
** ===========================================================================
*/

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "sense2gol_bsp_limits.h"
#include "doppler.h"

#include "i2c_master.h"
#include "dsp_lib.h"
#include "timer.h"
#include <DAVE.h>

#define DOWNSAMPLING_FACTOR 10
#define BACKGROUND_FRAMES 100  // Numero di frame per acquisire il background
#define WAIT_TIME 500000000        // 5 secondi di attesa


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

static float   breathing_spectrum[2*FFT_SIZE];
static float*  breathing_fft_signal = doppler_spectrum;

static float   if_scale = (float)DOPPLER_IQ_SCALE * 3.3f / 4095.0f;

/* FFT window */
static float   fft_fast_window_buffer[FFT_SIZE];
static uint32_t fft_fast_window_flag = 0;
static FFT_Window_Struct_t fft_fast_time_window =
{
	//
  .fft_window_type = FFT_WINDOW_CHEBYSHEV,       /* Type of window defined by \ref FFT_Window_Type_t */
  .fft_size = FFT_SIZE_256,                      /* FFT size \ref FFT_Size_t  */
  .fft_window_length = 0,                        /* Number of elements in the FFT window  */
  //TODO: To be checked with CW the value of AT
  .fft_chebyshev_at_dB = 60,                     /* Attenuation parameter needed in case of Chebyshev window */
  .fft_time_twiddle_table = NULL,                /* Pointer to twiddle factor table for FFT windowing over time */
  .fft_window_buffer = fft_fast_window_buffer    /* Pointer to floating point memory containing data values for FFT windowing over time */
};

//==============================================================================
/* Struttura per la finestra FFT */
/*static FFT_Window_Struct_t fft_window;
static float fft_window_buffer[FFT_SIZE] = {0};

/* Funzione per inizializzare la finestra FFT */
/*void init_fft_window() {
    fft_window.fft_window_type = FFT_WINDOW_HANN;  // Usa la finestra di Hanning
    fft_window.fft_window_length = FFT_SIZE;
    fft_window.fft_chebyshev_at_dB = 60;  // Usato solo per la finestra Chebyshev
    fft_window.fft_window_buffer = fft_window_buffer;
    fft_window.fft_size = FFT_SIZE_128;

    fft_window_gen(&fft_window); // Genera la finestra
}*/

// Stati del sistema
/*typedef enum {
    WAIT_BACKGROUND,
    ACQUIRE_BACKGROUND,
    WAIT_PERSON,
    MEASURE_BREATH
} SystemState_t;

static SystemState_t state = WAIT_PERSON;
static float background_fft[FFT_SIZE / 2] = {0};
static int frame_count = 0;
static uint32_t start_time = 0;

TIMER_t TIMER_0;*/

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

/*void delay_ms(uint32_t ms) {
    uint32_t start = TIMER_GetTime(&TIMER_0);
    while (TIMER_GetTime(&TIMER_0) - start < ms * 1000);
}

void breathing_do(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings,
                const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result)
{
    static float raw_data_i1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];
    static float raw_data_q1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];
    uint16_t *p_temp[BSP_NUM_OF_ADC_CHANNELS];
    uint16_t Ns = p_acq_buf->params.num_of_samples_per_chirp;

    switch (state) {
        case WAIT_BACKGROUND:
            USBD_VCOM_SendString("\r\nPosizionarsi LONTANO dal radar.\r\n");
            USBD_VCOM_SendString("Inizio acquisizione background tra 5 secondi...\r\n");
            start_time = TIMER_GetTime(&TIMER_0);
            while(TIMER_GetTime(&TIMER_0) - start_time < WAIT_TIME){
            bsp_led_red_on();
            bsp_led_blue_on();
            bsp_led_green_off();
            }
            state = ACQUIRE_BACKGROUND;
            break;

        case ACQUIRE_BACKGROUND:

        	bsp_led_red_off();
        	bsp_led_blue_on();
        	bsp_led_green_off();

            p_temp[0] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 2, 0);
            p_temp[1] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 3, 0);

            for (uint32_t i = 0; i < FFT_SIZE; i++) {
                raw_data_i1[i] = (float)((p_temp[0])[i * DOWNSAMPLING_FACTOR]) * if_scale;
                raw_data_q1[i] = (float)((p_temp[1])[i * DOWNSAMPLING_FACTOR]) * if_scale;
            }

            float complex_fft_signal[FFT_SIZE * 2] = {0};
            float fft_result[FFT_SIZE / 2] = {0};

            compute_fft_signal(fft_fast_time_window, raw_data_i1, raw_data_q1, FFT_SIZE, FFT_SIZE, 1.0f,
                               FFT_INPUT_COMPLEX, NULL, NULL, complex_fft_signal);
            compute_fft_spectrum(complex_fft_signal, FFT_SIZE, fft_result);

            for (int i = 0; i < FFT_SIZE / 2; i++) {
                background_fft[i] += fft_result[i] / BACKGROUND_FRAMES;
            }

            frame_count++;
            if (frame_count >= BACKGROUND_FRAMES) {
                USBD_VCOM_SendString("\r\nBackground acquisito.\r\n");
                USBD_VCOM_SendString("Posizionarsi DAVANTI al radar.\r\n");
                USBD_VCOM_SendString("Misurazione tra 5 secondi...\r\n");
                start_time = TIMER_GetTime(&TIMER_0);
                while(TIMER_GetTime(&TIMER_0) - start_time < WAIT_TIME){
                	bsp_led_red_off();
                	bsp_led_blue_on();
                	bsp_led_green_on();
                }
            state = WAIT_PERSON;
            }
            break;

        case WAIT_PERSON:

        	bsp_led_red_off();
        	bsp_led_blue_off();
        	bsp_led_green_on();

            USBD_VCOM_SendString("Inizio misurazione respiro...\r\n");
            state = MEASURE_BREATH;
            break;

        case MEASURE_BREATH:
            p_temp[0] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 2, 0);
            p_temp[1] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 3, 0);

            for (uint32_t i = 0; i < FFT_SIZE; i++) {
                raw_data_i1[i] = (float)((p_temp[0])[i * DOWNSAMPLING_FACTOR]) * if_scale;
                raw_data_q1[i] = (float)((p_temp[1])[i * DOWNSAMPLING_FACTOR]) * if_scale;
            }

            float complex_fft_signal2[FFT_SIZE * 2] = {0};
            float fft_result2[FFT_SIZE / 2] = {0};
            float clean_fft[FFT_SIZE / 2] = {0};

            compute_fft_signal(fft_fast_time_window, raw_data_i1, raw_data_q1, FFT_SIZE, FFT_SIZE, 1.0f,
                               FFT_INPUT_COMPLEX, NULL, NULL, complex_fft_signal2);
            compute_fft_spectrum(complex_fft_signal2, FFT_SIZE, fft_result2);

            for (int i = 0; i < FFT_SIZE / 2; i++) {
                clean_fft[i] = fft_result2[i];//- background_fft[i];
            }

            float max_amplitude = 0;
            float breathing_frequency = 0;
            for (int i = 1; i < FFT_SIZE / 2; i++) {
                float freq = (float)i / FFT_SIZE;
                if (freq > 0.1f && freq < 1.0f) {
                    if (clean_fft[i] > max_amplitude) {
                        max_amplitude = clean_fft[i];
                        breathing_frequency = freq;
                    }
                }
            }

            float BRPM = breathing_frequency * 60;

            if (BRPM < 12) {
                bsp_led_blue_on();
                bsp_led_red_on();
                bsp_led_green_off();
            } else if (BRPM > 16) {
                bsp_led_red_on();
                bsp_led_blue_off();
                bsp_led_green_off();
            } else {
                bsp_led_green_on();
                bsp_led_red_off();
                bsp_led_blue_on();
             }
            break;
    }
}*/
void breathing_do(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result){

    static float raw_data_i1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];
    static float raw_data_q1[BSP_MAX_NUM_SAMPLES_PER_CHIRP];

    uint16_t *p_temp[BSP_NUM_OF_ADC_CHANNELS];
    uint16_t Ns = p_acq_buf->params.num_of_samples_per_chirp;

    /* Genera finestra FFT */
    if (fft_fast_window_flag != Ns) {
        fft_fast_time_window.fft_window_length = Ns;
        if (fft_window_gen(&fft_fast_time_window) == 0) {
            XMC_DEBUG("Errore generazione finestra FFT!\n");
            while (1);
        }
        fft_fast_window_flag = Ns;
    }

    /* Pulizia buffer */
    memset(raw_data_i1, 0, sizeof(raw_data_i1));
    memset(raw_data_q1, 0, sizeof(raw_data_q1));

    /* Acquisizione dati */
    p_temp[0] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 0, 0);
    p_temp[1] = (uint16_t *)get_buffer_address_by_chirp(p_acq_buf, 1, 0);

    for (uint32_t i = 0; i < Ns; i++) {
        raw_data_i1[i] = (float)((p_temp[0])[i]) * if_scale;
        raw_data_q1[i] = (float)((p_temp[1])[i]) * if_scale;
    }

    /* Calcolo frequenza respiratoria */
    breathing_calc_frequency(fft_fast_time_window, raw_data_i1, raw_data_q1, Ns, cp_algo_settings, cp_dev_settings, p_algo_result);

    /* Salva frame corrente */
    p_algo_result->frame_counter = p_acq_buf->frame_counter;
}

//============================================================================

void breathing_calc_frequency(FFT_Window_Struct_t fft_window, float* if1_i, float* if1_q, uint16_t number_samples, const algo_settings_t *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result){

    uint32_t maxBin = 0;
    float maxVal = 0;
    float freq_per_bin;
    float if1_real, if1_imag;

    float breathing_frequency = 0.0f;
    float breathing_level = 0.0f;

    /* Calcolo FFT */
    compute_fft_signal(fft_window, if1_i, if1_q, number_samples, FFT_SIZE, 1.0, FFT_INPUT_COMPLEX,
                       &if1_real, &if1_imag, breathing_fft_signal);

    compute_fft_spectrum(breathing_fft_signal, FFT_SIZE, breathing_spectrum);

    /* Rimozione DC */
    breathing_spectrum[0] = 0;
    breathing_spectrum[FFT_SIZE/2] = 0;

    /* Setto range respirazione tra 0.1 Hz e 1.0 Hz */
    uint32_t fft_min_check = (uint32_t) ceilf(0.1f * FFT_SIZE / cp_dev_settings->adc_sampling_freq_Hz);
    uint32_t fft_max_check = (uint32_t) ceilf(1.0f * FFT_SIZE / cp_dev_settings->adc_sampling_freq_Hz);

    uint32_t size_check = fft_max_check - fft_min_check + 1;

    /* Trovo frequenza dominante */
    arm_max_f32(&breathing_spectrum[fft_min_check], size_check, &maxVal, &maxBin);
    maxBin += fft_min_check;

    freq_per_bin = cp_dev_settings->adc_sampling_freq_Hz / (float)FFT_SIZE;
    breathing_level = maxVal;
    breathing_frequency = maxBin * freq_per_bin;

    /* Converto in respiri per minuto */
    float breathing_rate = breathing_frequency * 60.0f;

    /* Risultati con i LED */
    if (breathing_rate < 10.0f) {
        bsp_led_red_off();
        bsp_led_blue_on();   // Respirazione bassa
        bsp_led_green_off();
    }
    else if (breathing_rate >= 10.0f && breathing_rate <= 20.0f) {
        bsp_led_red_off();
        bsp_led_blue_off();
        bsp_led_green_on();  // Respirazione normale
    }
    else {
        bsp_led_red_on();    // Respirazione elevata
        bsp_led_blue_off();
        bsp_led_green_off();
    }

    /* Salva i risultati */
    p_algo_result->doppler_frequency_hz = breathing_frequency;
    p_algo_result->velocity_kmph = breathing_rate; // Uso il campo della velocità per mostrare il respiro
}


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
