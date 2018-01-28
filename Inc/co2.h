/*
 * co2.h
 *
 *  Created on: 27.01.2018
 *      Author: LENOVO
 */

#ifndef CO2_H_
#define CO2_H_
#include <stdint.h>
void CO2_Init();
void CO2_DeInit();
uint16_t CO2_GetConcentration();
uint16_t CO2_CalibrateZeroPoint();
uint16_t CO2_CalibrateSpanPoint();
void CO2_TurnOffAutoCalibration();


#endif /* CO2_H_ */
