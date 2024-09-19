/**
    @file  datastore_eep.c
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

#include "datastore.h"
#include "datastore_eep.h"

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

#ifdef DS_USE_EEPROM

/* Function to write settings into EEPROM */
EEPROM_STATUS_t ds_eeprom_write_settings(const void *p_settings, uint32_t struct_size, uint16_t eeprom_address)
{
  uint32_t buffer[struct_size/4 + 2]; /* Plus one extra for the CRC32 */
  uint32_t buffer_size = 0, crc32 = 0;

  if ((p_settings == NULL) || (struct_size <= 0))
  {
    /* Invalid input parameters */
	return EEPROM_STATUS_FAILURE;
  }

  /* Get the buffer size for the structure */
  if ((struct_size % 4) != 0)
  {
    buffer_size = struct_size/4 + 1;
  }
  else
  {
	buffer_size = struct_size/4;
  }

  /* Clear the transmit buffer */
  memset(buffer, 0, sizeof(buffer));

  /* Copy hardware settings structure into transmit buffer */
  memcpy(buffer, p_settings, struct_size);

  /*  Get the CRC for all structure */
  CRC_SW_CalculateCRC(&CRC_SW_EEPROM, buffer, buffer_size);
  crc32 = CRC_SW_GetCRCResult(&CRC_SW_EEPROM);
  buffer[buffer_size++] = crc32;

  /* Write hardware settings structure in the EEPROM */
  return(bsp_eeprom_24cw128x_write_data(eeprom_address, (uint8_t*)buffer, buffer_size*4));
}

//============================================================================

/* Function to read settings from EEPROM */
EEPROM_STATUS_t ds_eeprom_read_settings(void *p_settings, uint32_t struct_size, uint16_t eeprom_address)
{
  uint32_t buffer[struct_size/4 + 2]; /* Plus one extra for the CRC32 */
  uint32_t buffer_size = 0, new_crc32 = 0, old_crc32 = 0;
  EEPROM_STATUS_t ret_status = EEPROM_STATUS_FAILURE;

  if ((p_settings == NULL) || (struct_size <= 0))
  {
    /* Invalid input parameters */
	return EEPROM_STATUS_FAILURE;
  }

  /* Get the buffer size for the structure */
  if ((struct_size % 4) != 0)
  {
    buffer_size = struct_size/4 + 1;
  }
  else
  {
	buffer_size = struct_size/4;
  }

  /* Clear the receive buffer */
  memset(buffer, 0, sizeof(buffer));

  /* Read hardware settings structure from EEPROM */
  ret_status = bsp_eeprom_24cw128x_read_data(eeprom_address, (uint8_t*)buffer, (buffer_size + 1)*4);

  /* Get the old CRC and compare it with the new one */
  old_crc32 = buffer[buffer_size];
  CRC_SW_CalculateCRC(&CRC_SW_EEPROM, buffer, buffer_size);
  new_crc32 = CRC_SW_GetCRCResult(&CRC_SW_EEPROM);

  /* Check for CRC error */
  if ((new_crc32 == old_crc32) && (ret_status == EEPROM_STATUS_SUCCESS))
  {
    /* Valid CRC: copy buffer into the HW settings structure */
	memcpy(p_settings, buffer, struct_size);

    return EEPROM_STATUS_SUCCESS;
  }
  else
  {
	return EEPROM_STATUS_FAILURE;
  }
}

//============================================================================

/* Function to read board ID from EEPROM
   !!! Only 32 bytes are reserved for the RF board ID */
RF_SHIELD_STATUS_t ds_eeprom_read_board_id(char *board_id, uint8_t id_size)
{
  uint16_t eeprom_address = DS_EEPROM_ADDRESS_BOARD_ID;

  uint8_t buffer[32], index = 0;
  char *id = board_id;
  RF_SHIELD_STATUS_t shield_status = RF_SHIELD_BOARD_NOT_CONNECTED;

  if ((board_id == NULL) || (id_size > 32) || (id_size == 0))
  {
	return RF_SHIELD_BOARD_INVALID_PARAM;
  }

  /* Clear the receive buffer */
  memset(buffer, 0, sizeof(buffer));

  /*  Read RF board ID from EEPROM and check if a valid board ID is written in EEPROM */
  if (bsp_eeprom_24cw128x_read_data(eeprom_address, buffer, id_size) == EEPROM_STATUS_SUCCESS)
  {
    do
    {
      if ((index == id_size) && (*id == '\0'))
      {
        /* Valid board ID */
        shield_status = RF_SHIELD_BOARD_SUPPORTED;
        break;
      }
      else if (*id == buffer[index])
      {
        /* Go to the next char */
        id++;
        index++;
      }
      else  /* Invalid board ID */
      {
    	if (index >= 3)
    	{
          /* RF shield board is not compatible and is not supported in the current Firmware.
      	   Shield programmed with the wrong ID, For example RBBxxxxx */
          shield_status = RF_SHIELD_BOARD_UNSUPPORTED;
    	}
    	else
    	{
    	  /* Invalid RF shield board.
    	  RF shield is not programmed or it contains invalid data */
          shield_status = RF_SHIELD_BOARD_NOT_PROGRAMMED;
    	}
        break;
      }
    }while (index <= id_size);
  }
  else /* In case of I2C error */
  {
    shield_status = RF_SHIELD_BOARD_NOT_CONNECTED;
  }

  return shield_status;
}

//============================================================================

/* TODO: remove for release! */

uint8_t ds_eeprom_write_inital_board_id(void)
{
  uint8_t buffer[] = RF_BOARD_ID;

  /* Read RF board ID from EEPROM */
  return bsp_eeprom_24cw128x_write_data(DS_EEPROM_ADDRESS_BOARD_ID, buffer, sizeof(buffer));
}

/* compile time check, if device_settings_t struct is larger then the available space in the EEPROM */
STATIC_ASSERT((DS_EEPROM_ADDRESS_ALGO_SETTING - DS_EEPROM_ADDRESS_HW_SETTING) > sizeof(device_settings_t));
STATIC_ASSERT((DS_EEPROM_END_ADDRESS_SETTING - DS_EEPROM_ADDRESS_ALGO_SETTING)  > sizeof(algo_settings_t));

#endif /* DS_USE_EEPROM */

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
