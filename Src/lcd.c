/*
 * lcd.c
 *
 *  Created on: 09.12.2017
 *      Author: LENOVO
 */
#include "lcd.h"
#include "spi.h"
#include "gpio.h"
#include <stdint.h>
#include "port.h"
#include "BV1750FVI.h"
#include "mb.h"
#include "am2302.h"
#include "tim.h"
#include "mbtask.h"
#include "mbport.h"
struct LCD_att lcd;
struct LCD_GPIO lcd_gpio;
volatile int cnt =0;
volatile int lcdcnt = 0;
volatile int isOn =0;
extern uint16_t downcounter;
extern volatile bool STOPRequested;
/*
 * @brief Send information to the LCD using configured GPIOs
 * @param val: value to be sent
 */
void stepBrightness(void) {
	/*static uint16_t stepSize = 3;
	uint16_t currentBright_pct,desiredBright_pct;

    BV_Init();
    BV_ReadData(&currentBright_pct);

    if(curr > 60)
    	desiredBright_pct = 0;
    else {

    }
	// if current is at desired, don't do anything
	if (currentBright_pct == desiredBright_pct)
		return;
	// is the current brightness above the desired brightness?
	else if (currentBright_pct > desiredBright_pct) {
		// is the difference between the two less than one step?
		if ( (currentBright_pct-stepSize) < desiredBright_pct)
			currentBright_pct = desiredBright_pct;
		else
			currentBright_pct -= stepSize;
	} // else if
	else if (currentBright_pct < desiredBright_pct) {
		// is the difference between the two less than one step?
		if ( (currentBright_pct+stepSize) > desiredBright_pct)
			currentBright_pct = desiredBright_pct;
		else
			currentBright_pct += stepSize;
	} // else if
	 __HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, currentBright_pct);
	return;*/
} // stepBrightness
static void LCD_AdjustBrightness()
{
	//HAL_ResumeTick();
	uint16_t pwm;
    uint16_t lightVal=0;
    BV_Init();
    BV_ReadData(&lightVal);
    if(lightVal==0)
    	lightVal=1;
    if(lightVal<40) {
    	pwm = 30;
    }
    else {
    	pwm = 0;
    }
    __HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, pwm);


}
static void LCD_Power_Down() {
	LCD_clrScr();
	LCD_write(LCD_DISPLAY_POWER_DOWN, LCD_COMMAND);
}
void LCD_Off() {
	if(!isOn)
		return;
	isOn=0;
	minutes = 0;
	HAL_TIM_PWM_Stop(&htim22,TIM_CHANNEL_1);
	HAL_TIM_Base_DeInit(&htim22);
	LCD_Power_Down();
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance == TIM2) {
		if(isOn) {
			LCD_AdjustBrightness();
			++lcdcnt;
		}
		cnt++;
		if(lcdcnt >= 6) {
			LCD_Off();
			lcdcnt = 0;
		}
		if(cnt>=6) {
			LCD_Off();
			am2302_DeIninit();
			STOPRequested=true;
			HAL_TIM_Base_Stop_IT(&htim2);
		}
	}
	else if(htim->Instance == TIM6) {
		if (!--downcounter)
			pxMBPortCBTimerExpired();
	}
}
void LCD_Reset() {
HAL_GPIO_WritePin(lcd_gpio.RSTPORT,lcd_gpio.RSTPIN,GPIO_PIN_RESET);
HAL_GPIO_WritePin(lcd_gpio.RSTPORT,lcd_gpio.RSTPIN,GPIO_PIN_SET);
}
void LCD_send(uint8_t val){
	HAL_SPI_Transmit(&hspi1,&val,1,100);
}

/*
 * @brief Writes some data into the LCD
 * @param data: data to be written
 * @param mode: command or data
 */
void LCD_write(uint8_t data, uint8_t mode){
  if(mode == LCD_COMMAND){
    HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_RESET);
    LCD_send(data);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_SET);
  }
  else{
    HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_RESET);
    LCD_send(data);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_SET);
  }
}

/*
 * @brief Initialize the LCD using predetermined values
 */
