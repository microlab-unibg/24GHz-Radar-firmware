/**
 * \file radar_common.h
 *
 * \brief This file contains the API methods applicable generically to Industrial radar demo Sence2Go-Pulse and Distance2GoL
 *
 */

/* ===========================================================================
 * Copyright (C) 2017-2021 Infineon Technologies AG
 * All rights reserved.
 * ===========================================================================
 *
 * ===========================================================================
 * This document contains proprietary information of Infineon Technologies AG.
 * Passing on and copying of this document, and communication of its contents
 * is not permitted without Infineon's prior written authorisation.
 * ===========================================================================
 */

#ifndef RADAR_COMMON_H_INCLUDED
#define RADAR_COMMON_H_INCLUDED

/**
* \brief This enumeration type lists the possible formats of sampled radar
*        data.
*
* The format of sampled radar data acquired by the driver and retrieved
* through a struct of type \ref Frame_Info_t by calling
* \ref ep_radar_base_get_frame_data depends on the radar sensor hardware.
*
* The radar front end comprises a mixer that demodulates the RF signal
* received through the connected antennas down to the base band, where it is
* captured by an Analog/Digital Converter (ADC). A RF mixer can output two
* signals with a phase shift of 90?, called I-signal and Q-signal. These two
* signals can be considered to be real and imaginary part of a complex valued
* signal. Depending on the sensor hardware, the sensor may be able to capture
* only one or both signals with its ADCs.
*
* If only one signal is captured, the radar frame data is real valued, so a
* block of N samples consists of N values. If both signals are captured, a
* block of N samples consists of 2*N values, which may be interleaved (each
* real value is followed by the according imaginary value) or straight (N real
* samples followed by N imaginary samples).
*
*/
typedef enum
{
	EP_RADAR_BASE_RX_DATA_REAL = 0, /**< The frame data
									contains only I or Q
									signal. */
	EP_RADAR_BASE_RX_DATA_COMPLEX = 1, /**< The frame data
									   contains I and Q
									   signals in separate
									   data blocks. */
	EP_RADAR_BASE_RX_DATA_COMPLEX_INTERLEAVED = 2  /**< The frame data
												   contains I and Q
												   signals in one
												   interleaved data
												   block. */
} Rx_Data_Format_t;

/**
* \brief This enumeration type lists the possible signals parts formats of
*        sampled radar data.
*
* If the device is capable to capture complex signal data (I and Q), the
* values of this enumeration allow to chose between the full complex signal,
* or just the real or imaginary signal part.
*
* A value of this type is part of the struct \ref Frame_Format_t.
*/
typedef enum
{
	EP_RADAR_BASE_SIGNAL_ONLY_I = 0, /**< Only the I signal is captured
									 during radar data frame
									 acquisition. */
	EP_RADAR_BASE_SIGNAL_ONLY_Q = 1, /**< Only the Q signal is captured
									 during radar data frame
									 acquisition. */
	EP_RADAR_BASE_SIGNAL_I_AND_Q = 2  /**< Both, I and Q signal are captured
									  as a complex signal during radar
									  data frame acquisition. */
} Signal_Part_t;

