///*
// * mbtask.c1
// *
// *  Created on: 26.11.2017
// *      Author: LENOVO
// */

#include "mbtask.h"
#include "am2302.h"
#include "BV1750FVI.h"
#include "usart.h"
#include "tim.h"
#include "i2c.h"
#include "adc.h"
#include "spi.h"
#include "gpio.h"
#include "lcd.h"
#include "dma.h"
#include "SoilMoisture.h"
#include "battery.h"
#include "power.h"
#include "co2.h"
#include "rtc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



extern Calibration_t lastCalibrated;
static USHORT usRegInputStart = REG_INPUT_START;
USHORT usRegInputBuf[REG_INPUT_NREGS];
volatile uint8_t coils[COIL_NREGS] = {1,1,1,1,1,0};
uint16_t y[2];
char bf[10];
char bufferT[SCR_WIDTH];
char bufferH[SCR_WIDTH];
uint16_t timcnt = 0;
volatile int minutes = 0;
TH_Data data=(const TH_Data){ 0 };
extern volatile int cnt;
volatile bool STOPRequested =0;
volatile bool informRequested =0;
volatile bool calibrationRequested=false;
volatile bool lcdPrintRequested =false;
volatile bool measurementRequested = false;
volatile int LCD_request = 0;


void ModbusRTUTask(void const * argument)
{
	eMBInit(MB_RTU, 1, 3, 9600, MB_PAR_NONE);
	eMBEnable();
	StopMode();
  while(1) {
	/*Przetwarza elementy zakolejkowane w buforze Modbus*/
    eMBPoll();
    /*Uruchamia sprzêtowy zegar, który po up³ywie x sekund przeniesie system w stan uœpienia*/
    StartSTOPTimer();
    /*Obs³uga ¿¹dania wyœwietlenia pomiarów na wbudowanym ekranie*/
    if(lcdPrintRequested) {
    	ResetSTOPTimer();
    	eMBDisable();
    	NVIC_DisableIRQ(AES_LPUART1_IRQn);
    	MeasureAndPrint();
    	eMBEnable();
    	NVIC_EnableIRQ(AES_LPUART1_IRQn);
    	ResetLCDTimer();
    	lcdPrintRequested=0;
    }
    /*Obs³uga ¿¹dania kalibracji cuzjnika wilgotnoœci gleby*/
    if(calibrationRequested) {
    	ReadCalibrationData();
    	ResetSTOPTimer();
    	ResetLCDTimer();
    	calibrationRequested=false;
    }
    if(informRequested) {
    	sprintf(bf,"for:%i m",minutes);
    	LCD_init();
    	LCD_clrScr();
    	LCD_print("CO2 sensor on",0,0);
    	LCD_print(bf,3,1);
    	informRequested = 0;
    }
    /*Przejœcie w stan uspienia*/
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
    	ResetSTOPTimer();
    	MeasureAll();
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
		if(coils[5] == 1) {
			HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 60*3, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
			CO2_TurnOn();
		}
		else
			CO2_TurnOff();
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
		if(HAL_GPIO_ReadPin(BTN_LCD_GPIO_Port,BTN_LCD_Pin)==GPIO_PIN_RESET) {
			minutes++;
			HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
			HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 60*minutes, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
			CO2_TurnOn();
			informRequested=1;
			return;
		}
		calibrationRequested =true;
		cnt=0;
	}
}
void ReadCalibrationData() {
	LCD_init();
	LCD_clrScr();
	if(lastCalibrated == WET) {
		LCD_print("Saving value  of dry soil...",0,0);
		SaveDryValue();
	}
	else {
		LCD_print("Saving value  of wet soil...",0,0);
		SaveWetValue();
	}
}
void MeasureAnalogs() {
	SoilInit();
	BatteryInit();
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,100);
	HAL_ADC_Start_DMA(&hadc,y,2);
	HAL_ADC_PollForConversion(&hadc,100);
	SoilDeInit();
	BatteryDeInit();
	usRegInputBuf[3] = ReadValue();
	usRegInputBuf[4] = ReadBatteryValue();
}
void MeasureAll() {
    	MeasureLight();
    	MeasureAnalogs();
    	MeasureCO2Level();
    	MeasureTemperatureAndHumidity();
}

void PrintAnalogs() {
	memset(bufferT,' ',SCR_WIDTH);
	LCD_print(bufferT,0,3);
	sprintf(bufferT, "Soil: %i %%",usRegInputBuf[3]);
	LCD_print(bufferT,0,3);
	memset(bufferT,' ',SCR_WIDTH);
	LCD_print(bufferT,0,4);
	sprintf(bufferT, "Bat: %i mV",usRegInputBuf[4]);
	LCD_print(bufferT,0,4);
}
void MeasureAndPrint() {
	if(!isOn) {
		LCD_init();
		LCD_clrScr();
		LCD_print("T: ",0,0);
		LCD_print("H: ",0,1);
		LCD_print("L: ",0,2);
		LCD_print("Soil: ",0,3);
		LCD_print("Bat: ",0,4);
		LCD_print("C02: ",0,5);
	}
	PrintTemperatureAndHumidity(MeasureTemperatureAndHumidity());
	PrintLightLevel(MeasureLight());
	MeasureAnalogs();
	PrintAnalogs();
	PrintCO2Level(MeasureCO2Level());
}
int MeasureLight() {
	int a = 0;
	if(coils[2]) {
		BV_Init();
		a = BV_ReadData(&usRegInputBuf[2]);
	}
	return a;
}
void PrintLightLevel(int error) {
	memset(bufferT,' ',SCR_WIDTH);
	LCD_print(bufferT,0,2);
	if(error)
		sprintf(bufferT, "L: Error(%i)",error);
	else
		sprintf(bufferT, "L: %i lx",usRegInputBuf[2]);
	LCD_print(bufferT,0,2);
}


int MeasureCO2Level() {
	int a=0;
	CO2_Init();
	a=CO2_GetConcentration(&usRegInputBuf[5]);
	CO2_DeInit();
	return a;
}
void PrintCO2Level(int error) {
	memset(bufferT,' ',SCR_WIDTH);
	LCD_print(bufferT,0,5);
	if(error) {
		sprintf(bufferT,"CO2: Error (%i)",error);
	}
	else
		sprintf(bufferT,"CO2: %i ppm",usRegInputBuf[5]);
	LCD_print(bufferT,0,5);
}

int MeasureTemperatureAndHumidity() {
	am2302_Init();
	data=(const TH_Data){ 0 };
	int result=0;
	result = am2302_ReadData(&data);
	if(!result) {
		usRegInputBuf[0] = coils[0] ?data.itemperature:0;
		usRegInputBuf[1] = coils[1] ?data.ihumidity:0;
	}
	else {
		usRegInputBuf[0] = -1;
		usRegInputBuf[1] = -1;
	}
	am2302_DeIninit();
	return result;
}

void PrintTemperatureAndHumidity(int error) {
	memset(bufferT,' ',SCR_WIDTH);
	LCD_print(bufferT,0,1);
	LCD_print(bufferT,0,0);
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


