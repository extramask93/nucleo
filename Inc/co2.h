/*
 * co2.h
 *
 *  Created on: 27.01.2018
 *      Author: LENOVO
 */

#ifndef CO2_H_
#define CO2_H_
#include <stdint.h>
enum ErrorType {
	CO2_OK, CO2_TRERROR, CO2_REERROR,CO2_CHERROR
};
void CO2_Init();
void CO2_DeInit();
void CO2_TurnOn();
void CO2_TurnOff();
enum ErrorType CO2_GetConcentration(uint16_t *result);



#endif /* CO2_H_ */
