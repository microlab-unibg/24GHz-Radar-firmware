/**
 * @file usbd_vcom.c
 * @date 2015-10-08
 * NOTE:
 * This file is generated by DAVE. Any manual modification done to this file will be lost when the code is regenerated.
 *
 * @cond
 ***********************************************************************************************************************
 * USBD_VCOM v4.0.10 - The USB virtual COM port driver for XMC4000 family of controllers.
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the           
 * following conditions are met:   
 *                                                                              
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following   
 *   disclaimer.                        
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the    
 *   following disclaimer in the documentation and/or other materials provided with the distribution.     
 *                         
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 *   products derived from this software without specific prior written permission. 
 *                                             
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,  
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,  
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR        
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,      
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE   
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes       
 * with Infineon Technologies AG (dave@infineon.com).        
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-02-16:
 *     - Initial version.      
 * 2015-03-18:
 *     - Guidelines updated.
 * 2015-04-23:
 *     - Updated the review comments from AEs.
 * 2015-06-20:
 *     - Updated the file header.
 *     
 * @endcond 
 *
 */

/***********************************************************************************************************************
 * HEADER FILES                                                                                                      
 **********************************************************************************************************************/
#include "usbd_vcom.h"

/**********************************************************************************************************************
 * MACROS                                                                                                            
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * LOCAL DATA
 **********************************************************************************************************************/
 
 /***********************************************************************************************************************
 * GLOBAL DATA
 **********************************************************************************************************************/
/**
 * Global CDC class event flags.
 * These flags can be polled from top level application.
 **/
CDC_Class_Event_Flags_t cdc_event_flags;

/*
 * Static allocation of endpoint buffers
 **/
uint8_t notification_ep_buf[USB_FS_MAXPKT_SIZE];
uint8_t tx_ep_buf[USB_FS_MAXPKT_SIZE];
uint8_t rx_ep_buf[USB_FS_MAXPKT_SIZE];

/**
 * @ingroup USBD_VCOM_datastructures
 * @{
 */

/**
 * CDC Class driver interface configuration and state information. This
 *  structure is passed to all CDC Class driver functions, so that multiple
 *  instances of the same class within a device can be differentiated from 
 *  one another.
 */
 USB_ClassInfo_CDC_Device_t USBD_VCOM_cdc_interface;
 
/**
 * @}
 */

/**
 * @ingroup USBD_VCOM_datastructures
 * @{
 */

