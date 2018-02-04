/*
 * BV1750FVI.h
 *
 *  Created on: 19.11.2017
 *      Author: LENOVO
 */

#ifndef BV1750FVI_H_
#define BV1750FVI_H_
#include "stdint.h"
enum BH_ErrorType {
	BH_OK, BH_TRERROR, BH_REERROR
};
int BV_ReadData(uint16_t *value);
void BV_Init();
void BV_DeInit();

#endif /* BV1750FVI_H_ */
