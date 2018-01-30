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
/*
 Rules for co2 timer:
 if(czas <=2min)
 	 always_on;
 	 sleep();
 if(czas > 2min)
 	 set_timer_for(czas-2min);
 	 sleep();
 after wake up:
 	 readCo2();
 	 turnOffCo2()
 	 readSensors();
 	 restert timer();
 	 sleep();
 */
#define REG_INPUT_START 1001
#define REG_INPUT_NREGS 8
#define REG_HOLD_START 0
#define REG_HOLD_NREGS 8
#define COIL_START 2000
#define COIL_NREGS 12


extern Calibration_t lastCalibrated;
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static uint32_t * holdingRegMem = (uint32_t*)(0x080800000);
uint8_t coils[COIL_NREGS];
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
volatile bool measurementRequested = false;
volatile int LCD_request = 0;


void ModbusRTUTask(void const * argument)
{
	eMBInit(MB_RTU, 1, 3, 9600, MB_PAR_NONE);
	eMBEnable();
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
    	ReloadRTC();
    	PrintOnLCD();
    	lcdPrintRequested=0;
    }
    if(calibrationRequested) {
    	ResetSTOPTimer();
    	ReadCalibrationData();
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

