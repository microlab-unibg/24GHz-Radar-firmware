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

  /*Inizializzo comunicazione*/
  		if (USBD_VCOM_Connect() != USBD_VCOM_STATUS_SUCCESS)
  		{
  			USBD_VCOM_SendString("\r\nCollegamento fallito\r\n");
  			while (1U);
  		}


  /*inizializzo tempo*/

  if (status == DAVE_STATUS_SUCCESS){
  	timer_status = TIMER_Start(&TIMER_0);
  }

  /* Register algorithm processing function:
     Set the algorithm processing function pointer, it will
     be used by the application for algorithm data processing */
    app_register_algo_process(breathing_do);

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
