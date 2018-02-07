/*
 * BV1750FVI.c
 *
 *  Created on: 18.11.2017
 *      Author: LENOVO
 */
#include <string.h>
#include "i2c.h"

static  uint8_t BH1750FVI_ADDR =  (0x23 << 1);
static uint8_t BH1750FVI_ON  = (0x01);
static  uint8_t BH1750FVI_OFF =(0x00);
#define BH1750FVI_OTH (0x20)
static  uint8_t BH1750FVI_CTH =(0x10);

void BV_Init() {
	MX_I2C1_Init();
	HAL_I2C_Master_Transmit(&hi2c1,BH1750FVI_ADDR,&BH1750FVI_ON,1,100);
	HAL_I2C_Master_Transmit(&hi2c1,BH1750FVI_ADDR,&BH1750FVI_CTH,1,100);
}
void BV_DeInit() {
	HAL_I2C_MspDeInit(&hi2c1);
	HAL_I2C_Master_Transmit(&hi2c1,BH1750FVI_ADDR,&BH1750FVI_OFF,1,100);
	hi2c1.State   = HAL_I2C_STATE_RESET;
}
int BV_ReadData(uint16_t *value) {
	uint8_t buffer[2], dataON, dataOTH,dataOFF,addr;
	volatile int result = 0;
	*value =0;
	memset(buffer,0x0,2);
	dataON = BH1750FVI_ON;
	dataOTH = BH1750FVI_OTH;
	dataOFF = BH1750FVI_OFF;
	addr = BH1750FVI_ADDR;
	result = HAL_I2C_Master_Receive(&hi2c1,addr,buffer,2,100);
	if(result)
		return result;
	*value |= buffer[0] & 0xFF;
	*value <<= 8;
	*value |= buffer[1] & 0xFF;
	return result;
}