/**
 * USBD_VCOM events.Define the event callbacks which are really required.
 * All other unwanted events can be made 0.
 * To add any of the USB event:\n
 * 1) In the event callback structure add the required event callback handler name.\n
 * 2) Define the callback handler in usbd_vcom.c file.\n
 * 3) Declare the callback handler in usbd_vcom.h file.\n
 *
 * The following events are supported in USBD_VCOM APP:\n
 * USB connect:\n
 * Event for USB device connection. This event fires when the microcontroller is in USB device mode and the device is
 * connected to a USB host, beginning the enumeration process measured by a rising level on the microcontroller's VBUS sense pin.
 * This event is time-critical; exceeding OS-specific delays within this event handler (typically of around two seconds) will
 * prevent the device from enumerating correctly.\n\n
 *
 * USB Disconnect:\n
 * Event for USB device disconnection. This event fires when the microcontroller is in USB device mode and the device is
 * disconnected from a host, measured by a falling level on the microcontroller's VBUS sense pin.\n\n
 *
 * Configuration Changed:\n
 * Event for USB configuration number changed. This event fires when a the USB host changes the selected configuration number.
 * This event should be hooked in device applications to create the endpoints and configure the device for the selected configuration.
 * This event is time-critical; exceeding OS-specific delays within this event handler (typically of around one second) will prevent
 * the device from enumerating correctly.\n\n
 *
 * Control Request:\n
 * Event for control requests. This event fires when a the USB host issues a control request to the mandatory device control
 * endpoint (of address 0). This may either be a standard request that the library may have a handler code for internally,
 * or a class specific request issued to the device which must be handled appropriately. If a request is not processed in the
 * user application via this event, it will be passed to the library for processing internally if a suitable handler exists.\n
 *
 * This event is time-critical; each packet within the request transaction must be acknowledged or sent within 50ms or the host
 * will abort the transfer.\n
 *
 * The library internally handles all standard control requests with the exceptions of SYNC FRAME,  SET DESCRIPTOR and
 * SET INTERFACE. These and all other non-standard control requests will be left for the user to process via this event if desired.
 * If not handled in the user application or by the library internally, unknown requests are automatically STALLed.\n\n
 *
 * Set Address:\n
 * USB set address event. This event gets fired when USB host requests the device to set the address.\n\n
 *
 * Get Descriptor:\n
 * USB Get descriptor event handler. This function is called by the library when in device mode. When the device receives a
 * Get Descriptor request on the control endpoint, this function is called so that the descriptor details can be passed back
 * and the appropriate descriptor sent back to the USB host.
 * The user has to send proper descriptor(device,configuration and string) and should return the size of the descriptor data.\n\n
 *
 * Wakeup Event:\n
 * Event for USB wake up. This event fires when a the USB interface is suspended while in device mode, and the host wakes up the
 * device by supplying Start Of Frame pulses. This is generally hooked to pull the user application out of a low power state and
 * back into normal operating mode.\n\n
 *
 * Suspend Event:\n
 * Event for USB suspend. This event fires when a the USB host suspends the device by halting its transmission of Start Of Frame
 * pulses to the device. This is generally hooked in order to move the device over to a low power state until the host wakes up the
 * device.\n\n
 *
 * SOF(Start Of Frame) Event:\n
 * Event for USB Start Of Frame detection, when enabled. This event fires at the start of each USB frame, once per millisecond, and
 * is synchronized to the USB bus. This can be used as an accurate millisecond timer source when the USB bus is enumerated in
 * device mode. This event is time-critical; it is run once per millisecond and thus will significantly degrade device performance.
 * This event should only be enabled when needed to reduce device wake-ups.\n\n
 *
 * Reset event:\n
 * Event for USB interface reset. This event fires when the USB interface is in device mode, and  the USB host requests that the
 * device reset its interface.
 * This event is time-critical; exceeding OS-specific delays within this event handler (typically of around  two seconds)
 * will prevent the device from enumerating correctly.
 *
 **/
USBD_Event_CB_t usbd_vcom_events = 
{
  0,/**< connect event*/
  0,/**< disconnect event*/
  USBD_VCOM_Event_ConfigurationChanged,/**< configuration changed event*/
  USBD_VCOM_Event_ControlRequest,/**< control request event*/
  0,/**< Set Address event*/
  USBD_VCOM_Event_GetDescriptor,/**< Get descriptor event*/
  0,/**< wake up event*/
  0,/**< suspend event*/
  0,/**< start of frame event*/
  USBD_VCOM_Event_Reset/**< USB reset event*/
};
/**
 * @}
 */

/**
 * @ingroup USBD_VCOM_datastructures
 * @{
 */
/**
 * USB CDC class event call backs.
 * Define the event call backs if required.
 * Unwanted events can be made 0.
 **/
USB_CDCClass_event_CB_t cdc_class_events = 
{
  /**< CDC class line encoding changed event handler.*/
  USBD_VCOM_CDCClass_Event_LineEncodingChanged,

  /**< CDC class control line state changed event handler.*/
  USBD_VCOM_CDCClass_Event_ControlLineStateChanged,

  /**< CDC class break sent event handler.*/
  USBD_VCOM_CDCClass_Event_BreakSent
};

/**
 * @}
 */
 
 /***********************************************************************************************************************
 * LOCAL ROUTINES 
 **********************************************************************************************************************/


/**********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/
/*
 * API to retrieve the version of the USBD_VCOM
 */
DAVE_APP_VERSION_t USBD_VCOM_GetAppVersion(void)
{
  DAVE_APP_VERSION_t version;

  version.major = USBD_VCOM_MAJOR_VERSION;
  version.minor = USBD_VCOM_MINOR_VERSION;
  version.patch = USBD_VCOM_PATCH_VERSION;

  return version;
}


