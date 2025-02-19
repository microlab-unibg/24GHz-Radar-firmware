/**
    @file: main.c

    @brief: This application runs on demo Sense2GoL Pulse board with BGT24LTR11 and XMC4700 MCU.
            It consists on radar demonstration application.
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

#include "application.h"


#include "i2c_master.h"
#include "dsp_lib.h"
#include "timer.h"
#include <DAVE.h>

#define FFT_SIZE 128
#define DOWNSAMPLING_FACTOR 10
#define BACKGROUND_FRAMES 100  // Numero di frame per acquisire il background
#define WAIT_TIME 5000         // 5 secondi di attesa

// Stati del sistema
typedef enum {
    WAIT_BACKGROUND,
    ACQUIRE_BACKGROUND,
    WAIT_PERSON,
    MEASURE_BREATH
} SystemState_t;

static SystemState_t state = WAIT_BACKGROUND;
static float background_fft[FFT_SIZE / 2] = {0};
static int frame_count = 0;


/* Struttura per la finestra FFT */
static FFT_Window_Struct_t fft_window;
static float fft_window_buffer[FFT_SIZE] = {0};

/*
==============================================================================
   2. MAIN METHOD
==============================================================================
 */

/* Funzione per inizializzare la finestra FFT */
void init_fft_window() {
    fft_window.fft_window_type = FFT_WINDOW_HANN;  // Usa la finestra di Hanning
    fft_window.fft_window_length = FFT_SIZE;
    fft_window.fft_chebyshev_at_dB = 60;  // Usato solo per la finestra Chebyshev
    fft_window.fft_window_buffer = fft_window_buffer;
    fft_window.fft_size = FFT_SIZE_128;

    fft_window_gen(&fft_window); // Genera la finestra
}

TIMER_t TIMER_0;

int main(void)
{
  DAVE_STATUS_t status;
  TIMER_STATUS_t timer_status;
  
  /* Initialize DAVE APPs */
  status = DAVE_Init();
  
  if (status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");
    while (1U);
  }

  /*inizializzo tempo*/

  if (status == DAVE_STATUS_SUCCESS){
  	timer_status = TIMER_Start(&TIMER_0);
  }

  /* Register algorithm processing function:
     Set the algorithm processing function pointer, it will
     be used by the application for algorithm data processing */
    app_register_algo_process(doppler_do);

  /* Initialize the application */
  app_init();
  
  /* Infinite loop */
  while (1U)
  {
    /* Main application process */
    app_process();
  }
}


