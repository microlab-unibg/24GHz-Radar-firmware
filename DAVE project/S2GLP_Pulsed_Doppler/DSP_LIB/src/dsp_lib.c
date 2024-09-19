/**
    @file: dsp_lib.c

  	@brief: This library includes basic functions like FFT and windowing functions etc.
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

#include "dsp_lib.h"
#include "math.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

#define MAX_FFT_WIN_LEN		          (256U)		/**< Max Window length for FFT windowing over time */

/*
==============================================================================
   3. DATA
==============================================================================
 */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief This function generates the coefficients of a user selected window for a given length.
 *
 * \param [in]       window_type     Define the type of window \ref FFT_Window_Type_t
 * \param [in]       window_size     Define the number of elements in the window:
 *                                   For fast FFT window, it is the the number of sample per chirp or per frame
 *                                   For slow FFT window, it is the number of chirp per frame
 * \param [in]       at_dB           Attenuation parameter used only in case of Chebyshev window
 *                                   It defines the attenuation in dBs required to generate the pass
 *                                   band ripple for a Chebyshev window.This must be a positive number.
 * \param [in, out]  window_buffer   Pointer to an allocated buffer to filled with Window coefficients
 *
 * \return uint8_t   0 for Error and non-zero for success case.
 */
static uint8_t fft_window_init(FFT_Window_Type_t window_type, uint32_t window_size, float at_dB, float *window_buffer);

/**
 * @brief  This function generates the coefficients of a Hamming window for a given length.
 *
 * @param [in]       window_size     Define the number of elements in the window
 * @param [in, out]  window_buffer   Pointer to an allocated buffer to filled with Window coefficients
 *
 * @return uint8_t   0 for Error and non-zero for success case.
 */
static uint8_t fft_window_init_hamming(uint32_t window_size, float *window_buffer);

/**
 * @brief  This function generates the coefficients of a Hanning window for a given length.
 *
 * @param [in]       window_size     Define the number of elements in the window
 * @param [in, out]  window_buffer   Pointer to an allocated buffer to filled with Window coefficients
 *
 * @return uint8_t   0 for Error and non-zero for success case.
 *
 * @return uint8_t Error code.
 */
static uint8_t fft_window_init_hanning(uint32_t window_size, float *window_buffer);

/**
 * @brief  This function generates the coefficients of a Blackman window for a given length.
 *
 * @param [in]       window_size     Define the number of elements in the window
 * @param [in, out]  window_buffer   Pointer to an allocated buffer to filled with Window coefficients
 *
 * @return uint8_t   0 for Error and non-zero for success case.
 */
static uint8_t fft_window_init_blackmanharris(uint32_t window_size, float *window_buffer);

/**
 * @brief  This function generates the coefficients of a Chebyshev window for a given length.
 *
 * @param [in]       window_size     Define the number of elements in the window
 * @param [in]       at_dB Defines the attenuation in dBs required to generate
 *                       the pass band ripple for a Chebyshev window.
 *                       This must be a positive number.
 * @param [in, out]  window_buffer   Pointer to an allocated buffer to filled with Window coefficients
 *
 *       Reference: Dolph Chebyshev window according to
 *                  http://practicalcryptography.com/miscellaneous/
 *                  machine-learning/implementing-dolph-chebyshev-window/
 *
 * @return uint8_t   0 for Error and non-zero for success case.
 */
static uint8_t fft_window_init_chebyshev(uint32_t window_size, float *window_buffer, float at_dB);

/**
 * @brief  Computes Chebyshev polynomial of the first kind.
 *
 * @param [in] size       Size of Chebyshev polynomial
 * @param [in] poly_order Order of the polynomial.
 *
 * @return Return value from the Chebyshev polynomial.
 */
static float cheby_poly(const uint32_t size, const float poly_order);

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
 */

/*--------------------- fft_window_init_hamming() ------------------------*/
static uint8_t fft_window_init_hamming(uint32_t window_size, float *window_buffer)
{
  if (window_size == 1)
  {
	window_buffer[0] = 1;
  }
  else
  {
    for (uint32_t i = 0; i < window_size; ++i)
    {
      window_buffer[i] = 0.54f - 0.46f * arm_cos_f32(2.0f * M_PI * (float)i / (float)(window_size - 1));
    }
  }

  return 1;
}

/*----------------------- fft_window_init_hanning() ----------------------*/
static uint8_t fft_window_init_hanning(uint32_t window_size, float *window_buffer)
{
  if (window_size == 1)
  {
	window_buffer[0] = 1;
  }
  else
  {
    for (uint32_t i = 0; i < window_size; ++i)
    {
      window_buffer[i] = 0.5f * (1.0f - arm_cos_f32(2 * M_PI * (float)i / (float)(window_size - 1)));
    }
  }
  return 1;
}

