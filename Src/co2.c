/*
 * co2.c
 *
 *  Created on: 27.01.2018
 *      Author: LENOVO
 */

#include "co2.h"

static char CO2_GetCheckSum(char * packet) {
	char i,checksum;
	for(i=1;i<8;i++) {
		checksum += packet[i];
	}
	checksum = 0xFF - checksum;
	checksum += 1;
	return checksum;
}
void CO2_Init() {
/*baud: 9600, data bits: 8, stop bits: 1, no parity*/
}
void CO2_DeInit() {
	;
}
uint16_t CO2_GetConcentration() {
	;
	/*starting byte: 0xFF
	sensor id: 0x01
	command id: 0x86
	5x byte: 0x00
	check value byte: 0x79*/
	/******************************/
	/*RESPONSE*/
	/*starting byte: 0xFF
	command id: 0x86
	4x byte: 0x00
	check value byte: */
	/*CALCULATION */
	/*gas concetration = high byte *256 + low byte*/
}
uint16_t CO2_CalibrateZeroPoint() {
	;/*starting byte: 0xFF
	sensor id: 0x01
	command id: 0x87
	5x byte: 0x00
	check value byte: 0x78*/
}
uint16_t CO2_CalibrateSpanPoint(){
	;/*starting byte: 0xFF
	sensor id: 0x01
	command id: 0x88
	higher byte span: 0x07
	lower byte span: 0xD0
	3x byte: 0x00
	check value byte: 0x79*/
}
void CO2_TurnOffAutoCalibration() {
	;/*starting byte: 0xFF
	sensor id: 0x01
	command id: 0x79
	turn_off: 0x00
	4x byte: 0x00
	check value byte: 0x79*/
}
