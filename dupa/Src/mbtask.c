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
#include <stdio.h>
#include <stdbool.h>

#define REG_INPUT_START 1001
#define REG_INPUT_NREGS 8
extern Calibration_t lastCalibrated;
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static char bufferT[14];
static char bufferH[14];
static char bufferLX[14];
static char bufferSoil[20];
static char bufferBattery[20];
static char bufferCO2[20];
static char deg[]= {0x00,0x00,0x00,0x02,0x05,0x02};
uint16_t timcnt = 0;
TH_Data data=(const TH_Data){ 0 };
extern volatile int cnt;
volatile bool STOPRequested =0;
volatile bool calibrationRequested=false;
volatile bool lcdPrintRequested =false;
volatile bool measurementRequested =true;
volatile int LCD_request = 0;


void ModbusRTUTask(void const * argument)
{
	eMBInit(MB_RTU, 1, 3, 9600, MB_PAR_NONE);
	eMBEnable();
	StopMode();
  while(1) {
    eMBPoll();
    if(measurementRequested) {
    	MeasureAll();
    	measurementRequested = false;
    	STOPRequested = true;
    }
    if(lcdPrintRequested) {
    	MeasureAll();
    	ReloadRTC();
    	PrintOnLCD();
    	lcdPrintRequested=0;
    	StartSTOPTimer();
    }
    if(calibrationRequested) {
    	ReadCalibrationData();
    	StartSTOPTimer();
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
    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
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
    if(LCD_request) {PrintOnLCD();}
    //init BV
    BV_Init();
    BV_ReadData(&usRegInputBuf[2]);
    //init am23
    am2302_Init();
    data=(const TH_Data){ 0 };
    if(!am2302_ReadData(&data)) {
    	usRegInputBuf[0] = data.itemperature;
    	usRegInputBuf[1] = data.ihumidity;
    }
    //init soil
    SoilInit();
    usRegInputBuf[3] = ReadValue();
    //init battery
	BatteryInit();
	usRegInputBuf[4] = ReadBatteryValue();
}
void SaveConfiguration() {
	uint32_t errorCode=0;
	FLASH_EraseInitTypeDef FLASH_EraseTypeDef;
	FLASH_EraseTypeDef.PageAddress = 0x0801FC00; //last page
	FLASH_EraseTypeDef.NbPages = 1;
	FLASH_EraseTypeDef.TypeErase = FLASH_TYPEERASE_PAGES;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&FLASH_EraseTypeDef,&errorCode);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x801FC00,(666>>0)& 0xF); //save 666 to address
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x801FC01,(666>>8)&0xF); //save 666 to address
	HAL_FLASH_Lock();
}
void PrintOnLCD() {
	LCD_init();
	LCD_clrScr();
	float temp = usRegInputBuf[0]/10.0;
	int tmpInt1 = temp;                  // Get the integer (678).
	float tmpFrac = temp - tmpInt1;      // Get fraction (0.1).
	int tmpInt2 = tmpFrac * 10;  // Turn into integer (123).

	float humidity = usRegInputBuf[1]/10.0;
	int humInt1 = humidity;                  // Get the integer (678).
	float humFrac = humidity - humInt1;      // Get fraction (0.1).
	int humInt2 = humFrac * 10;  // Turn into integer (123).

	sprintf(bufferT,"T: %d.%d",tmpInt1,tmpInt2);
	LCD_print(bufferT,0,0);
	  for(int i = 0; i < 6; i++)
	      LCD_write(deg[i], LCD_DATA);
	LCD_putChar('C');
	sprintf(bufferH,"H: %d.%d %%",humInt1,humInt2);
	LCD_print(bufferH,0,1);
	sprintf(bufferLX, "L: %i lx",usRegInputBuf[2]);
	LCD_print(bufferLX,0,2);
	sprintf(bufferSoil,"Soil: %i %%",usRegInputBuf[3]);
	LCD_print(bufferSoil,0,3);

	float bat = usRegInputBuf[4]/10.0;
	int bat1 = bat;                  // Get the integer (678).
	float batfrac = bat - bat1;      // Get fraction (0.1).
	int bat2 = batfrac * 10;  // Turn into integer (123).

	sprintf(bufferBattery,"Battery:%dmV",usRegInputBuf[4]);
	LCD_print(bufferBattery,0,4);
	sprintf(bufferCO2,"CO2: %i ppm",0);
	LCD_print(bufferCO2,0,5);
	LCD_request = 0;
}