/*----------------- fft_window_init_blackmanharris() ---------------------*/
static uint8_t fft_window_init_blackmanharris(uint32_t window_size, float *window_buffer)
{
  if (window_size == 1)
  {
	window_buffer[0] = 1;
  }
  else
  {
    const float a0 = 0.35875f;
    const float a1 = 0.48829f;
    const float a2 = 0.14128f;
    const float a3 = 0.01168f;

   for (uint32_t i = 0; i < window_size; ++i)
   {
	 window_buffer[i] = a0
                        - (a1 * arm_cos_f32( (2.0f * M_PI * (float)i) / (window_size - 1) ))
                        + (a2 * arm_cos_f32( (4.0f * M_PI * (float)i) / (window_size - 1) ))
                        - (a3 * arm_cos_f32( (6.0f * M_PI * (float)i) / (window_size - 1) ));
    }
  }
  return 1;
}

/*-------------------- fft_window_init_chebyshev() -----------------------*/
static uint8_t fft_window_init_chebyshev(uint32_t window_size, float *window_buffer, float at_dB)
{
  if (window_size == 1)
  {
	window_buffer[0] = 1;
  }
  else
  {
    float pssbnd_ripple = powf(10.0f, at_dB / 20.0f);
    float alpha = coshf((1.0f / (float)(window_size - 1)) * acoshf(pssbnd_ripple));
    float win_even = (float)((window_size - 1) / 2);

    if (window_size % 2 == 0)
    {
      win_even = win_even + 0.5f;
    }

    float max_val = 0;
    for (uint32_t j = 0; j < (window_size / 2 + 1); ++j)
    {
      float n = j - win_even;
      float sum = 0;

      for (uint32_t i = 1; i <= win_even; ++i)
      {
       	float poly_order = alpha * arm_cos_f32( (M_PI * i) / window_size);
       	float res = cheby_poly( window_size - 1, poly_order);
        sum += res * arm_cos_f32( 2.0f * n * M_PI * i / (float)window_size );
      }

      window_buffer[j] = pssbnd_ripple + 2 * sum;
      window_buffer[window_size - j - 1] = window_buffer[j];

      if (window_buffer[j] > max_val)
      {
        max_val = window_buffer[j];
      }
    }

    for (uint32_t j = 0; j < window_size; ++j)
    {
      window_buffer[j] /= max_val;
    }
  }
  return 1;
}

/*----------------------------- cheby_poly() -----------------------------*/
static float cheby_poly(const uint32_t size, const float poly_order)
{
  float ret = 0;

  if (fabsf(poly_order) <= 1)
  {
    ret = arm_cos_f32( size * acosf(poly_order));
  }
  else
  {
   	  ret = coshf( size * acoshf(poly_order)); 
  }

  return ret;
}

/*-------------------------- fft_window_init() ---------------------------*/
static uint8_t fft_window_init(FFT_Window_Type_t window_type, uint32_t window_size, float at_dB, float *window_buffer)
{
  if ((window_buffer == NULL) || (window_size == 0) || (window_size > MAX_FFT_WIN_LEN))
  {
    return 0;
  }

  switch (window_type)
  {
  case FFT_WINDOW_HAMM:
	  return fft_window_init_hamming(window_size, window_buffer);
	  break;

  case FFT_WINDOW_HANN:
	  return fft_window_init_hanning(window_size, window_buffer);
	  break;

  case FFT_WINDOW_BLACKMANHARRIS:
	  return fft_window_init_blackmanharris(window_size, window_buffer);
	  break;

  case FFT_WINDOW_CHEBYSHEV:
	  return fft_window_init_chebyshev(window_size, window_buffer, at_dB);
	  break;

  default:
	  return 0;
	  break;
  }
   return 1;
}

/*
==============================================================================
   5. EXPORTED FUNCTIONS
==============================================================================
 */

