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
#define REG_INPUT_NREGS 8
#define REG_HOLD_START 0
#define REG_HOLD_NREGS 8
#define COIL_START 2000
#define COIL_NREGS 12
#define SCR_WIDTH 14
extern volatile uint8_t coils[COIL_NREGS];
extern volatile uint8_t printCoils[5];
extern volatile uint16_t y[2];
extern volatile USHORT usRegInputBuf[REG_INPUT_NREGS];
extern RTC_TimeTypeDef lastMeasurementTime;
extern RTC_DateTypeDef tempdate;
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
