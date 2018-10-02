/*
 * mbtask.h
 *
 *  Created on: 26.11.2017
 *      Author: LENOVO
 */

#ifndef MBTASK_H_
#define MBTASK_H_
#include "mb.h"
#include "mbport.h"
#define REG_INPUT_START 1001
#define REG_INPUT_NREGS 6

#define COIL_START 2001
#define COIL_NREGS 6

#define SCR_WIDTH 14
extern volatile uint8_t coils[COIL_NREGS];

extern uint16_t y[2];
extern volatile int minutes;
extern USHORT usRegInputBuf[REG_INPUT_NREGS];

extern char bufferT[SCR_WIDTH];
extern char bufferH[SCR_WIDTH];

void ModbusRTUTask(void const * argument);
void PrintOnLCD();
void MeasureAll();
void ReadCalibrationData();
void GoToStop();
void MeasureAndPrint();
int MeasureBattery();
void PrintBatteryLevel();
int MeasureSoilLevel();
void PrintSoilLevel();
int MeasureCO2Level();
void PrintCO2Level(int error);
int MeasureLight();
void PrintLightLevel(int error);
int MeasureTemperatureAndHumidity();
void PrintTemperatureAndHumidity(int error);
#endif /* MBTASK_H_ */
