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

void am2302_Init();
int am2302_ReadData(TH_Data *result);


#endif /* AM2302_H_ */
