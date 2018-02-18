/*
 * co2.c
 *
 *  Created on: 27.01.2018
 *      Author: LENOVO
 */

#include "co2.h"
#include "usart.h"
#include "mbtask.h"
#include "stm32l0xx_hal_uart.h"
#define MAX_RESPONSE_SIZE 9
static const uint8_t tab[] = {0xFF, 0x01, 0x86, 0x00,0x00,0x00,0x00,0x00,0x79};
uint8_t temp[10];
typedef enum {
	Ready, HeatingUp, Transmiting, WaitingForResponse, Error
}CO2_State;
volatile CO2_State CO2_state = Ready;
static uint8_t CO2_GetCheckSum(uint8_t * packet) {
	uint8_t i,checksum;
	for(i=1;i<8;i++) {
		checksum += packet[i];
	}
	checksum = 0xFF - checksum;
	checksum += 1;
	return checksum;
}
void CO2_Init() {
	CO2_TurnOn();
	MX_USART1_UART_Init();
}
void CO2_DeInit() {
	if(!coils[11]) {
		CO2_TurnOff();
	}
	HAL_UART_MspDeInit(&huart1);
	huart1.gState = HAL_UART_STATE_RESET;
}
enum ErrorType CO2_GetConcentration(uint16_t *result) {
	if(HAL_UART_Transmit(&huart1,tab,sizeof(tab),100))
		return CO2_TRERROR;
	HAL_UART_Transmit_IT(&huart1,tab,sizeof(tab));
	if(HAL_UART_Receive(&huart1,temp,9,1000))
		return CO2_REERROR;
	if(CO2_GetCheckSum(temp)!= temp[8])
		return CO2_CHERROR;
	*result = temp[2]*256 + temp[3];
	return CO2_OK;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(CO2_GetCheckSum(temp)!= temp[8])
		CO2_state = Error;
	else {
		usRegInputBuf[5] = temp[2]*256 + temp[3];
		CO2_state = Ready;
	}
	coils[4]=0;
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	HAL_UART_Receive_IT(&huart,temp,9);
	CO2_state = WaitingForResponse;
}
void  HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	//error
}
inline void CO2_TurnOn() {
	HAL_GPIO_WritePin(CO2_PWR_GPIO_Port,CO2_PWR_Pin,GPIO_PIN_SET);
}
inline void CO2_TurnOff() {
	if(!coils[11])
		HAL_GPIO_WritePin(CO2_PWR_GPIO_Port,CO2_PWR_Pin,GPIO_PIN_RESET);
}
