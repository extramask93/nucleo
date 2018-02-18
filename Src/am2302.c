/*
 * am2302.c
 *
 *  Created on: 18.11.2017
 *      Author: LENOVO
 */

#include "am2302.h"
#include <string.h>
#include "i2c.h"
#include "tim.h"
#include "tim.h"

uint8_t am2302Data[5];
volatile int timeOutFlag;

static void am2302_pinOut() {
	  GPIO_InitTypeDef GPIO_InitStruct;
	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(TMP_GPIO_Port, TMP_Pin, GPIO_PIN_SET);
	  /*Configure GPIO pin : PB3 */
	  GPIO_InitStruct.Pin = TMP_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(TMP_GPIO_Port, &GPIO_InitStruct);
}
static void am2302_pinInput() {

	  GPIO_InitTypeDef GPIO_InitStruct;
	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(TMP_GPIO_Port, TMP_Pin, GPIO_PIN_SET);
	  /*Configure GPIO pin : PB3 */
	  GPIO_InitStruct.Pin = TMP_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(TMP_GPIO_Port, &GPIO_InitStruct);

}
static void am2302_getTemperature(TH_Data *data) {
	  uint16_t retVal=0;
	  float temperature=0;
	  retVal = am2302Data[2];
	  retVal <<= 8;
	  retVal |= (am2302Data[3]) & 0xFF;
	  temperature = retVal;
	  temperature /= 10;
	  data->itemperature=retVal;
	  data->temperature = temperature;
}
static void am2302_getHumidity(TH_Data *data) {
	  uint16_t retVal=0;
	  float humidity=0;
	  retVal = am2302Data[0] & 0xFF;
	  retVal <<= 8;
	  retVal |= (am2302Data[1]) & 0xFF;
	  humidity = retVal;
	  humidity /= 10;
	  data->ihumidity=retVal;
	  data->humidity = humidity;
}
static int am2302_checkParity() {
	uint8_t parity = am2302Data[0]+am2302Data[1]+am2302Data[2]+am2302Data[3];
	if(parity!=am2302Data[4])
		return 1;
	return 0;
}
void am2302_Init() {
	MX_TIM21_Init();//MX_TIM2_Init();
	am2302_pinOut();
}
void am2302_DeIninit() {
	HAL_TIM_Base_MspDeInit(&htim21);//HAL_TIM_Base_MspDeInit(&htim2);
	htim21.State = HAL_TIM_STATE_RESET;
	//HAL_GPIO_DeInit(TMP_GPIO_Port, TMP_Pin);
}
int am2302_ReadData(TH_Data *result) {
	memset(am2302Data,0x0,5); //reset
	timeOutFlag=0;
	int currentByte=0;
	int counter = 7;
	am2302_pinOut();
	HAL_GPIO_WritePin(TMP_GPIO_Port,TMP_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);//start -> data line for 1ms down
	am2302_pinInput();
	HAL_TIM_Base_Start(&htim21);
	__disable_irq();
	TIM21->CNT=0;//TIM2->CNT = 0;
	while(HAL_GPIO_ReadPin(TMP_GPIO_Port,TMP_Pin)){ if(TIM21->CNT>500) {__enable_irq();return 1;}}
	TIM21->CNT = 0;
	while(!HAL_GPIO_ReadPin(TMP_GPIO_Port,TMP_Pin)){ if(TIM21->CNT>500) {__enable_irq();return 1;}}
	TIM21->CNT = 0;
	while(HAL_GPIO_ReadPin(TMP_GPIO_Port,TMP_Pin)){ if(TIM21->CNT>500) {__enable_irq();return 1;}}
	for(int i=0;i<40;i++) { //receieve 40 bits
		TIM21->CNT = 0;
		while(!HAL_GPIO_ReadPin(TMP_GPIO_Port,TMP_Pin)){ if(TIM21->CNT>500) {__enable_irq();return 1;}}
		uint32_t time = TIM21->CNT;
		while(HAL_GPIO_ReadPin(TMP_GPIO_Port,TMP_Pin)){ if(TIM21->CNT>500){__enable_irq(); return 1;}}
		if((TIM21->CNT- time) >30) {
			am2302Data[currentByte] |= (1 << counter);
		}
		if(counter == 0) {
			counter=7;
			currentByte++;
		}
		else {
			counter--;
		}
	}
	__enable_irq();
	if(am2302_checkParity())
		return 1;
	HAL_TIM_Base_Stop(&htim21);
	am2302_getTemperature(result);
	am2302_getHumidity(result);
	return 0;
}