void acq_completed_cb(void)
{
  /*
  The following code shows an example of how to access raw data buffer
  
  acq_buf_obj *p_acq_buf 	= ds_get_active_acq_buf();
  uint8_t *raw_data 		= p_acq_buf->p_acq_buf;
  uint32_t raw_data_size 	= p_acq_buf->used_size_of_acq_buffer;
  
  -- Add your code here --
  
  */

	/*Inizializzo comunicazione*/
	if (USBD_VCOM_Connect() != USBD_VCOM_STATUS_SUCCESS)
	{
		return -1;
	}
	while(!USBD_VCOM_IsEnumDone());



	static uint32_t start_time = 0;
	acq_buf_obj *p_acq_buf2;
	uint8_t *raw_data2;

	    switch (state) {
	        case WAIT_BACKGROUND:
	            USBD_VCOM_SendString("\r\nPosizionarsi LONTANO dal radar.\r\n");
	            USBD_VCOM_SendString("Inizio acquisizione background tra 5 secondi...\r\n");
	            start_time = TIMER_GetTime(&TIMER_0);
	            state = ACQUIRE_BACKGROUND;
	            break;

	        case ACQUIRE_BACKGROUND:
	            if (TIMER_GetTime(&TIMER_0) - start_time < WAIT_TIME) return;  // Attendi 5 sec

	            acq_buf_obj *p_acq_buf = ds_get_active_acq_buf();
	            uint8_t *raw_data = p_acq_buf->p_acq_buf;
	            uint32_t raw_data_size = p_acq_buf->used_size_of_acq_buffer;

	            float signal[FFT_SIZE] = {0};
	            float complex_fft_signal[FFT_SIZE * 2] = {0};  // Buffer per FFT complessa
	            float fft_result[FFT_SIZE / 2] = {0};

	            /* Campionamento e conversione del segnale */
	            for (int i = 0; i < FFT_SIZE; i++) {
	            	signal[i] = raw_data[i * DOWNSAMPLING_FACTOR];
	            }

	            /* Calcolo della FFT */
	            compute_fft_signal(fft_window, signal, NULL, FFT_SIZE, FFT_SIZE, 1.0f, FFT_INPUT_REAL_I, NULL, NULL, complex_fft_signal);
	            compute_fft_spectrum(complex_fft_signal, FFT_SIZE, fft_result);

	            /* Aggiornamento del background */
	            for (int i = 0; i < FFT_SIZE / 2; i++) {
	            	background_fft[i] += fft_result[i] / BACKGROUND_FRAMES;
	            }

	            frame_count++;
	            if (frame_count >= BACKGROUND_FRAMES) {
	            	USBD_VCOM_SendString("\r\nBackground acquisito.\r\n");
	            	USBD_VCOM_SendString("Posizionarsi DAVANTI al radar.\r\n");
	            	USBD_VCOM_SendString("Misurazione tra 5 secondi...\r\n");
	                start_time = TIMER_GetTime(&TIMER_0);
	                state = WAIT_PERSON;
	            }
	            break;

	        case WAIT_PERSON:
	        	if (TIMER_GetTime(&TIMER_0) - start_time < WAIT_TIME) return;
	        	USBD_VCOM_SendString("Inizio misurazione respiro...\r\n");
	            state = MEASURE_BREATH;
	            break;

	        case MEASURE_BREATH:
	        	p_acq_buf2 = ds_get_active_acq_buf();
	        	raw_data2 = p_acq_buf2->p_acq_buf;

	            float signal2[FFT_SIZE] = {0};
	            float complex_fft_signal2[FFT_SIZE * 2] = {0};
	            float fft_result2[FFT_SIZE / 2] = {0};
	            float clean_fft[FFT_SIZE / 2] = {0};

	            /* Campionamento e conversione del segnale */
	            for (int i = 0; i < FFT_SIZE; i++) {
	            	signal2[i] = raw_data2[i * DOWNSAMPLING_FACTOR];
	            }

	            /* Calcolo della FFT */
	            compute_fft_signal(fft_window, signal2, NULL, FFT_SIZE, FFT_SIZE, 1.0f, FFT_INPUT_REAL_I, NULL, NULL, complex_fft_signal2);
	            compute_fft_spectrum(complex_fft_signal2, FFT_SIZE, fft_result2);

	            /* Sottrazione del background */
	            for (int i = 0; i < FFT_SIZE / 2; i++) {
	            	clean_fft[i] = fft_result2[i] - background_fft[i];
	            }

	            /* Identificazione della frequenza dominante */
	            float max_amplitude = 0;
	            float breathing_frequency = 0;
	            for (int i = 1; i < FFT_SIZE / 2; i++) {  // Ignora la DC (indice 0)
	            	float freq = (float)i / FFT_SIZE;
	            	if (freq > 0.1f && freq < 1.0f) {  // Frequenze tra 6 BPM e 60 BPM
	            		if (clean_fft[i] > max_amplitude) {
	            			max_amplitude = clean_fft[i];
	            			breathing_frequency = freq;
	            		}
	            	}
	            }

	            /* Calcolo della frequenza respiratoria in BPM */
	            float BRPM = breathing_frequency * 60;

	            char msg[50];
	            sprintf(msg, "BRPM: %.2f\r\n", BRPM);
	            USBD_VCOM_SendString(msg);
	            break;
	    }
	}



void algo_completed_cb(void)
{
  /*
  * The follow sample code could be seen as a small example, how to get the information from the algorithm and use it accordingly.
  */

  /*
  static float s_max_level = 0;
  static uint32_t countFrames = 0;
  static uint32_t s_max_num_targets = 0;
  
  extern Radar_Handle_t h_radar_device;
  
  Target_Info_t target_info[MAX_NUM_OF_TARGETS];
  
  uint8_t num_targets;
  
  if (radar_get_target_info(h_radar_device, target_info, &num_targets) == RADAR_ERR_OK)
  {
    float max_level = 0;
    for(int i= 0; i< num_targets; i++)
    {
      if (target_info[i].level > max_level)
      {
        s_max_level = target_info[i].level;
      }
    }
    countFrames += 1;
    
    if (num_targets > s_max_num_targets)
    {
      s_max_num_targets = num_targets;
    }
  }
  */
}

/* --- End of File -------------------------------------------------------- */