/*The function initializes the USB core layer and register USB event call backs. */
USBD_VCOM_STATUS_t USBD_VCOM_Init(USBD_VCOM_t *vcom_handle)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;

  XMC_ASSERT("USBD_VCOM_Init: vcom_handle not valid", (vcom_handle != NULL));

  /*Initialize CDC interface*/
  USBD_VCOM_cdc_interface.Config.ControlInterfaceNumber = 0U;
  USBD_VCOM_cdc_interface.Config.DataINEndpoint.Address = (ENDPOINT_DIR_IN | CDC_TX_EPNUM);
  USBD_VCOM_cdc_interface.Config.DataINEndpoint.Size = CDC_TXRX_EPSIZE;
  USBD_VCOM_cdc_interface.Config.DataINEndpoint.Type = (uint8_t)EP_TYPE_BULK;
  USBD_VCOM_cdc_interface.Config.DataINEndpoint.Banks = 0U;
  USBD_VCOM_cdc_interface.Config.DataOUTEndpoint.Address = CDC_RX_EPNUM;
  USBD_VCOM_cdc_interface.Config.DataOUTEndpoint.Size = CDC_TXRX_EPSIZE;
  USBD_VCOM_cdc_interface.Config.DataOUTEndpoint.Type = (uint8_t)EP_TYPE_BULK;
  USBD_VCOM_cdc_interface.Config.DataOUTEndpoint.Banks = 0U;
  USBD_VCOM_cdc_interface.Config.NotificationEndpoint.Address = (ENDPOINT_DIR_IN | CDC_NOTIFICATION_EPNUM);
  USBD_VCOM_cdc_interface.Config.NotificationEndpoint.Size = CDC_NOTIFICATION_EPSIZE;
  USBD_VCOM_cdc_interface.Config.NotificationEndpoint.Type = (uint8_t)EP_TYPE_INTERRUPT;
  USBD_VCOM_cdc_interface.Config.NotificationEndpoint.Banks = 0U;

  /*Register the required USB events*/
  if (USBD_VCOM_STATUS_SUCCESS == USBD_VCOM_Register_USBEvent_CB(&usbd_vcom_events))
  {
    if (USBD_VCOM_STATUS_SUCCESS != (USBD_VCOM_STATUS_t)USBD_Init(USBD_handle))
    {
      status = USBD_VCOM_STATUS_FAILURE;
    }
  }
  else
  {
    status = USBD_VCOM_STATUS_FAILURE;
  }
  /*Remove the compilation warning*/
  (void)vcom_handle;

  return status;
}

/*The function check if the enumeration has been done already. */
uint32_t USBD_VCOM_IsEnumDone(void)
{
  return (uint32_t)(USBD_IsEnumDone() && (USB_DeviceState == (uint8_t)DEVICE_STATE_Configured));
}

/* This function sends a byte to the USB host. */
USBD_VCOM_STATUS_t USBD_VCOM_SendByte(const uint8_t data_byte)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;

  /* Send a byte to the host. */
  if ((uint8_t)ENDPOINT_RWSTREAM_NoError != CDC_Device_SendByte(&USBD_VCOM_cdc_interface, data_byte))
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }
  if ((uint8_t)ENDPOINT_READYWAIT_NoError != CDC_Device_Flush(&USBD_VCOM_cdc_interface))
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }

  return status;
}

/* This function sends multiple bytes to the USB host. */
USBD_VCOM_STATUS_t USBD_VCOM_SendData(const int8_t* const data_buffer, const uint16_t length)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;

  if ((length == 0U) || (data_buffer == NULL))
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;		  
  }
  else
  {
    /* Send data to USB host.*/
    if ((uint8_t)ENDPOINT_RWSTREAM_NoError !=
        CDC_Device_SendData(&USBD_VCOM_cdc_interface, (const int8_t *)data_buffer, length))
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
    if ((uint8_t)ENDPOINT_READYWAIT_NoError != CDC_Device_Flush(&USBD_VCOM_cdc_interface))
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
  }
  return status;
}

