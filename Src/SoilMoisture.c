/*
 * SoilMoisture.c
 *
 *  Created on: 19.11.2017
 *      Author: LENOVO
 */

#include "SoilMoisture.h"
#include "adc.h"
#include "stm32l053xx.h"
#include <math.h>
#include <stdlib.h>

#define SOIL_CHANNEL ADC_CHANNEL_1
extern uint16_t y[2];

volatile Calibration_t lastCalibrated = WET;
void SoilInit() {
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = SOIL_CHANNEL;
	  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
	  HAL_GPIO_WritePin(SOIL_PWR_GPIO_Port,SOIL_PWR_Pin,GPIO_PIN_SET);
}
void SoilDeInit() {
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = SOIL_CHANNEL;
	  sConfig.Rank = ADC_RANK_NONE;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
	  HAL_GPIO_WritePin(SOIL_PWR_GPIO_Port,SOIL_PWR_Pin,GPIO_PIN_RESET);
}
static uint32_t SoilMap(uint32_t value) {
	uint16_t *dry = (uint16_t*)(0x0808000C);
	uint16_t *wet = (uint16_t*)(0x0808000A);
	if(value < *dry) return 0;
	if(value > *wet) return 100;
	return ((value * 100)/(*wet));
}
uint32_t ReadValue() {
	uint32_t temp = y[0];
	temp = SoilMap(temp);
	return temp;
}
void SaveWetValue() {
	SoilInit();
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,2000);
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,2000);
	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD,0x0808000A,y[0]);
	HAL_FLASHEx_DATAEEPROM_Lock();
	lastCalibrated = WET;
}
void SaveDryValue() {
	SoilInit();
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,2000);
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,2000);
	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD,0x0808000C,y[0]);
	HAL_FLASHEx_DATAEEPROM_Lock();
	lastCalibrated = DRY;
}
