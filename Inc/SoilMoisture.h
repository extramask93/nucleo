/*
 * SoilMoisture.h
 *
 *  Created on: 19.11.2017
 *      Author: LENOVO
 */

#ifndef SOILMOISTURE_H_
#define SOILMOISTURE_H_
#include <stdint.h>
typedef enum {
	WET,DRY
} Calibration_t;
void SoilInit();
void SoilDeInit();
void SaveWetValue();
void SaveDryValue();
uint32_t ReadValue();

#endif /* SOILMOISTURE_H_ */
