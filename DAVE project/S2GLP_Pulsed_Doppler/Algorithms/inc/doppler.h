/**
    @file: doppler.h

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

#ifndef DOPPLER_H_
#define DOPPLER_H_

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */

#include "config.h"
#include "datastore.h"
#include "dsp_lib.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define FFT_SIZE                DOPPLER_FFT_SIZE
#define DOPPLER_IQ_SCALE        (8U)

/*
==============================================================================
   3. DATA
==============================================================================
 */


/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  Performs the doppler scaling \ref DOPPLER_IQ_SCALE to the raw IQ data and calls Doppler post-processing function.
 *
 * \param[in]	p_acq_buf           Pointer to the raw I and Q data stream
 * \param[in]	cp_algo_settings    Pointer to the current Doppler algorithm settings
 * \param[in]	cp_dev_settings     Pointer to the current hardware settings
 * \param[in]	p_algo_result       Pointer to the output Doppler algorithm results
 *
 */
 void doppler_do(acq_buf_obj *p_acq_buf, const algo_settings_t *cp_algo_settings,
 		        const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result);


/**
 * \brief  Performs the doppler post-processing means computation of spectrum, detecting speed and direction of motion.
 *
 * Internally this function calculates the complex FFT spectrum and apply speed thresholds for motion detection event
 * and determine target velocity in KM Per Hour (kmH).
 *
 * \param[in]	fft_window           FFT time window used to compute FFT
 * \param[in]	if1_i                Pointer to the raw I data stream of 32-bit for rx1
 * \param[in]	if1_q                Pointer to the raw Q data stream of 32-bit for rx1
 * \param[in]	number_samples       Current number of samples peer frame
 * \param[in]	cp_algo_settings     Pointer to the current Doppler algorithm settings
 * \param[in]	cp_dev_settings      Pointer to the current hardware settings
 * \param[in]	p_algo_result      	 Pointer to the processing result struct
 *
 */
void doppler_calc_speed(FFT_Window_Struct_t fft_window, float* if1_i, float* if1_q, uint16_t number_samples,
		                const algo_settings_t  *cp_algo_settings, const device_settings_t *cp_dev_settings, algo_result_t *p_algo_result);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DOPPLER_H_ */

/* --- End of File -------------------------------------------------------- */
