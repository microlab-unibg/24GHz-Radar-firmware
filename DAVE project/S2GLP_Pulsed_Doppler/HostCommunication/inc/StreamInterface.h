/**
 * \file StreamInterface.h
 *
 * \brief This header defines the communication interface needed by the
 *        protocol module.
 *
 * The module Protocol.c requires a communication interface to send and
 * receive data. That communication interface is treated as a simple
 * bidirectional byte stream interface. An implementation of each function
 * declared in this header must be provided in order to work with the
 * communication protocol.
 *
 * Incoming data from a host is stored in an internal receive buffer. Data is
 * retrieved from that buffer by calling \ref com_get_data.
 *
 * Data is sent through this interface by calling \ref com_send_data. Data can
 * be transferred directly by that function or it may be stored in an internal
 * transmit buffer to be transmitted later. If an internal transmit buffer is
 * used, an immediate transmission can be forced by calling \ref com_flush.
 */

/* ===========================================================================
** Copyright (C) 2016-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef HOSTCOMMUNICATION_STREAMINTERFACE_H_
#define HOSTCOMMUNICATION_STREAMINTERFACE_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
 */
#include <stdint.h>

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
 */

/**
 * \brief This function initializes the stream interface.
 *
 * This function does everything needed to prepare the communication interface
 * for operation. This function is called once during protocol initialization.
 */
void com_init();

/**
 * \brief This function sends a buffer of data through the communication
 *        interface.
 *
 * The function sends the data in the provided buffer to the host through the
 * communication interface. This function may use an internal transmit buffer
 * and copy the data to that buffer instead of sending it directly. By using
 * an internal transmit buffer data from consecutive calls to this function
 * can be combined to a single transaction to reduce overhead. The function
 * \ref com_flush can be used to send all data from the internal buffer
 * immediately.
 *
 * \param[in] data       A pointer to the buffer that contains the data to be
 *                       sent.
 * \param[in] num_bytes  The number of bytes in the buffer to be sent.
 */
void com_send_data(const void* data, uint16_t num_bytes);

/**
 * \brief This function retrieves data that has been received through the
 *        communication interface.
 *
 * This function tries to retrieve the requested number of bytes from the
 * internal receive buffer and copies them to the provided buffer. If the
 * internal receive buffer does not contain enough data this function will not
 * wait but copy all data from the receive buffer and return the number of
 * bytes that have been copied to the provided buffer.
 *
 * \param[in] data       A pointer to the buffer where the received data will
 *                       be copied to.
 * \param[in] num_bytes  The number of bytes that should be copied to data.
 *                       It is assumed that the capacity of data is
 *                       sufficient.
 *
 * \return The function returns the number of bytes that have been copied to
 *         the buffer.
 */
uint16_t com_get_data(void* data, uint16_t num_bytes);

/**
 * \brief This function sends all data from the internal transmit buffer.
 *
 * If an implementation of the communication interfaces contain an internal
 * transmit buffer, this functions sends all data from that buffer to the
 * host. After a call to this function the transmit buffer is empty.
 */
void com_flush();

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* HOSTCOMMUNICATION_STREAMINTERFACEVCOM_H_ */

/* --- End of File -------------------------------------------------------- */
