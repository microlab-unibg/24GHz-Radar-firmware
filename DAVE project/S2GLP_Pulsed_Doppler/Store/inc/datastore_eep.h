/**
    @file  datastore_eep.h
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

#ifndef DATASTORE_EEP_H_
#define DATASTORE_EEP_H_

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

#include "radar_mcu4_bsp.h" // for EEPROM_STATUS_t

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT(test_for_true) _Static_assert((test_for_true), "(" #test_for_true ") failed")
#endif

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define RF_BOARD_ID                     "RBBPDV30" /* The RF board ID, it used to identify the RF board and to run the FW accordingly \
                                                   !!!! The ID length must be less than 32 bytes including the end of string character */

#define DS_EEPROM_ADDRESS_BOARD_ID       0U        /* The first page of 32-Byte is reserved for the RF board ID in EEPROM \
                                                      !!! Only one page of 32 bytes is reserved for the RF board ID */

#define DS_EEPROM_ADDRESS_HW_SETTING      0x20U    /* Read/write Hardware settings starting from the second page in EEPROM \
                                                      !!! Only 3 pages of 32 bytes (96-Byte) are reserved for Hardware settings structure */

#define DS_EEPROM_ADDRESS_ALGO_SETTING    0xA0U    /* Read/write Algorithm settings starting from the fourth page in EEPROM \
                                                      !!! Only 2 pages of 32 bytes (64-Byte) are reserved for Algorithm settings structure */

#define DS_EEPROM_ADDRESS_DEFAULT_SETTING 0xE0U    /* Read/Write default settings check, only one page*/

#define DS_EEPROM_END_ADDRESS_SETTING     0x100U   /* End address of settings in EEPROM, defines can be changed if more space is needed */

/* EEPROM structure:
     _______________________________________________________________
    |                                                               |
    |           ___________________________                         |
    |      0x00 |                       |                           |
    |           |                       | => RF shield board ID     |
    |          _|_ _ _ _ _ _ _ _ _ _ _ _|__                         |
    |      0x20 |                       |                           |
    |           |                       |                           |
    |           |_ _ _ _ _ _ _ _ _ _ _ _|                           |
    |           |                       |                           |
    |           |                       | => Hardware settings      |
    |           |_ _ _ _ _ _ _ _ _ _ _ _|    structure              |
    |           |                       |                           |
    |           |                       |                           |
    |          _|_ _ _ _ _ _ _ _ _ _ _ _|__                         |
    |      0xA0 |                       |                           |
    |           |                       |                           |
    |           |_ _ _ _ _ _ _ _ _ _ _ _| => Algorithm settings     |
    |           |                       |    structure              |
    |           |                       |                           |
    |          _|_ _ _ _ _ _ _ _ _ _ _ _|__                         |
    |      0xE0 |                       |                           |
    |           |                       | => Default settings       |
    |          _|_ _ _ _ _ _ _ _ _ _ _ _|__  	check               |
    |     0x100 |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       | => User space             |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |                       |                           |
    |           |_______________________|__                         |
    |                                                               |
    |                                                               |
    | EEPROM structure                                              |
    |_______________________________________________________________|
 */

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/*
==============================================================================
   4. DATA
==============================================================================
 */

/**
 * \brief Defines RF shield board status.
 * @{
 */
typedef enum
{
   RF_SHIELD_BOARD_INVALID_PARAM  = 1U, /* Status in case of invalid parameter */
   RF_SHIELD_BOARD_SUPPORTED      = 2U, /* Status for supported RF shield with valid board ID */
   RF_SHIELD_BOARD_UNSUPPORTED    = 3U, /* Status for unsupported RF shield board
                                           RF shield board is not compatible and is not supported in the current Firmware
                                           Shield programmed with the wrong ID, For example RBBxxxxxx */
   RF_SHIELD_BOARD_NOT_PROGRAMMED = 4U, /* Status for unsupported RF shield board
                                           RF shield is not programmed or it contains invalid data */
   RF_SHIELD_BOARD_NOT_CONNECTED  = 5U, /* Status when RF shield board is not plugged into */
} RF_SHIELD_STATUS_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
 */

/**
 * \brief Function to write hardware settings into EEPROM
 *
 * \param[in]  p_settings   Pointer to settings structure to be copied into EEPROM
 *
 * \param[in]  struct_size   The settings structure size. e.g sizeof(device_settings_t)
 *
 * \param[in]  eeprom_address  The write address in the EEPROM memory
 *
 * \return  EEPROM_STATUS_t  Success or failure will be return
 */
EEPROM_STATUS_t ds_eeprom_write_settings(const void *p_settings, uint32_t struct_size, uint16_t eeprom_address);

/**
 * \brief Function to read hardware settings from EEPROM
 *
 * \param[in]  p_settings   Pointer to settings structure to be read from EEPROM
 *
 * \param[in]  struct_size   The settings structure size. e.g sizeof(device_settings_t)
 *
 * \param[in]  eeprom_address  The read address in the EEPROM memory
 *
 * \return  EEPROM_STATUS_t  Success or failure will be return
 */
EEPROM_STATUS_t ds_eeprom_read_settings(void *p_settings, uint32_t struct_size, uint16_t eeprom_address);

/**
 * \brief Function to read board ID from EEPROM
 *        The first page of 32-Byte is reserved for the RF board ID in EEPROM
 *        !!! Only 32 bytes are reserved for the RF board ID
 *
 * \param[in]  board_id   Pointer to the current board ID string to be checked
 *                        !!! Compare this board ID string with what have been programmed in EEPROM
 *
 * \param[in]  id_size   Size of current board ID string
 *
 * \return  RF_SHIELD_STATUS_t The RF shield board status
 */
RF_SHIELD_STATUS_t ds_eeprom_read_board_id(char *board_id, uint8_t id_size);


uint8_t ds_eeprom_write_inital_board_id(void);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* DATASTORE_EEP_H_ */

/* --- End of File -------------------------------------------------------- */
