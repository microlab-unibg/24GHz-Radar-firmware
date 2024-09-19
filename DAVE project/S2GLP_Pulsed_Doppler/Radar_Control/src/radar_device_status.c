/**
    @file: radar_device_status.c
 */

/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
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

#include "radar_device_status.h"

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

static Device_Status_t device_status;

typedef struct {
	Device_Error_t code;
	char *		   pText;
	uint32_t       red_led_toggle_time_msec;
} device_error_desc_t;

device_error_desc_t DEVICE_ERROR_DESCRIPTION_TABLE[] =
{
	{ UNKNOWN, "UNKNOWN ERROR", 200 },  /* Unknown error must be first entry! */
	{ FATAL_ERROR_BOARD_NOT_SUPPORTED, 	"ERROR: - Board unsupported", 100 },
	{ FATAL_ERROR_INVALID_RF_SHIELD, 	"ERROR: - Invalid Board ID, or Board not connected", 400 },
	{ FATAL_ERROR_BOARD_NOT_PROGRAMMED, "ERROR: - Board ID not programmed", 200 },

};

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

Device_Status_t* device_status_get_handle()
{
	return &device_status;
}

//============================================================================

void device_status_get_state(Device_State_t *mcu_state)
{
	*mcu_state = device_status.state;
}

//============================================================================

/**
 * @brief function to set specific error
 * @param radar_error
 */
/*Error prioritized, keep either first or last error*/
void device_status_set_error(Device_Error_t radar_error)
{
	uint8_t new_error_type =  FIND_TYPE_OF_ERROR(radar_error);
	uint8_t latest_error_type = FIND_TYPE_OF_ERROR(device_status.latest_error);

	/* only one message is stored: guard to not overwrite for example error through a status*/
	if (latest_error_type < new_error_type)
	{
		device_status.latest_error = radar_error;
	}

	switch(new_error_type)
	{
	case STATE_INFO:
		device_status.state = STATE_INFO;
		device_status.info_count++;
		break;

	case STATE_WARNING:
		device_status.state = STATE_WARNING;
		device_status.warning_count++;
		break;

	case STATE_ERROR:
		device_status.state = STATE_ERROR;
		device_status.error_count++;
		break;

	case STATE_FATAL_ERROR:
		device_status.state = STATE_FATAL_ERROR;
		device_status.fatal_error_count++;
		break;

	default:
		break;
	}
}

//============================================================================

/**
 * @brief function to get the corresponding description string of the error
 * @param error is the device error enum type
 * @return const char * to error description
 */
const char* device_status_get_error_description(Device_Error_t error)
{
	/*
	 * in case of an error, we do not need to be efficient!
	 */
	uint32_t elems = sizeof(DEVICE_ERROR_DESCRIPTION_TABLE)/sizeof(DEVICE_ERROR_DESCRIPTION_TABLE[0]);
	for (uint32_t i = 0; i< elems; i++)
	{
		if (error == DEVICE_ERROR_DESCRIPTION_TABLE[i].code)
		{
			return DEVICE_ERROR_DESCRIPTION_TABLE[i].pText;
		}
	}
	/* did not found an entry, return text with index 0! */
	return DEVICE_ERROR_DESCRIPTION_TABLE[0].pText;
}

//============================================================================

uint32_t device_status_get_toggle_time_msec(Device_Error_t error)
{
	/*
	 * in case of an error, we do not need to be efficient!
	 */
	uint32_t elems = sizeof(DEVICE_ERROR_DESCRIPTION_TABLE)/sizeof(DEVICE_ERROR_DESCRIPTION_TABLE[0]);
	for (uint32_t i = 0; i< elems; i++)
	{
		if (error == DEVICE_ERROR_DESCRIPTION_TABLE[i].code)
		{
			return DEVICE_ERROR_DESCRIPTION_TABLE[i].red_led_toggle_time_msec;
		}
	}
	/* did not found an entry, return text with index 0! */
	return DEVICE_ERROR_DESCRIPTION_TABLE[0].red_led_toggle_time_msec;
}

//============================================================================

/**
 * @brief function to check device status
 * 
 * @return -1 if error exists else 0
 */
uint32_t device_status_check()
{
	uint32_t retval = 0;

	if (device_status.state != STATE_NO_ERROR)
	{
		retval = -1;
	}
	return retval;
}

//============================================================================

/**
 * @brief function to get the current device error
 * 
 * @param radar_error
 */
void device_status_get_error(Device_Error_t *radar_error)
{
	*radar_error = device_status.latest_error;
}

//============================================================================

/**
 * @brief function to clean the errors of the device status
 */
void device_status_clean()
{
	device_status.latest_error = 0x0000;
	device_status.state = STATE_NO_ERROR;
	//TODO: decrease or reset the current error counter?
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
