/**
    @file: radar_mcu4_bsp.c

    @brief: This file provides a set of firmware functions to manage all peripherals,
            LED, Button and current sensor available on Radar_MCU4 board from Infineon.
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

#include "radar_mcu4_bsp.h"

/*
==============================================================================
   4. DATA
==============================================================================
 */

/* SCL IO: In case of software reset this pin will be used to generate 9 clock cycles */
const DIGITAL_IO_t DIGITAL_IO_EEPROM_SW_RESET =
{
  .gpio_port = XMC_GPIO_PORT4,
  .gpio_pin = 2U,
  .gpio_config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,

  },
  .hwctrl = XMC_GPIO_HWCTRL_DISABLED
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

/* Function to write data into EEPROM in case of write operation in multi-page, within more then one page */
/* ==> For many write operations we have to wait at least 5ms between each operation
       This is needed only for write operation, there is no need to have this delay for the read operation */
EEPROM_STATUS_t bsp_eeprom_24cw128x_write_data(const uint16_t mem_address, uint8_t *tx_data, uint32_t tx_size)
{
  I2C_MASTER_STATUS_t status = I2C_MASTER_STATUS_FAILURE;
  uint8_t mem_add[2];
  uint32_t nb_of_start_page = 0;  /* The index of start page */
  uint32_t nb_of_end_page = 0;    /* The index of last page */
  uint32_t written_data = 0;
  uint32_t tmp_tx_size = tx_size;
  uint16_t tmp_mem_address = mem_address;

  /* Check for valid start address and valid data size */
  if ((tmp_mem_address >= EEPROM_24CW128X_START_MEM_ADDR) &&
	 ((tmp_mem_address + tmp_tx_size) <= EEPROM_24CW128X_MAX_SIZE) &&
	 (tmp_tx_size > 0))
  {
	/* The start write address is located in page : */
	nb_of_start_page = (tmp_mem_address / EEPROM_24CW128X_PAGE_SIZE) + 1;

	/* The last write address is located in page : */
	nb_of_end_page = ((tmp_mem_address + tmp_tx_size - 1) / EEPROM_24CW128X_PAGE_SIZE) + 1;

	/* Write operation in multi-page */
	if ((nb_of_end_page - nb_of_start_page) > 0)
	{
	  /* Write the first part of data */
	  written_data = (nb_of_start_page * EEPROM_24CW128X_PAGE_SIZE) - tmp_mem_address;

	  /* Write the first chunk of data */
	  if (written_data > 0)
	  {
	    /* EEPROM start write address */
        mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
        mem_add[1] = tmp_mem_address & 0x00FF;

        /* Send memory address */
        status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
        while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

        if (status == I2C_MASTER_STATUS_SUCCESS)
	    {
          /* Start sending data */
          status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, false, EEPROM_24CW128X_I2C_ADDRESS, tx_data, written_data, true);
          while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

		  /* Remaining data */
		  tmp_tx_size = tmp_tx_size - written_data;
		  tmp_mem_address = tmp_mem_address + written_data;

		  /* Wait for the EEPROM internal write cycle, minimum for 5ms */
          bsp_time_delay(6, BSP_TIMER_DELAY_MILLISECOND);
		}
	  }

	  /* Loop for the number of pages to be written */
	  do
	  {
		if (tmp_tx_size >= EEPROM_24CW128X_PAGE_SIZE) /* Write one page data */
		{
		  /* EEPROM start write address */
          mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
          mem_add[1] = tmp_mem_address & 0x00FF;

          /* Send memory address */
          status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
          while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

          if (status == I2C_MASTER_STATUS_SUCCESS)
	      {
            /* Start sending data */
            status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, false, EEPROM_24CW128X_I2C_ADDRESS, (tx_data + written_data), EEPROM_24CW128X_PAGE_SIZE, true);
            while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

		    /* Remaining data */
            tmp_mem_address = tmp_mem_address + EEPROM_24CW128X_PAGE_SIZE;
		    tmp_tx_size = tmp_tx_size - EEPROM_24CW128X_PAGE_SIZE;
			written_data = written_data + EEPROM_24CW128X_PAGE_SIZE;

			/* Wait for the EEPROM internal write cycle, minimum for 5ms */
			bsp_time_delay(6, BSP_TIMER_DELAY_MILLISECOND);
		  }
		}
		else if ((tmp_tx_size > 0) && (tmp_tx_size < EEPROM_24CW128X_PAGE_SIZE)) /* Write the last chunk of data */
		{
		  /* EEPROM start write address */
          mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
          mem_add[1] = tmp_mem_address & 0x00FF;

          /* Send memory address */
          status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
          while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

          if (status == I2C_MASTER_STATUS_SUCCESS)
	      {
            /* Start sending data */
            status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, false, EEPROM_24CW128X_I2C_ADDRESS, (void *)(tx_data + written_data), tmp_tx_size, true);
            while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

		    /* No remaining data */
		    tmp_tx_size = 0;

			/* Wait for the EEPROM internal write cycle, minimum for 5ms */
		    bsp_time_delay(6, BSP_TIMER_DELAY_MILLISECOND);
		  }
		}
	  }while(tmp_tx_size > 0);
	}
	else /* Write operation within the same page */
	{
	  /* EEPROM start write address */
      mem_add[0] = (mem_address & 0x3F00) >> 8;
      mem_add[1] = mem_address & 0x00FF;

      /* Send memory address */
      status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
      while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

      if (status == I2C_MASTER_STATUS_SUCCESS)
	  {
        /* Start sending data */
        status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, false, EEPROM_24CW128X_I2C_ADDRESS, (void*) tx_data, tx_size, true);
        while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));
      }

	  /* Wait for the EEPROM internal write cycle, minimum for 5ms */
      bsp_time_delay(6, BSP_TIMER_DELAY_MILLISECOND);
	}
  }

  if (status == I2C_MASTER_STATUS_SUCCESS)
  {
    return EEPROM_STATUS_SUCCESS;
  }
  else
  {
    return EEPROM_STATUS_WRITE_ERROR;
  }
}

