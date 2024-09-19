/**
    @file: radar_device_status.h
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

#ifndef RADAR_DEVICE_STATUS_H_
#define RADAR_DEVICE_STATUS_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/*
 * Error Code: 				4 Byte
							b0000 00000 0000 00000
 */
// Status Type:				4 BIT
#define NO_ERROR			0x0000
#define INFO				0x1000	/* ex. updating device setings completed*/
#define WARNING				0x2000	/* ex. updating device setting failed, old settings used */
#define	ERROR				0x3000	/* ex. fatal error, SAMPLE_PER_CHIRP value too high */
#define FATAL_ERROR			0x4000	/* ex. fatal error, SAMPLE_PER_CHIRP value too high */

// Status Source:			4 BIT = 16 Source
#define SOURCE_0			0x0000
#define SOURCE_1			0x0100
#define SOURCE_2			0x0200
#define SOURCE_3			0x0300
#define SOURCE_4			0x0400
#define SOURCE_5			0x0500
#define SOURCE_6			0x0600
#define SOURCE_7			0x0700
#define SOURCE_8			0x0800
#define SOURCE_9			0x0900
#define SOURCE_10			0x0A00
#define SOURCE_11			0x0B00
#define SOURCE_12			0x0C00
#define SOURCE_13			0x0D00
#define SOURCE_14			0x0E00
#define SOURCE_15			0x0F00


#define FIND_TYPE_OF_ERROR(error)   ((((error) &  0xF000) >> 12))
#define FIND_SOURCE_OF_ERROR(error) ((((error) &  0x0F00) >> 8))
#define FIND_NUMER_OF_ERROR(error)  (((error)  &  0x00FF))

/*
==============================================================================
   3. TYPES
==============================================================================
 */

typedef enum
{
	INFO_SOURCE_1_DEF_0              = INFO + SOURCE_1 + 0,
	WARNING_SOURCE_1_DEF_0           = WARNING + SOURCE_1 + 0,
	ERROR_SOURCE_1_DEF_0             = ERROR + SOURCE_1 + 0,
	ERROR_SOURCE_1_DEF_1             = ERROR + SOURCE_1 + 1,
	FATAL_ERROR_BOARD_NOT_SUPPORTED  = FATAL_ERROR + SOURCE_1 + 0,
	FATAL_ERROR_INVALID_RF_SHIELD    = FATAL_ERROR + SOURCE_1 + 1,
	FATAL_ERROR_BOARD_NOT_PROGRAMMED = FATAL_ERROR + SOURCE_1 + 2,
	UNKNOWN = 0xFFFFFFFF,
} Device_Error_t;

typedef enum
{
	STATE_NO_ERROR    = 0,
	STATE_INFO        = 1,
	STATE_WARNING     = 2,
	STATE_ERROR       = 3,
	STATE_FATAL_ERROR = 4
} Device_State_t;

typedef struct
{
	Device_State_t state;
	Device_Error_t latest_error;
	uint8_t info_count;
	uint8_t warning_count;
	uint8_t error_count;
	uint8_t fatal_error_count;
} Device_Status_t;

/*
==============================================================================
   4. DATA
==============================================================================
 */

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

Device_Status_t* device_status_get_handle();

void device_status_get_state(Device_State_t *mcu_state);

void device_status_set_error(Device_Error_t radar_error); /* Error prioritized, keep either first or last error */

const char* device_status_get_error_description(Device_Error_t error);

uint32_t device_status_get_toggle_time_msec(Device_Error_t error);

uint32_t device_status_check();

void device_status_get_error(Device_Error_t *radar_error);

void device_status_clean();

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* RADAR_DEVICE_STATUS_H_ */

/* --- End of File -------------------------------------------------------- */

