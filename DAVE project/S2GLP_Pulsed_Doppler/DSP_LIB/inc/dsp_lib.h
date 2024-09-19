/**
    @file: dsp_lib.h

  	@brief: This Library includes basic functions like FFT Signal/Spectrum, Angle calculation
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

#ifndef DSP_LIB_H_
#define DSP_LIB_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "DAVE.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#include "config.h"

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
 * @brief Defines supported Window options.
 * @{
 */
typedef enum
{
    FFT_WINDOW_HAMM           = 1U,    /**< Hamming window */
    FFT_WINDOW_HANN           = 2U,    /**< Hanning window */
    FFT_WINDOW_BLACKMANHARRIS = 3U,    /**< Blackmann Harris window */
    FFT_WINDOW_CHEBYSHEV      = 4U,    /**< Chebyshev window */
} FFT_Window_Type_t;

/** @} */

/**
 * \brief Defines supported FFT input options. Use type FFT_Input_t for this enum.
 * @{
 */
typedef enum
{
   FFT_INPUT_REAL_I    = 1U,  	/**< Only real input I channel for Complex FFT */
   FFT_INPUT_REAL_Q    = 2U,  	/**< Only real input Q channel for Complex FFT */
   FFT_INPUT_COMPLEX   = 3U		/**< Complex input IQ channels for Complex FFT */
}  FFT_Input_t;

/** @} */

/**
 * \brief Defines supported FFT window size, it is the number of floating point elements in the window.
 *        Use type FFT_Window_Size_t for this enum.
 * @{
 */
typedef enum
{
   FFT_SIZE_16    = 16U,  	/**< FFT size, 16 floating points */
   FFT_SIZE_32    = 32U,  	/**< FFT size, 32 floating points */
   FFT_SIZE_64    = 64U,  	/**< FFT size, 64 floating points */
   FFT_SIZE_128   = 128U,  	/**< FFT size, 128 floating points */
   FFT_SIZE_256   = 256U  	/**< FFT size, 256 floating points */
}  FFT_Size_t;

/** @} */

/**
 * \brief Defines FFT window structure settings.
 * @{
 */
typedef struct
{
  FFT_Window_Type_t             fft_window_type;              /* Type of window defined by \ref FFT_Window_Type_t */
  FFT_Size_t                    fft_size;                     /* FFT size \ref FFT_Size_t  */
  uint32_t                      fft_window_length;            /* Number of elements in the FFT window */
  float                         fft_chebyshev_at_dB;          /* Attenuation parameter needed in case of Chebyshev window */
  float                         *fft_window_buffer;           /* Pointer to floating point memory containing data values for FFT windowing over time */
  const arm_cfft_instance_f32   *fft_time_twiddle_table;      /* Pointer to twiddle factor table for FFT windowing over time */
} FFT_Window_Struct_t;

/** @} */

/*
==============================================================================
   4. EXPORTED DATA
==============================================================================
 */


/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  This function generates the coefficients of a user selected window for a given length.
 *         It Performs the initialization of FFT complex structure used by CMSIS library for FFT calculation.
 *
 * \param [in]  fft_window  Structure defining the type of window, the number of elements in the window
 *                          and also any additional parameter specific to certain window type
 *                          \ref FFT_Window_Struct_t.
 *
 * \return      0 for Error and non-zero for success case.
 */
uint8_t fft_window_gen(FFT_Window_Struct_t *fft_window);

/**
 * \brief  This function computes the FFt signal out of raw ADC samples.
 *
 *  Internally it computes mean of respective I & Q signal and subtract it before applying IF scaling and Windowing.
 *  Afterwards computes the FFT signal and returns the Nf number of complex samples.
 *
 * \param[in]	*i_data		Pointer of type signed 16-bit integer, containing the address of the I data buffer
 * \param[in]	*q_data		Pointer of type signed 16-bit integer, containing the address of the Q data buffer
 * \param[in]	Nd			Unsigned 16-bit integer, containing the size of raw ADC IQ data buffer
 * \param[in]	Nf			Unsigned 16-bit integer, containing the size of FFT complex values array
 * \param[in]	if_scale	Floating point scale applied to the FFT spectrum to enhance the visibility of targets
 * \param[in]	fft_type	Complex or Real input FFT to be computed defined by \ref FFT_Input_t
 * \param[in]	fft_window	FFT time window used to compute FFT \ref FFT_Window_Struct_t
 *
 * \param[out]  *p_i_mean	Pointer to a floating point value, containing the mean of the I channel
 * \param[out]  *p_q_mean	Pointer to a floating point value, containing the mean of the Q channel
 * \param[out]  *complex_fft_signal		Pointer to a floating point array, to return the complex FFT signal in interleaved I&Q format.
 *
 */
void compute_fft_signal(FFT_Window_Struct_t fft_window, float* i_data, float* q_data, uint16_t Nd,
		                uint16_t Nf, float if_scale, FFT_Input_t fft_type,
						float* p_i_mean, float* p_q_mean, float* complex_fft_signal);

/**
 * \brief  This function computes the FFt spectrum out of raw ADC samples.
 *
 *  Internally it computes mean of respective I & Q signal and subtract it before applying IF scaling and Windowing.
 *  Afterwards computes the FFT signal and returns the Nf number of real samples as FFT spectrum.
 *
 * \param[in]	*fft_input_signal		Pointer of type float, containing the address of the Complex FFT signal with interleaved IQ
 * \param[in]	Nf						Unsigned 32-bit integer, containing the size of FFT complex values array
 *
 * \param[out]  *fft_output_spectrum	Pointer to a floating point array, to return the real valued FFT spectrum.
 *
 */
void compute_fft_spectrum(float* fft_input_signal, uint32_t Nf, float32_t* fft_output_spectrum);

/**
 * \brief  This function computes the phase (in radian) from real and imaginary part of FFT signal.
 *
 * \param[in]	real		Real component of FFT signal for the desired detected target bin.
 * \param[in]	imag		Imaginary component of FFT signal for the desired detected target bin.
 *
 * \return	Phase in units of radians between (0 , 2Pi).
 *
 */
double get_phase(float real, float imag);

#endif /* DSP_LIB_H_ */

/* --- End of File -------------------------------------------------------- */
