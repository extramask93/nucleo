/*
 * battery.c
 *
 *  Created on: 05.01.2018
 *      Author: LENOVO
 */
#include "battery.h"
#include "adc.h"
#include "mbtask.h"
#include <math.h>
#define BATTERY_CHANNEL ADC_CHANNEL_3
#define VREF 3300
#define SCLAING 1.16

void BatteryInit() {
	  /*just add battery channel to DMA sequence*/
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = BATTERY_CHANNEL;
	  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}
void BatteryDeInit() {
	  /*remove battery channel from DMA sequence*/
	  ADC_ChannelConfTypeDef sConfig;
	  sConfig.Channel = BATTERY_CHANNEL;
	  sConfig.Rank = ADC_RANK_NONE;
	  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}
uint32_t ReadBatteryValue() {
	/*load raw value from dma buffer and calculate actual voltage*/
	double temp = ((double)y[1])/ 4095 *3300;
	temp = (temp*4.2)/1.16;
	return (uint32_t)round(temp);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1) {
}
