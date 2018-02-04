///*
// * mbtask.c1
// *
// *  Created on: 26.11.2017
// *      Author: LENOVO
// */
#include "mb.h"
#include "mbport.h"
#include "mbtask.h"
#include "am2302.h"
#include "BV1750FVI.h"
#include "usart.h"
#include "tim.h"
#include "rtc.h"
#include "i2c.h"
#include "adc.h"
#include "spi.h"
#include "gpio.h"
#include "lcd.h"
#include "rtc.h"
#include "SoilMoisture.h"
#include "battery.h"
#include "power.h"
#include "co2.h"
#include <stdio.h>
#include <stdbool.h>



extern Calibration_t lastCalibrated;
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
volatile uint8_t coils[COIL_NREGS] = {0,0,0,0,0,1,1,1,1,1,1,1};
static char bufferT[13];
static char bufferH[13];
uint16_t timcnt = 0;
TH_Data data=(const TH_Data){ 0 };
extern volatile int cnt;
volatile bool STOPRequested =0;
volatile bool calibrationRequested=false;
volatile bool lcdPrintRequested =false;
volatile bool measurementRequested = false;
volatile int LCD_request = 0;


void ModbusRTUTask(void const * argument)
{
	eMBInit(MB_RTU, 1, 3, 9600, MB_PAR_NONE);
	eMBEnable();
	memset(coils,0x1,5);
	MeasureAll();
	StopMode();
  while(1) {
    eMBPoll();
    StartSTOPTimer();
    if(measurementRequested) {
    	ResetSTOPTimer();
    	MeasureAll();
    	ReloadRTC();
    	measurementRequested = false;
    }
    if(lcdPrintRequested) {
    	ResetSTOPTimer();
    	MeasureAndPrint();
    	ReloadRTC();
    	lcdPrintRequested=0;
    }
    if(calibrationRequested) {
    	ReadCalibrationData();
    	ResetSTOPTimer();
    	calibrationRequested=false;
    }
    if(STOPRequested) {
    	STOPRequested = false;
    	StopMode();
    }
  }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	if(eMode==MB_REG_WRITE) {
	    if( ( usAddress >= REG_HOLD_START )
	        && ( usAddress + usNRegs <= REG_HOLD_START+ REG_HOLD_NREGS ))
	    {
	    	while(usNRegs>0) {
	    	HAL_FLASHEx_DATAEEPROM_Unlock();
	    	HAL_FLASHEx_DATAEEPROM_Erase(0x08080000);/*erase only available by word*/
	    	unsigned int h = *pucRegBuffer++;
	    	unsigned int l = *pucRegBuffer++;
	    	unsigned int a = (h <<8) | (l&0xFF);
	    	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD,0x08080000+(usAddress*4),a);
	    	usAddress++;
	    	usNRegs--;
	    	}
	    	HAL_FLASHEx_DATAEEPROM_Lock();
	    }
	    else
	    	return MB_ENOREG;
	}
	if(eMode == MB_REG_READ) {
	    if( ( usAddress >= REG_HOLD_START )
	        && ( usAddress + usNRegs <= REG_HOLD_START+ REG_HOLD_NREGS ))
	    {
	    	uint32_t *mem = (uint32_t*)(0x08080000);
	    	mem += usAddress;
	    	while(usNRegs>0) {
	    		*pucRegBuffer++ = (unsigned char)((*mem) >>8);
	    		*pucRegBuffer++ = (unsigned char)((*mem) & 0xFF);
	    		mem++;
	    		usNRegs--;
	    	}
	    }
	    else
	    	return MB_ENOREG;
	}

    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int iRegIndex;
	int shift=0;
	iRegIndex = ( int )( usAddress - COIL_START );

	if(eMode == MB_REG_WRITE) {
		/*write measurements coils*/
		unsigned reg = *pucRegBuffer++;
		if(usNCoils>8)
			reg |= ((*pucRegBuffer)<<8) & 0xFF00;

		if( ( usAddress >= COIL_START )
		   && ( usAddress + usNCoils <= COIL_START+ COIL_NREGS )) {
			while(usNCoils>0) {
				if(((reg>>shift)&0x1))
					coils[iRegIndex] = 1;
				else
					coils[iRegIndex] = 0;
				iRegIndex++;
				shift++;
				usNCoils--;
			}
			measurementRequested=true;
		}
		else
			return MB_ENOREG;
	}
	else if(eMode == MB_REG_READ) {
		if( ( usAddress >= COIL_START )
		   && ( usAddress + usNCoils <= COIL_START+ COIL_NREGS )) {
			*pucRegBuffer=0;
			while(usNCoils>0) {
				if(shift>8) {
					shift=0;
					pucRegBuffer++; *pucRegBuffer=0;
				}
				*pucRegBuffer |= coils[iRegIndex]<<shift;
				++iRegIndex;
				++shift;
				--usNCoils;
			}
		}
		else
			return MB_ENOREG;
	}
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == BTN_LCD_Pin) {	/*Show data on LCD*/
		lcdPrintRequested = true;
		cnt=0;
	}
	else if(GPIO_Pin==BTN_CAL_Pin) {	/*Calibrate soil sensor*/
		calibrationRequested =true;
		cnt=0;
	}
}
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
	measurementRequested = true;
}
void ReadCalibrationData() {
	LCD_init();
	LCD_clrScr();
	if(lastCalibrated == WET) {
		LCD_print("Saving value  of dry soil...",0,0);
		SaveDryValue();
		lastCalibrated = DRY;
	}
	else {
		LCD_print("Saving value  of wet soil...",0,0);
		SaveWetValue();
		lastCalibrated = WET;
	}
}
void MeasureAll() {
    if(coils[0]) {
    	MeasureTemperatureAndHumidity();
    	coils[0]=0;
    }
    if(coils[1]) {
    	MeasureLight();
    	coils[1]=0;
    }
    if(coils[2]) {
    	MeasureSoilLevel();
    	coils[2]=0;
    }
    if(coils[3]) {
    	MeasureBattery();
    	coils[3]=0;
    }
    if(coils[4]) {
    	if(coils[11])
    		MeasureCO2Level();
    	else
    		;//turn on co2 sensor and scheduele measurement
    	coils[4]=0;
    }
}
void MeasureAndPrint() {
	LCD_init();
	LCD_clrScr();
	LCD_print("Measuring...",0,0);
	PrintTemperatureAndHumidity(MeasureTemperatureAndHumidity());
	ResetSTOPTimer();
	PrintLightLevel(MeasureLight());
	ResetSTOPTimer();
	PrintSoilLevel(MeasureSoilLevel());
	ResetSTOPTimer();
	PrintBatteryLevel(MeasureBattery());
	ResetSTOPTimer();
	PrintCO2Level(MeasureCO2Level());
	ResetSTOPTimer();
}
int MeasureBattery() {
	if(!coils[8])
		return 4;
	BatteryInit();
	usRegInputBuf[4] = ReadBatteryValue();
}
void PrintBatteryLevel() {
	float bat = usRegInputBuf[4]/10.0;
	int bat1 = bat;                  // Get the integer (678).
	float batfrac = bat - bat1;      // Get fraction (0.1).

	sprintf(bufferT,"Battery:%dmV",usRegInputBuf[4]);
	LCD_print(bufferT,0,4);
}
int MeasureSoilLevel() {
	if(!coils[7])
		return 4;
	SoilInit();
	usRegInputBuf[3] = ReadValue();
}
void PrintSoilLevel() {
	sprintf(bufferT,"Soil: %i %%",usRegInputBuf[3]);
	LCD_print(bufferT,0,3);
}
int MeasureCO2Level() {
	if(!coils[9])
		return 4;
	CO2_Init();
	int a = CO2_GetConcentration(&usRegInputBuf[5]);
	CO2_DeInit();
	return a;
}
void PrintCO2Level(int error) {
	if(error) {
		sprintf(bufferT,"CO2: Error (%i)",error);
	}
	else
		sprintf(bufferT,"CO2: %i ppm",usRegInputBuf[5]);
	LCD_print(bufferT,0,5);
}
int MeasureLight() {
	if(!coils[6])
		return 4;
	BV_Init();
	int a = BV_ReadData(&usRegInputBuf[2]);
	BV_DeInit();
	return a;
}
void PrintLightLevel(int error) {
	if(error)
		sprintf(bufferT, "L: Error(%i)",error);
	else
		sprintf(bufferT, "L: %i lx",usRegInputBuf[2]);
	LCD_print(bufferT,0,2);
}
int MeasureTemperatureAndHumidity() {
	/*static RTC_TimeTypeDef lastMeasurementTime = {0};
	RTC_DateTypeDef temp;
	RTC_TimeTypeDef now;
	HAL_RTC_GetTime(&hrtc,&now,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&temp,RTC_FORMAT_BIN);*/
	if(!coils[5])
		return 4;
	am2302_Init();
	data=(const TH_Data){ 0 };
	int result=0;
	for(int i=0;i<2;i++) {
		result = am2302_ReadData(&data);
		if(!result) {
			usRegInputBuf[0] = data.itemperature;
			usRegInputBuf[1] = data.ihumidity;
			break;
		}
		HAL_Delay(2000);
	}
	am2302_DeIninit();
	return result;
}
void PrintTemperatureAndHumidity(int error) {
	if(error){
		sprintf(bufferT,"T: Error (%i)",error);
		sprintf(bufferH,"H: Error (%i)",error);
	}
	else {
	float temp = usRegInputBuf[0]/10.0;
	int tmpInt1 = temp;                  // Get the integer (678).
	float tmpFrac = temp - tmpInt1;      // Get fraction (0.1).
	int tmpInt2 = tmpFrac * 10;  // Turn into integer (123).
	float humidity = usRegInputBuf[1]/10.0;
	int humInt1 = humidity;                  // Get the integer (678).
	float humFrac = humidity - humInt1;      // Get fraction (0.1).
	int humInt2 = humFrac * 10;  // Turn into integer (123).
	sprintf(bufferT,"T: %d.%d\x7f C   ",tmpInt1,tmpInt2);
	sprintf(bufferH,"H: %d.%d %%",humInt1,humInt2);
	}
	LCD_print(bufferT,0,0);
	LCD_print(bufferH,0,1);
}


