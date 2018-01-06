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

void StopMode() {
	GPIO_InitTypeDef GPIO_InitStruct;
	LCD_clrScr();
	//MX_GPIO_DeInit();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = BTN_LCD_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_LCD_GPIO_Port, &GPIO_InitStruct);
	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = BTN_CAL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_CAL_GPIO_Port, &GPIO_InitStruct);
	/*DEINIT UNUSER PERPHERALS*/
	HAL_ADC_DeInit(&hadc);
	HAL_SPI_DeInit(&hspi1);
	HAL_I2C_DeInit(&hi2c1);
	HAL_TIM_Base_MspDeInit(&htim2);
	HAL_TIM_Base_MspDeInit(&htim22);
	HAL_TIM_Base_MspDeInit(&htim6);
	LPUART1->CR1 |=USART_CR1_UESM;
	HAL_SuspendTick();
	/* ENABLE CLOCK TO WPR MODULE*/
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_RESET);
	 /* Request to enter STOP mode */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	/* Resume Tick interrupt if disabled prior to sleep mode entry*/
	LPUART1->CR1 &= (~USART_CR1_UESM);
	hlpuart1.gState = HAL_UART_STATE_RESET;
	SystemClock_Config();
	HAL_ResumeTick();
	//htim2.State = HAL_UART_STATE_RESET;
	//MX_TIM2_Init();
	MX_LPUART1_UART_Init();
	eMBInit(MB_RTU, 1, 3, 9600, MB_PAR_NONE);
	eMBEnable();

	/*ENABLE BACK GPIO*/
	MX_GPIO_Init();
	HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_SET);
	/*CLEAR STATE OF PERIPHERALS TO TURN THEM BACK ON*/
	htim22.State = HAL_UART_STATE_RESET;
	htim6.State = HAL_UART_STATE_RESET;
	hi2c1.State   = HAL_I2C_STATE_RESET;
	hadc.State    = HAL_ADC_STATE_RESET;
	hspi1.State = HAL_SPI_STATE_RESET;
	/*ENABLE MODBUS TIMER*/
}