/* This function sends string data to the USB host. */
USBD_VCOM_STATUS_t USBD_VCOM_SendString(const int8_t* const data_string)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;

  if (data_string != NULL)
  {
    /* Send string to the host */
    if ((uint8_t)ENDPOINT_RWSTREAM_NoError != CDC_Device_SendString(&USBD_VCOM_cdc_interface, (const char*)data_string))
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
    if ((uint8_t)ENDPOINT_READYWAIT_NoError != CDC_Device_Flush(&USBD_VCOM_cdc_interface))
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
  }
  else
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }

  return status;
}

/* This function receives a byte from the USB host.*/
USBD_VCOM_STATUS_t USBD_VCOM_ReceiveByte(int8_t* data_byte)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;
  int16_t rx_byte;

  if (data_byte != NULL)
  {
    /* Receive one byte of data */
    rx_byte = CDC_Device_ReceiveByte(&USBD_VCOM_cdc_interface);

    if (rx_byte != -1)
    {
      *data_byte = (int8_t)rx_byte;
    }
    else
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
  }
  else
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }

  return status;
}

/* This function receives number of bytes from the USB host.*/
USBD_VCOM_STATUS_t USBD_VCOM_ReceiveData(int8_t* data_buffer, uint16_t data_byte)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;
  uint16_t bytes_processed = 0U;
  uint8_t ret = 0U;

  if ((data_byte > 0U) && (data_buffer != NULL))
  {
    /* Fix for new read/write */
    Endpoint_SelectEndpoint(CDC_RX_EPNUM);
    while ((uint8_t)ENDPOINT_RWSTREAM_IncompleteTransfer == 
    (ret = Endpoint_Read_Stream_LE(data_buffer, data_byte,	&bytes_processed)))
    {      
    }      
    if ((uint8_t)ENDPOINT_RWSTREAM_NoError != ret )
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
  }
  else
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }

  return status;
}

/* Gets the line encoding */
USBD_VCOM_STATUS_t USBD_VCOM_GetLineEncoding(CDC_LineEncoding_t* line_encoding_ptr)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;	

  if (line_encoding_ptr != NULL)
  {
    if ((USB_DeviceState != (uint8_t)DEVICE_STATE_Configured) ||
      (!(USBD_VCOM_cdc_interface.State.LineEncoding.BaudRateBPS)))
    {
      status = USBD_VCOM_STATUS_CDC_ERROR;
    }
    else
    {
      memcpy(line_encoding_ptr, &USBD_VCOM_cdc_interface.State.LineEncoding,
                sizeof(CDC_LineEncoding_t));
    }
  }
  else
  {
    status = USBD_VCOM_STATUS_CDC_ERROR;
  }
  return status;
}

/* This function checks number of bytes received from the USB host. */
uint16_t USBD_VCOM_BytesReceived(void)
{
  uint16_t bytes = 0U;

  /* Check if bytes are received from host */
  NVIC_DisableIRQ(USB0_0_IRQn);
  bytes = CDC_Device_BytesReceived(&USBD_VCOM_cdc_interface);
  NVIC_EnableIRQ(USB0_0_IRQn);	
    
  return bytes;
}

/**
 * Register the USB event call backs.
 * Call this API before calling USBD_Init
 **/
USBD_VCOM_STATUS_t USBD_VCOM_Register_USBEvent_CB(USBD_Event_CB_t *event_callback)
{
  USBD_VCOM_STATUS_t status = USBD_VCOM_STATUS_SUCCESS;

  /*Register USB event call backs*/
  if (event_callback != NULL)
  {
    /*NULL checks for the event callbacks are done in the low level APP USBD*/
    USBD_handle->event_cb->set_address = event_callback->set_address;
    USBD_handle->event_cb->config_changed = event_callback->config_changed;
    USBD_handle->event_cb->connect = event_callback->connect;
    USBD_handle->event_cb->control_request = event_callback->control_request;
    USBD_handle->event_cb->disconnect = event_callback->disconnect;
    USBD_handle->event_cb->get_descriptor = event_callback->get_descriptor;
    USBD_handle->event_cb->suspend = event_callback->suspend;
    USBD_handle->event_cb->wakeup = event_callback->wakeup;
    USBD_handle->event_cb->start_of_frame = event_callback->start_of_frame;
    USBD_handle->event_cb->reset = event_callback->reset;
  }
  else
  {
    status = USBD_VCOM_STATUS_FAILURE;
  }

  return status;

}

