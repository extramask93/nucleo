/*
 * battery.c
 *
 *  Created on: 05.01.2018
 *      Author: LENOVO
 */
#include "battery.h"
#include "adc.h"
void BatteryInit() {
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = 1;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}
uint32_t ReadBatteryValue() {
	uint32_t result=0;
	HAL_ADC_Start(&hadc);
	if(HAL_ADC_PollForConversion(&hadc,100) == HAL_OK)
		result = HAL_ADC_GetValue(&hadc);
		else {
			result= -1;
		}
	HAL_ADC_Stop(&hadc);
	double temp = ((double)result) / 1023 * 3300;
	return temp*1.577;
}

