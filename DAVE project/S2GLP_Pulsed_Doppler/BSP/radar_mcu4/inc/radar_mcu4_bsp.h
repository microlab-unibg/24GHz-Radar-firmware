/**
    @file: radar_mcu4_bsp.h

    @brief: This file contains constants declarations and functions prototypes for
            Radar_MCU4 board hardware resources.
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

#ifndef RADAR_MCU4_BSP_H_
#define RADAR_MCU4_BSP_H_

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

#include "DAVE.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
 */

#define EEPROM_24CW128X_I2C_ADDRESS           	0xA0       /* I2C Address 101 0000 plus one extra bit for write and read operation */
#define EEPROM_24CW128X_START_MEM_ADDR        	0          /* Start write/read address */
#define EEPROM_24CW128X_PAGE_SIZE             	32         /* Page size is 32 bytes */
#define EEPROM_24CW128X_MAX_SIZE             	0x4000     /* 16 * 1024 Max memory size */
#define EEPROM_24CW128X_MEM_ADDRESS_SIZE		2

/*
==============================================================================
   3. TYPES
==============================================================================
 */

/**
 * \brief Defines EEPROM read and write operations status.
 * @{
 */
typedef enum
{
  EEPROM_STATUS_SUCCESS      	= 1U,
  EEPROM_STATUS_FAILURE      	= 2U,
  EEPROM_STATUS_READ_ERROR   	= 3U,
  EEPROM_STATUS_WRITE_ERROR  	= 4U,
  EEPROM_STATUS_LENGTH_MISMATCH = 5U,
  EEPROM_STATUS_CRC_MISMATCH 	= 6U,
} EEPROM_STATUS_t;

/**
 * \brief Defines BSP TIMER delay unit in microseconds or milliseconds.
 * @{
 */
typedef enum
{
  BSP_TIMER_DELAY_MILLISECOND      = 1U,
  BSP_TIMER_DELAY_MICROSECOND      = 2U,
} BSP_TIMER_DELAY_t;


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

/**
 * \brief Function to write data into EEPROM in case of write operation in multi-page, within more then one page
 *        !!! For many write operations we have to wait at least 5ms between each operation
 *            This is needed only for write operation, there is no need to have this delay for the read operation
 *
 * \param[in]  mem_address   The write address in the EEPROM memory
 *
 * \param[in]  tx_data       Pointer to uint8_t buffer, the data buffer to be written in the EEPROM
 *
 * \param[in]  tx_size       The size of the buffer in term of byte to be transmitted
 *
 * \return  EEPROM_STATUS_t  Success or failure will be return
 */
EEPROM_STATUS_t bsp_eeprom_24cw128x_write_data(const uint16_t mem_address, uint8_t *tx_data, uint32_t tx_size);

/**
 * \brief Function to read data from EEPROM in case of read operation in multi-page, within more then one page
 *
 * \param[in]  mem_address   The read address in the EEPROM memory
 *
 * \param[in]  rx_data       Pointer to uint8_t buffer, the read data will be copied in this buffer
 *
 * \param[in]  rx_size       The size of the buffer in term of byte to be read
 *
 * \return  EEPROM_STATUS_t  Success or failure will be return
 */
EEPROM_STATUS_t bsp_eeprom_24cw128x_read_data(const uint16_t mem_address, uint8_t *rx_data, uint32_t rx_size);

/**
 * \brief Function to perform a software rest in case of I2C communication error
 */
EEPROM_STATUS_t bsp_eeprom_24cw128x_reset(void);

/**
 * \brief Function used to generate a time in units of microseconds or milliseconds
 *
 * \param[in]  delay_value   The delay value
 *
 * \param[in]  time_unit     The time unit in microseconds or milliseconds
 */
void bsp_time_delay(uint32_t delay_value, BSP_TIMER_DELAY_t time_unit);

#if !defined(BUILD_FOR_ARDUINO)
/**
 * \brief	This function turns On the green LED mounted on Radar_MCU4 board.
 */
void bsp_led_green_on(void);

/**
 * \brief	This function turns Off the green LED mounted on Radar_MCU4 board.
 */
void bsp_led_green_off(void);

/**
 * \brief This function toggles On/Off the green LED mounted on Radar_MCU4 board.
 */
void bsp_led_green_toggle(void);

/**
 * \brief	This function turns On the blue LED mounted on Radar_MCU4 board.
 */
void bsp_led_blue_on(void);

/**
 * \brief	This function turns Off the blue LED mounted on Radar_MCU4 board.
 */
void bsp_led_blue_off(void);

/**
 * \brief This function toggles On/Off the blue LED mounted on Radar_MCU4 board.
 */
void bsp_led_blue_toggle(void);

/**
 * \brief	This function turns On the red LED mounted on Radar_MCU4 board.
 */
void bsp_led_red_on(void);

/**
 * \brief	This function turns Off the red LED mounted on Radar_MCU4 board.
 */
void bsp_led_red_off(void);

/**
 * \brief This function toggles On/Off the red LED mounted on Radar_MCU4 board.
 */
void bsp_led_red_toggle(void);

#endif

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RADAR_MCU4_BSP_H_ */

/* --- End of File -------------------------------------------------------- */