/* Event handler for the USB Configuration Changed event. */
void USBD_VCOM_Event_ConfigurationChanged(void)
{
  USBD_SetEndpointBuffer(((uint8_t)CDC_IN_EP_MASK|(uint8_t)CDC_NOTIFICATION_EPNUM), notification_ep_buf, 
  (uint16_t)USB_FS_MAXPKT_SIZE);
  USBD_SetEndpointBuffer(((uint8_t)CDC_IN_EP_MASK|(uint8_t)CDC_TX_EPNUM), tx_ep_buf, (uint16_t)USB_FS_MAXPKT_SIZE);
  USBD_SetEndpointBuffer((CDC_RX_EPNUM), rx_ep_buf, (uint16_t)USB_FS_MAXPKT_SIZE);
  if (true == CDC_Device_ConfigureEndpoints(&USBD_VCOM_cdc_interface))
  {
    device.IsConfigured = 1U;
    USB_DeviceState = (uint8_t)DEVICE_STATE_Configured;
  }
}

/* Event handler for the USB Control Request reception event. */
void USBD_VCOM_Event_ControlRequest(void)
{
  CDC_Device_ProcessControlRequest(&USBD_VCOM_cdc_interface);
}

/* Event handler for the USB reset event. */
void USBD_VCOM_Event_Reset(void)
{
  /*For self powered devices, reinitialize USB*/
  if (device.IsConfigured)
  {
    device.IsConfigured = 0U;
    if (USBD_VCOM_STATUS_SUCCESS == USBD_VCOM_Init(USBD_VCOM_handle))
    {
      if (USBD_VCOM_STATUS_SUCCESS != USBD_VCOM_Connect())
      {
      }
    }
  }
}

/*CDC class driver event for a line encoding change on a CDC interface. This event fires each time the host
 * requests a line encoding change (containing the serial parity, baud and other configuration information). The new
 * line encoding settings are available in the LineEncoding structure inside the CDC interface structure passed
 * as a parameter.*/
void USBD_VCOM_CDCClass_Event_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const cdc_interface_info)
{
  /*set here the global volatile flag(which can be polled from the top level application) to indicate
   * the event. */
  cdc_event_flags.line_encoding_event_flag = 1U;
  (void)cdc_interface_info;

}

/*CDC class driver event for a control line state change on a CDC interface. This event fires
 * each time the host requests a control line state change (containing the virtual serial control
 * line states, such as DTR).The new control line states are available in the ControlLineStates.HostToDevice
 * value inside the CDC interface structure passed as a parameter. */
void USBD_VCOM_CDCClass_Event_ControlLineStateChanged(USB_ClassInfo_CDC_Device_t* const cdc_interface_info)
{
  /*set here the global volatile flag(which can be polled from the top level application) to indicate the event */
  cdc_event_flags.control_line_state_change_event_flag = 1U;
  (void)cdc_interface_info;

}

/*CDC class driver event for a send break request sent to the device from the host.
 * This is generally used to separate data or to indicate a special condition to the receiving device.*/
void USBD_VCOM_CDCClass_Event_BreakSent(USB_ClassInfo_CDC_Device_t* const cdc_interface_info,	const uint8_t duration)
{
  /*set here the global volatile flag(which can be polled from the top level application) to indicate the event */
  cdc_event_flags.break_event_flag = 1U;
  (void)cdc_interface_info;
  (void)duration;

}

/*The API to connect the VCOM device to USB host*/
USBD_VCOM_STATUS_t USBD_VCOM_Connect(void)
{
  return (USBD_VCOM_STATUS_t)USBD_Connect();
}

/*The API to disconnect the VCOM device from USB host*/
USBD_VCOM_STATUS_t USBD_VCOM_Disconnect(void)
{
  return (USBD_VCOM_STATUS_t)USBD_Disconnect();
}