//============================================================================

/* Function to read data from EEPROM in case of read operation in multi-page, within more then one page */
EEPROM_STATUS_t bsp_eeprom_24cw128x_read_data(const uint16_t mem_address, uint8_t *rx_data, uint32_t rx_size)
{
  I2C_MASTER_STATUS_t status = I2C_MASTER_STATUS_FAILURE;
  uint8_t mem_add[2];
  uint32_t nb_of_start_page = 0;  /* The index of start page */
  uint32_t nb_of_end_page = 0;    /* The index of last page */
  uint32_t read_data = 0;
  uint32_t tmp_rx_size = rx_size;
  uint16_t tmp_mem_address = mem_address;

  /* Check for valid start address and valid data size */
  if ((tmp_mem_address >= EEPROM_24CW128X_START_MEM_ADDR) &&
	 ((tmp_mem_address + tmp_rx_size) <= EEPROM_24CW128X_MAX_SIZE) &&
	 (tmp_rx_size > 0))
  {
	/* The start read address is located in page : */
	nb_of_start_page = (tmp_mem_address / EEPROM_24CW128X_PAGE_SIZE) + 1;

	/* The last read address is located in page : */
	nb_of_end_page = ((tmp_mem_address + tmp_rx_size - 1) / EEPROM_24CW128X_PAGE_SIZE) + 1;

	/* Read operation from multi-pages */
	if ((nb_of_end_page - nb_of_start_page) > 0)
	{
	  /* Read the first part of data */
		read_data = (nb_of_start_page * EEPROM_24CW128X_PAGE_SIZE) - tmp_mem_address;

	  /* Write the first chunk of data */
	  if (read_data > 0)
	  {
	    /* EEPROM start read address */
        mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
        mem_add[1] = tmp_mem_address & 0x00FF;

        /* Send memory address */
        status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
        while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

        if (status == I2C_MASTER_STATUS_SUCCESS)
	    {
          /* Start reading data */
          status = I2C_MASTER_Receive(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, rx_data, read_data, true, true);
          while(I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));

		  /* Remaining data */
          tmp_rx_size = tmp_rx_size - read_data;
          tmp_mem_address = tmp_mem_address + read_data;
		}
	  }

	  /* Loop for the number of pages to be read */
	  do
	  {
		if (tmp_rx_size >= EEPROM_24CW128X_PAGE_SIZE) /* read one page data */
		{
		  /* EEPROM start read address */
          mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
          mem_add[1] = tmp_mem_address & 0x00FF;

          /* Send memory address */
          status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
          while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

          if (status == I2C_MASTER_STATUS_SUCCESS)
	      {
            /* Start reading data */
            status = I2C_MASTER_Receive(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, (rx_data + read_data), EEPROM_24CW128X_PAGE_SIZE, true, true);
            while(I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));

		    /* Remaining data */
            tmp_mem_address = tmp_mem_address + EEPROM_24CW128X_PAGE_SIZE;
			tmp_rx_size = tmp_rx_size - EEPROM_24CW128X_PAGE_SIZE;
			read_data = read_data + EEPROM_24CW128X_PAGE_SIZE;
		  }
		}
		else if ((tmp_rx_size > 0) && (tmp_rx_size < EEPROM_24CW128X_PAGE_SIZE)) /* read the last chunk of data */
		{
		  /* EEPROM start read address */
          mem_add[0] = (tmp_mem_address & 0x3F00) >> 8;
          mem_add[1] = tmp_mem_address & 0x00FF;

          /* Send memory address */
          status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
          while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

          if (status == I2C_MASTER_STATUS_SUCCESS)
	      {
            /* Start reading data */
            status = I2C_MASTER_Receive(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, (rx_data + read_data), tmp_rx_size, true, true);
            while(I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));

		    /* No remaining data */
            tmp_rx_size = 0;
		  }
		}
	  }while(tmp_rx_size > 0);
	}
	else /* Read operation within the same page */
	{
	  /* EEPROM start write address */
      mem_add[0] = (mem_address & 0x3F00) >> 8;
      mem_add[1] = mem_address & 0x00FF;

      /* Send memory address */
      status = I2C_MASTER_Transmit(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, mem_add, 2, false);
      while(I2C_MASTER_IsTxBusy(&I2C_MASTER_EEPROM));

      if (status == I2C_MASTER_STATUS_SUCCESS)
	  {
        /* Start reading data */
        status = I2C_MASTER_Receive(&I2C_MASTER_EEPROM, true, EEPROM_24CW128X_I2C_ADDRESS, rx_data, rx_size, true, true);
        while(I2C_MASTER_IsRxBusy(&I2C_MASTER_EEPROM));
      }
	}
  }

  if (status == I2C_MASTER_STATUS_SUCCESS)
  {
    return EEPROM_STATUS_SUCCESS;
  }
  else
  {
    return EEPROM_STATUS_READ_ERROR;
  }
}

