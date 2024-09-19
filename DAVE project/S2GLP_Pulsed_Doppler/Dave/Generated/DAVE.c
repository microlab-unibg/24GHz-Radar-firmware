
/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2014-06-16:
 *     - Initial version<br>
 * 2015-08-28:
 *     - Added CLOCK_XMC1_Init conditionally
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "DAVE.h"

/***********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/

/*******************************************************************************
 * @brief This function initializes the APPs Init Functions.
 *
 * @param[in]  None
 *
 * @return  DAVE_STATUS_t <BR>
 ******************************************************************************/
DAVE_STATUS_t DAVE_Init(void)
{
  DAVE_STATUS_t init_status;
  
  init_status = DAVE_STATUS_SUCCESS;
     /** @Initialization of APPs Init Functions */
     init_status = (DAVE_STATUS_t)CLOCK_XMC4_Init(&CLOCK_XMC4_0);

  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_G0 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_G0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_FRAME */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_FRAME); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance TIMER_FRAME_TRIG */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&TIMER_FRAME_TRIG); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of USBD_VCOM APP instance USBD_VCOM_0 */
	 init_status = (DAVE_STATUS_t)USBD_VCOM_Init(&USBD_VCOM_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of GLOBAL_CCU8 APP instance GLOBAL_CCU8_0 */
	 init_status = (DAVE_STATUS_t)GLOBAL_CCU8_Init(&GLOBAL_CCU8_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_G1 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_G1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DMA_CH APP instance DMA_CH_G0 */
	 init_status = (DAVE_STATUS_t)DMA_CH_Init(&DMA_CH_G0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DMA_CH APP instance DMA_CH_G1 */
	 init_status = (DAVE_STATUS_t)DMA_CH_Init(&DMA_CH_G1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_SCAN */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_SCAN); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_BGT_VCC_PTAT */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_BGT_VCC_PTAT); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWM_CCU8_BGT_VCC */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWM_CCU8_BGT_VCC); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWM_CCU8_BGT_TX_ON */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWM_CCU8_BGT_TX_ON); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWM_CCU8_SAMPLE_HOLD */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWM_CCU8_SAMPLE_HOLD); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWM_CCU8_ADC_SAMPLE */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWM_CCU8_ADC_SAMPLE); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_RED_LED */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_RED_LED); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_BLUE_LED */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_BLUE_LED); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_GREEN_LED */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_GREEN_LED); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance TIMER_PTAT_DELAY */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&TIMER_PTAT_DELAY); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_PTAT_TIMER */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_PTAT_TIMER); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_G2 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_G2); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_G3 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_G3); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DMA_CH APP instance DMA_CH_G3 */
	 init_status = (DAVE_STATUS_t)DMA_CH_Init(&DMA_CH_G3); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DMA_CH APP instance DMA_CH_G2 */
	 init_status = (DAVE_STATUS_t)DMA_CH_Init(&DMA_CH_G2); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of I2C_MASTER APP instance I2C_MASTER_EEPROM */
	 init_status = (DAVE_STATUS_t)I2C_MASTER_Init(&I2C_MASTER_EEPROM); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance EEPROM_TIMER_DELAY */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&EEPROM_TIMER_DELAY); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_ARD_D4 */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_ARD_D4); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_ARD_D7 */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_ARD_D7); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_ARD_D6 */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_ARD_D6); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of CRC_SW APP instance CRC_SW_EEPROM */
	 init_status = (DAVE_STATUS_t)CRC_SW_Init(&CRC_SW_EEPROM); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_V_PTAT_SH_EN */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_V_PTAT_SH_EN); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance TIMER_PTAT_S_H_DELAY */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&TIMER_PTAT_S_H_DELAY); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_PTAT_S_H_DELAY */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_PTAT_S_H_DELAY); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_BGT_VCC_ON */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_BGT_VCC_ON); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_BB1_EN */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_BB1_EN); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIGITAL_IO_BB2_EN */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIGITAL_IO_BB2_EN); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_BB_PRE_TRIGGER */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_BB_PRE_TRIGGER); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance TIMER_BB_PRE_TRIGGER */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&TIMER_BB_PRE_TRIGGER); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance INTERRUPT_ERROR_TOGGLE */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&INTERRUPT_ERROR_TOGGLE); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of TIMER APP instance TIMER_ERROR_TOGGLE */
	 init_status = (DAVE_STATUS_t)TIMER_Init(&TIMER_ERROR_TOGGLE); 
   }  
  return init_status;
} /**  End of function DAVE_Init */

