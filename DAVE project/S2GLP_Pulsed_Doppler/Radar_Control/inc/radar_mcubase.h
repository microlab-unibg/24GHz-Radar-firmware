/**
    @file: radar_mcubase.h
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

#ifndef RADAR_MCUBASE_H_
#define RADAR_MCUBASE_H_

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

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

para_consumption_def_t *radar_get_consumption_labels(void);

void radar_set_consumption_labels(para_consumption_def_t *p_consumption_def_list);

size_t get_consumption_repeat_count(void);

void set_consumption_repeat_count(size_t count);

void inc_consumption_repeat_count(size_t count);

void mcu_base_updates(void);


/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RADAR_MCUBASE_H_ */

/* --- End of File -------------------------------------------------------- */