/**
 * \brief This struct holds all information about a single frame of radar
 *        data.
 *
 * A structure of this type is returned through the callback
 * \ref Callback_Data_Frame_t when \ref ep_radar_base_get_frame_data is
 * called.
 *
 * The data type of samples in sample_data depends on the ADC resolution. If
 * adc_resolution <= 8, sample_data points to an array of uint8_t. If
 * 8 < adc_resolution <= 16, sample_data points to an array of uint16_t.
 * Higher ADC resolutions are currently not supported.
 *
 * If the frame contains more than one chirp, the chirps are stored in
 * consecutive data blocks. The start of each chirp can be calculated by the
 * following formula.
 * \code
 * frame_start = &sample_data[CHIRP_NUMBER *
                              num_rx_antennas *
                              num_samples_per_chirp *
 *                            ((data_format == RADAR_RX_DATA_REAL)? 1 : 2)];
 * \endcode
 *
 * The index calculation of a certain data sample captured from a certain RX
 * antenna depends on the data interleaving and on real/complex data format.
 * The following code examples demonstrate how to access a data sample.
 * \code
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_REAL
 * // --------------------------------------------------------------------
 * data_value = frame_start[ANTENNA_NUMBER * num_samples_per_chirp +
 *                          SAMPLE_NUMBER];
 *
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_COMPLEX
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[(2 * ANTENNA_NUMBER    ) *
 *                               num_samples_per_chirp + SAMPLE_NUMBER];
 * data_value_imag = frame_start[(2 * ANTENNA_NUMBER + 1) *
 *                               num_samples_per_chirp + SAMPLE_NUMBER];
 *
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_COMPLEX_INTERLEAVED
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp +
 *                               SAMPLE_NUMBER];
 * data_value_imag = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp +
 *                               SAMPLE_NUMBER + 1];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_REAL
 * // --------------------------------------------------------------------
 * data_value = pFrameStart[SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_COMPLEX
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER];
 * data_value_imag = frame_start[(num_samples_per_chirp + SAMPLE_NUMBER) *
 *                               num_rx_antennas + ANTENNA_NUMBER];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_COMPLEX_INTERLEAVED
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER];
 * data_value_imag = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER + 1];
 * \endcode
 *
 * In the code examples above, ANTENNA_NUMBER does not refer to the physical
 * antenna number, but the n-th enabled antenna. If for example rx_mask = 0xA,
 * ANTENNA_NUMBER=0 refers to antenna 0010b and ANTENNA_NUMBER=1 refers to
 * antenna 1000b. Furthermore it is assumed that pDataBuffer and frame_start
 * are casted to (uint8_t*) or (uint16_t*) according to ADC resolution.
 *
 * In total the data buffer contains num_chirps * num_rx_antennas *
 * num_samples_per_chirp values. If radar data is complex, that value is
 * multiplied by 2.
 */
typedef struct
{
    const float*     sample_data;            /**< The buffer containing the
                                                  radar data */
    uint32_t         frame_number;           /**< The running number of the
                                                  data frame. The frame
                                                  counter is, reset every time
                                                  \ref ep_radar_base_set_automatic_frame_trigger
                                                  is called. If automatic
                                                  frame trigger is not active,
                                                  the frame counter may not
                                                  work, and this could be 0.
                                                  */
    uint32_t         num_chirps;             /**< The number of chirps in this
                                                 frame. */
    uint8_t          num_rx_antennas;        /**< The number of RX signals
                                                  that have been acquired with
                                                  each chirp. */
    uint32_t         num_samples_per_chirp;  /**< The number of samples
                                                  acquired in each chirp for
                                                  each enabled RX antenna. */
    uint8_t          rx_mask;                /**< Each antenna is represented
                                                  by a bit in this mask. If
                                                  the bit is set, the
                                                  according RX antenna was
                                                  used to capture data in this
                                                  frame. */
    uint8_t          adc_resolution;         /**< The ADC resolution of the
                                                  data in sample_data. */
    uint8_t          interleaved_rx;         /**< If this is 0, the radar data
                                                  of multiple RX antennas is
                                                  stored in consecutive data
                                                  blocks, where each block
                                                  holds data of one antenna.
                                                  If this is non-zero, the
                                                  radar data of multiple RX
                                                  antennas is stored in one
                                                  data block, where for each
                                                  point in time the samples
                                                  from all RX antennas are
                                                  stored consecutively before
                                                  the data of the next point
                                                  in time follows. */
    Rx_Data_Format_t data_format;            /**< This indicates if the data
                                                  is sample_data is real or
                                                  complex, and if complex data
                                                  is interleaved. */
} get_raw_data_cnf_t;



typedef void(*get_raw_data_cnf_callback_t)(void *p_context, int32_t protocol_handle,
											uint8_t endpoint, get_raw_data_cnf_t *ps_raw_data);


#endif
