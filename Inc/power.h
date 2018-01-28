/*
 * power.h
 *
 *  Created on: 06.01.2018
 *      Author: LENOVO
 */

#ifndef POWER_H_
#define POWER_H_
#include <stdint.h>
void StopMode();
uint16_t GetRTCTimFromEEPROM();
uint16_t GetAddressFromEEPROM();

#endif /* POWER_H_ */