/*-------------------------- fft_init() ------------------------------------*/
uint8_t fft_window_gen(FFT_Window_Struct_t *fft_window)
{
  if ((fft_window->fft_window_buffer == NULL) ||
	   (fft_window->fft_window_length == 0) ||
	   (fft_window->fft_window_length > MAX_FFT_WIN_LEN))
  {
    return 0;
  }

  switch (fft_window->fft_size)
  {
  case FFT_SIZE_16:
	  fft_window->fft_time_twiddle_table = &arm_cfft_sR_f32_len16;		/* as FFT_SIZE = 16 points */
	  break;

  case FFT_SIZE_32:
	  fft_window->fft_time_twiddle_table = &arm_cfft_sR_f32_len32;		/* as FFT_SIZE = 32 points */
	  break;

  case FFT_SIZE_64:
	  fft_window->fft_time_twiddle_table  = &arm_cfft_sR_f32_len64;		/* as FFT_SIZE = 64 points */
	  break;

  case FFT_SIZE_128:
	  fft_window->fft_time_twiddle_table  = &arm_cfft_sR_f32_len128;	/* as FFT_SIZE = 128 points */
	  break;

  case FFT_SIZE_256:
	  fft_window->fft_time_twiddle_table  = &arm_cfft_sR_f32_len256;	/* as FFT_SIZE = 256 points */
	  break;

  default:
	  return 0;
	  break;
  }

  return(fft_window_init(fft_window->fft_window_type,fft_window->fft_window_length,
		 fft_window->fft_chebyshev_at_dB, fft_window->fft_window_buffer));
}

/*-------------------------- compute_fft_signal() --------------------------*/
void compute_fft_signal(FFT_Window_Struct_t fft_window, float* i_data, float* q_data, uint16_t Nd,
		                uint16_t Nf, float if_scale, FFT_Input_t fft_type,
						float* p_i_mean, float* p_q_mean, float* complex_fft_signal)
{
	float i_mean = 0.0f;
	float q_mean = 0.0f;
	uint32_t idx;
	float *fft_window_buffer = fft_window.fft_window_buffer;

	if (!complex_fft_signal)
		return;

	if (!fft_window_buffer)
		return;

	if (fft_type == FFT_INPUT_REAL_I)
	{
		/* Find the mean in i_data */
		arm_mean_f32(i_data, Nd, &i_mean);

		/* Interleaved (re = I & im = 0) samples as {re[0], im[0], re[1], im[1], ...} */
		for (idx = 0; idx < Nd; idx++)
		{
			complex_fft_signal[2 * idx + 0] = 2.0 * (float)(i_data[idx] - i_mean) * if_scale * fft_window_buffer[idx];	// additional scaling by 2 for real input FFT
			complex_fft_signal[2 * idx + 1] = 0;
		}
	}
	else if (fft_type == FFT_INPUT_REAL_Q)
	{
		/* Find the mean in q_data */
		arm_mean_f32(q_data, Nd, &q_mean);

		/* Interleaved (re = Q & im = 0) samples as {re[0], im[0], re[1], im[1], ...} */
		for (idx = 0; idx < Nd; idx++)
		{
			complex_fft_signal[2 * idx + 0] = 2.0 * (float)(q_data[idx] - q_mean) * if_scale * fft_window_buffer[idx];	// additional scaling by 2 for real input FFT
			complex_fft_signal[2 * idx + 1] = 0;
		}
	}
	else
	{
		/* Find the mean in q_data */
		arm_mean_f32(q_data, Nd, &q_mean);

		/* Find the mean in i_data */
		arm_mean_f32(i_data, Nd, &i_mean);

		/* Interleaved (re = I & im = Q) samples as {re[0], im[0], re[1], im[1], ...} */
		for (idx = 0; idx < Nd; idx++)
		{
			complex_fft_signal[2 * idx + 0] = (float)(i_data[idx] - i_mean) * if_scale * fft_window_buffer[idx];
			complex_fft_signal[2 * idx + 1] = (float)(q_data[idx] - q_mean) * if_scale * fft_window_buffer[idx];
		}
	}

	/* Zero Padding */
	for (uint16_t idx = Nd; idx < Nf; idx++)
	{
		complex_fft_signal[2 * idx + 0] = 0;
		complex_fft_signal[2 * idx + 1] = 0;
	}

	/* copy mean values into output pointers */
	if ( p_i_mean )
		*p_i_mean = i_mean;

	if ( p_q_mean )
		*p_q_mean = q_mean;

	/* Processing the floating-point complex FFT. */
	arm_cfft_f32(fft_window.fft_time_twiddle_table, complex_fft_signal, 0, 1);
}

/*-------------------------- compute_fft_spectrum() ------------------------*/
void compute_fft_spectrum(float* fft_input_signal, uint32_t Nf, float* fft_output_spectrum)
{
	/* Convert to real magnitude data */
	arm_cmplx_mag_f32(fft_input_signal, fft_output_spectrum, Nf);
}

/* --- End of File -------------------------------------------------------- */