//============================================================================

/* Function to perform a software rest in case of I2C communication error */
EEPROM_STATUS_t bsp_eeprom_24cw128x_software_reset(void)
{
  DAVE_STATUS_t init_status = DAVE_STATUS_SUCCESS;
  EEPROM_STATUS_t status = EEPROM_STATUS_FAILURE;

  /**  Re-initialization SCL pin as output to perform the Software Reset */
  init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_EEPROM_SW_RESET);

  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_EEPROM_SW_RESET);
  if (init_status == DAVE_STATUS_SUCCESS)
  {
    /* Generate 9 clock cycles on SCL pin to perform a software reset */
    for(uint32_t index = 0; index < 9; index++)
    {
       DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_EEPROM_SW_RESET);
       bsp_time_delay(1, BSP_TIMER_DELAY_MICROSECOND);
       DIGITAL_IO_SetOutputLow(&DIGITAL_IO_EEPROM_SW_RESET);
       bsp_time_delay(1, BSP_TIMER_DELAY_MICROSECOND);
    }

    /* Re-initialization the I2C_MASTER APP instance I2C_MASTER_EEPROM */
	init_status = (DAVE_STATUS_t)I2C_MASTER_Init(&I2C_MASTER_EEPROM);

	/* Software Reset successfully performed */
	if (init_status == DAVE_STATUS_SUCCESS)
	{
	  status = EEPROM_STATUS_SUCCESS;
    }
  }

  return (status);
}

//============================================================================

/* Function used to generate a time in units of microseconds or milliseconds */
void bsp_time_delay(uint32_t delay_value, BSP_TIMER_DELAY_t time_unit)
{
  uint32_t delay_cnt;

  TIMER_ClearEvent(&EEPROM_TIMER_DELAY);

  if (time_unit == BSP_TIMER_DELAY_MILLISECOND)
  {
	/* Time in units of milliseconds */
	delay_cnt = (1000U * 100U) * delay_value;
  }
  else
  {
	/* Time in units of microseconds */
    delay_cnt = delay_value;
  }

  TIMER_SetTimeInterval(&EEPROM_TIMER_DELAY, delay_cnt);

  TIMER_Start(&EEPROM_TIMER_DELAY);

  while(!TIMER_GetInterruptStatus(&EEPROM_TIMER_DELAY));

  TIMER_Stop(&EEPROM_TIMER_DELAY);

  TIMER_Clear(&EEPROM_TIMER_DELAY);
}

#if !defined(BUILD_FOR_ARDUINO)

//============================================================================

void bsp_led_green_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_GREEN_LED);
}

//============================================================================

void bsp_led_green_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_GREEN_LED);
}

//============================================================================

void bsp_led_green_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_GREEN_LED);
}
//============================================================================

void bsp_led_blue_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_blue_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_blue_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_BLUE_LED);
}

//============================================================================

void bsp_led_red_on(void)
{
  DIGITAL_IO_SetOutputLow(&DIGITAL_IO_RED_LED);
}

//============================================================================

void bsp_led_red_off(void)
{
  DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_RED_LED);
}

//============================================================================

void bsp_led_red_toggle(void)
{
  DIGITAL_IO_ToggleOutput(&DIGITAL_IO_RED_LED);
}
#endif

/*
==============================================================================
   7. LOCAL FUNCTIONS
==============================================================================
 */

/* --- End of File -------------------------------------------------------- */
