/**
    @file: stream_interface_usb.c

  	@brief: This module implements the communication interface functions needed
  	for the communication protocol from the header \ref StreamInterface.h. 
  	Data is sent received through a virtual serial interface over a USB CDC Device.
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

#include "StreamInterface.h"
#include "USBD/usbd.h"
#include "DAVE.h"

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
   
   void com_init(void)
   {
     /* Don't do anything here as the init must have been done already when calling dave_init */
   }

//============================================================================

void com_send_data(const void* pData, uint16_t uNumBytes)
{
  USBD_VCOM_SendData(pData,uNumBytes);
}

//============================================================================

uint16_t com_get_data(void* pData, uint16_t uNumBytes)
{
  uint16_t bytes = 0;
  
  bytes = USBD_VCOM_BytesReceived();
  
  bytes = (bytes > uNumBytes) ? uNumBytes : bytes;
  
  if (bytes)
  {
    USBD_VCOM_ReceiveData((int8_t*)pData, bytes);
  }
  
  return (bytes);
}

//============================================================================

void com_flush(void)
{
  CDC_Device_USBTask(&USBD_VCOM_cdc_interface);	/* Do nothing as USBD VCOM already flushes the data that is sent */
}

/*
==============================================================================
  7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
