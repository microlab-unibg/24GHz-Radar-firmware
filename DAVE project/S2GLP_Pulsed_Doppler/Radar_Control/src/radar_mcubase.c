/**
    @file  radar_mcubase.c
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

#include "EndpointRadarMcuBase.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
 */

#define NB_OF_ELEMENTS(a)  (sizeof(a)/sizeof(a[0]))

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

/* set application parameter */
char *consumption_labels[] = { "Current Consumption" };
para_consumption_def_t consumption_defs = { .p_consumption_labels = consumption_labels, .num_labels = NB_OF_ELEMENTS(consumption_labels)};
para_consumption_def_t *p_consumption_def = &consumption_defs;

static size_t consumption_repeat_count = 0;

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

para_consumption_def_t *radar_get_consumption_labels(void)
{
	return p_consumption_def;
}

//============================================================================

/* so far we only have on consumption "sensor" */
uint16_t radar_send_consumption_to_host(int8_t index, float value, char *p_unit)
{
	consumption_response_t consumption;
	consumption.index = index;
	consumption.value = value;
	consumption.p_unit = p_unit;

	return send_consumption_response(1, &consumption);
}

//============================================================================

void set_consumption_repeat_count(size_t count)
{
	consumption_repeat_count = count;
}

//============================================================================

void inc_consumption_repeat_count(size_t count)
{
	consumption_repeat_count += count;
}

//============================================================================

void mcu_base_updates(void)
{
	if (consumption_repeat_count > 0)
	{
		device_settings_t *pDev = ds_device_get_settings();
		extern float calcCurrentConsumption_mA(device_settings_t *pDev);
		float consumption = calcCurrentConsumption_mA(pDev);

		radar_send_consumption_to_host(0, consumption, "mA");
		consumption_repeat_count -= 1;
	}
}

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
