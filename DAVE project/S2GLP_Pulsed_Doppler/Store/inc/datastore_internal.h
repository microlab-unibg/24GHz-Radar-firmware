/**
    @file  datastore_internal.h
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

#ifndef DATASTORE_INTERNAL_H_
#define DATASTORE_INTERNAL_H_

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
   3. EXPORTED DATA PROTOTYPES
==============================================================================
 */

const algo_settings_t ds_default_algo_settings;

const device_settings_t ds_default_device_settings;

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DATASTORE_H_ */

/* --- End of File -------------------------------------------------------- */
