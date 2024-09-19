/**
    @file: algo_version.h

    @brief: This file contains the Algorithm current version.
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

#ifndef ALGO_VERSION_H_
#define ALGO_VERSION_H_

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

#define ALGO_VERSION_MAJOR  		(2U) /**< Algorithm (Doppler & FMCW) major version */
#define ALGO_VERSION_MINOR  		(0U) /**< Algorithm (Doppler & FMCW) minor version */
#define ALGO_VERSION_REVISION 		(0U) /**< Algorithm (Doppler & FMCW) patch version */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ALGO_VERSION_H_ */

/* --- End of File -------------------------------------------------------- */
