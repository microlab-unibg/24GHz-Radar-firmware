/**
    @file: sense2gol_usb.h

    @brief: This file contains function to deal with usb.
 */

/* ===========================================================================
** Copyright (C) 2019-2021 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorization.
** ===========================================================================
*/

#ifndef SENSE2GOL_USB_H_
#define SENSE2GOL_USB_H_

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

#include <stdint.h>
#include <dave.h>

/*
==============================================================================
   2. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief  This function initializes the USB low layer driver and starts USB data communication.
 *
 * \param[in]  timeout_delay  Unsigned 32-bit counter value, representing number of attempts to be made to connect to the VCOM at the Host
 *
 * \return  Returns "0" if successful else "1" for failure.
 */
uint8_t bsp_usb_init(uint32_t timeout_delay);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* SENSE2GOL_USB_H_ */

/* --- End of File -------------------------------------------------------- */
