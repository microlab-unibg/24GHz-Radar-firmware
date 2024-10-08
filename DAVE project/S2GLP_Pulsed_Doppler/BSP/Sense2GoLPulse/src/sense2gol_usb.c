/**
    @file: sense2gol_usb.c

    @brief: This file provides function to deal with usb.
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

#include "sense2gol_usb.h"

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

uint8_t bsp_usb_init(uint32_t timeout_delay)
{
	uint32_t timeout_cnt = 0;

	if (USBD_VCOM_Connect() != USBD_VCOM_STATUS_SUCCESS)
	{
		return (1); /* Fail */
	}

	while (timeout_cnt < timeout_delay)
	{
		if (USBD_VCOM_IsEnumDone())
		{
			return (0); /* Success */
		}

		timeout_cnt++;
	}

	return (1); /* Fail */
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */

