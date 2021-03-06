/*
 * power.c
 *
 *  Created on: 06.01.2018
 *      Author: LENOVO
 */
#include "power.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "lcd.h"
#include "mb.h"
#include "co2.h"
#include "mbtask.h"
#include "stm32l0xx_hal_uart.h"
uint16_t GetRTCTimFromEEPROM() {
	uint16_t *mem = (uint16_t*)(0x08080004);
	return *mem;
}
uint16_t GetAddressFromEEPROM() {
	uint16_t *mem = (uint16_t*)(0x08080008);
	return *mem;
}
static void PreparteLPUARTToStopMode() {
	 /* make sure that no LPUART transfer is on-going */
	  while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_BUSY) == SET);
	  /* make sure that UART is ready to receive
	  * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
	  while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_REACK) == RESET);
	  LPUART1->CR1 |=USART_CR1_UESM;
}
static void SystemPower_Config(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();
  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
  HAL_SuspendTick();
}
void FromStopMode() {
	LPUART1->CR1 &= (~USART_CR1_UESM);
	MX_ADC_Init();
}
void StopMode() {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = BTN_LCD_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_LCD_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = BTN_CAL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_CAL_GPIO_Port, &GPIO_InitStruct);;
	/*DEINIT UNUSED PERPHERALS*/
	HAL_ADC_DeInit(&hadc);
	HAL_SPI_DeInit(&hspi1);
	HAL_I2C_DeInit(&hi2c1);
	HAL_TIM_Base_Stop_IT(&htim6);
	HAL_TIM_Base_MspDeInit(&htim2);
	HAL_TIM_Base_MspDeInit(&htim22);
	HAL_TIM_Base_MspDeInit(&htim21);
	htim22.State = HAL_TIM_STATE_RESET;
	htim21.State = HAL_TIM_STATE_RESET;
	htim2.State = HAL_TIM_STATE_RESET;
	hi2c1.State   = HAL_I2C_STATE_RESET;
	hadc.State= HAL_ADC_STATE_RESET;
	hspi1.State = HAL_SPI_STATE_RESET;
	CO2_DeInit();
	PreparteLPUARTToStopMode();
	SystemPower_Config();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
	FromStopMode();
	/*ENABLE MODBUS TIMER*/
}