void LCD_init(){
	if(isOn)
		return;
	isOn=1;
	MX_TIM22_Init();
	HAL_TIM_PWM_Start(&htim22, TIM_CHANNEL_1);
	LCD_AdjustBrightness();
	lcd_gpio.RSTPORT = LCD_PORT_RST;
	lcd_gpio.RSTPIN = LCD_PIN_RST;
	lcd_gpio.CEPORT = LCD_PORT_CE;
	lcd_gpio.CEPIN = LCD_PIN_CE;
	lcd_gpio.DCPORT = LCD_PORT_DC;
	lcd_gpio.DCPIN = LCD_PIN_DC;
	MX_SPI1_Init();
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
	LCD_Reset();
	LCD_write(0x21, LCD_COMMAND); //LCD extended commands.
	LCD_write(0xB9, LCD_COMMAND); //set LCD Vop(Contrast).
	LCD_write(0x04, LCD_COMMAND); //set temp coefficent.
	LCD_write(0x14, LCD_COMMAND); //LCD bias mode 1:40.
	LCD_write(0x20, LCD_COMMAND); //LCD basic commands.
	LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND); //LCD normal.
	LCD_clrScr();
	lcd.inverttext = false;
}

/*
 * @brief Invert the color shown on the display
 * @param mode: true = inverted / false = normal
 */
void LCD_invert(bool mode){
  if(mode == true){
    LCD_write(LCD_DISPLAY_INVERTED, LCD_COMMAND);
  }
  else{
    LCD_write(LCD_DISPLAY_NORMAL, LCD_COMMAND);
  }
}

/*
 * @brief Invert the colour of any text sent to the display
 * @param mode: true = inverted / false = normal
 */
void LCD_invertText(bool mode){
  if(mode == true){
    lcd.inverttext = true;
  }
  else{
    lcd.inverttext = false;
  }
}

/*
 * @brief Puts one char on the current position of LCD's cursor
 * @param c: char to be printed
 */
void LCD_putChar(char c){
  for(int i = 0; i < 6; i++){
    if(lcd.inverttext != true)
      LCD_write(ASCII[c - 0x20][i], LCD_DATA);
    else
      LCD_write(~(ASCII[c - 0x20][i]), LCD_DATA);
  }
}

/*
 * @brief Print a string on the LCD
 * @param x: starting point on the x-axis (column)
 * @param y: starting point on the y-axis (line)
 */
void LCD_print(char *str, uint8_t x, uint8_t y){
  LCD_goXY(x, y);
  while(*str){
    LCD_putChar(*str++);
  }
}

/*
 * @brief Clear the screen
 */
void LCD_clrScr(){
  for(int i = 0; i < 504; i++){
    LCD_write(0x00, LCD_DATA);
    lcd.buffer[i] = 0;
  }
}

/*
 * @brief Set LCD's cursor to position X,Y
 * @param x: position on the x-axis (column)
 * @param y: position on the y-axis (line)
 */
void LCD_goXY(uint8_t x, uint8_t y){
  LCD_write(0x80 | x, LCD_COMMAND); //Column.
  LCD_write(0x40 | y, LCD_COMMAND); //Row.
}

/*
 * @brief Updates the entire screen according to lcd.buffer
 */
void LCD_refreshScr(){
  LCD_goXY(LCD_SETXADDR, LCD_SETYADDR);
  for(int i = 0; i < 6; i++){
    for(int j = 0; j < LCD_WIDTH; j++){
      LCD_write(lcd.buffer[(i * LCD_WIDTH) + j], LCD_DATA);
    }
  }
}

/*
 * @brief Updates a square of the screen according to given values
 * @param xmin: starting point on the x-axis
 * @param xmax: ending point on the x-axis
 * @param ymin: starting point on the y-axis
 * @param ymax: ending point on the y-axis
 */

/*
 * @brief abs function used in LCD_drawLine
 * @param x: any integer
 * @return absolute value of x
 */
int abs(int x){
	if(x < 0){
		return x*-1;
	}
	return x;
}

/*
 * @brief Draws any line
 * @param x1: starting point on the x-axis
 * @param y1: starting point on the y-axis
 * @param x2: ending point on the x-axis
 * @param y2: ending point on the y-axis
 */


/*
 * @brief Draws a rectangle
 * @param x1: starting point on the x-axis
 * @param y1: starting point on the y-axis
 * @param x2: ending point on the x-axis
 * @param y2: ending point on the y-axis
 */


