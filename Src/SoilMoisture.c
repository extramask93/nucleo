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

volatile Calibration_t lastCalibrated = WET;
static uint32_t wet = 4096;
static uint32_t dry = 0;
void SoilInit() {
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = ADC_CHANNEL_1;
	  sConfig.Rank = 1;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}
static uint32_t SoilMap(uint32_t value) {
	if(value < dry) return 0;
	if(value > wet) return 100;
	return ((value * 100)/wet);
}
uint32_t ReadValue() {
	uint32_t result=0;
	HAL_ADC_Start(&hadc);
	if(HAL_ADC_PollForConversion(&hadc,100) == HAL_OK)
		result = HAL_ADC_GetValue(&hadc);
		else {
			result= -1;
		}
	result = abs(4096-result);
	result = SoilMap(result);
	HAL_ADC_Stop(&hadc);
	return result;
}
void SaveWetValue() {
	uint32_t result=0;
	HAL_ADC_Start(&hadc);
	if(HAL_ADC_PollForConversion(&hadc,100) == HAL_OK)
		result = HAL_ADC_GetValue(&hadc);
		else {
			result= -1;
		}
	wet = abs(4096-result);
	HAL_ADC_Stop(&hadc);
}
void SaveDryValue() {
	uint32_t result=0;
	HAL_ADC_Start(&hadc);
	if(HAL_ADC_PollForConversion(&hadc,100) == HAL_OK)
		result = HAL_ADC_GetValue(&hadc);
		else {
			result= -1;
		}
	dry = abs(4096-result);
	HAL_ADC_Stop(&hadc);
}
