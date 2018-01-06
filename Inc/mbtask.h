/*
 * mbtask.h
 *
 *  Created on: 26.11.2017
 *      Author: LENOVO
 */

#ifndef MBTASK_H_
#define MBTASK_H_

void ModbusRTUTask(void const * argument);
void PrintOnLCD();
void MeasureAll();
void ReadCalibrationData();
void GoToStop();
#endif /* MBTASK_H_ */
