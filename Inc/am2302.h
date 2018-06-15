/*
 * am2302.h
 *
 *  Created on: 18.11.2017
 *      Author: LENOVO
 */

#ifndef AM2302_H_
#define AM2302_H_
#include "stdint.h"

typedef struct TH_Data {
	int16_t itemperature;
	uint16_t ihumidity;
	float temperature;
	float humidity;
} TH_Data;

typedef struct {
	_Bool tempEnabled;
	_Bool humEnabled;
	int16_t temperature;
	uint16_t humidity;
	void (*Init)();
	void (*DeInit)();
	uint8_t (*GetReadings)();

} AM2302_t;
AM2302_t am2302;

void initam2302();
void am2302_Init();
void am2302_DeInit();
int am2302_ReadData(TH_Data *result);


#endif /* AM2302_H_ */
