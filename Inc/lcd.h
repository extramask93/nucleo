/*
 * lcd.h
 *
 *  Created on: 09.12.2017
 *      Author: LENOVO
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include "font.h"
#include "stm32l0xx_hal.h"

#define LCD_COMMAND 0
#define LCD_DATA 1

#define LCD_SETYADDR 0x40
#define LCD_SETXADDR 0x80
#define LCD_DISPLAY_BLANK 0x08
#define LCD_DISPLAY_NORMAL 0x0C
#define LCD_DISPLAY_ALL_ON 0x09
#define LCD_DISPLAY_INVERTED 0x0D
#define LCD_DISPLAY_POWER_DOWN 0x24

#define LCD_PORT_RST LCD_RST_GPIO_Port
#define LCD_PIN_RST LCD_RST_Pin
#define LCD_PORT_CE LCD_CE_GPIO_Port
#define LCD_PIN_CE LCD_CE_Pin
#define LCD_PORT_DC LCD_DC_GPIO_Port
#define LCD_PIN_DC LCD_DC_Pin


#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define LCD_SIZE LCD_WIDTH * LCD_HEIGHT / 8

/*
 * @brief LCD parameters
 */
struct LCD_att{
	uint8_t buffer[LCD_SIZE];
	bool inverttext;
};

/*
 * @brief GPIO ports used
 */
struct LCD_GPIO{
	GPIO_TypeDef* RSTPORT;
	uint16_t RSTPIN;

	GPIO_TypeDef* CEPORT;
	uint16_t CEPIN;

	GPIO_TypeDef* DCPORT;
	uint16_t DCPIN;

};

/*----- Library Functions -----*/
void LCD_send(uint8_t val);
void LCD_write(uint8_t data, uint8_t mode);
void LCD_init();
void LCD_invert(bool mode);
void LCD_invertText(bool mode);
void LCD_putChar(char c);
void LCD_print(char *str, uint8_t x, uint8_t y);
void LCD_clrScr();
void LCD_goXY(uint8_t x, uint8_t y);

/*----- Draw Functions -----*/
/*
 * These functions draw in a buffer variable. It's necessary to use LCD_refreshScr() or LCD_refreshArea()
 * in order to send data to the LCD.
 */
void LCD_refreshScr();
void LCD_refreshArea(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax);
void LCD_setPixel(uint8_t x, uint8_t y, bool pixel);
void LCD_drawHLine(int x, int y, int l);
void LCD_drawVLine(int x, int y, int l);
void LCD_drawLine(int x1, int y1, int x2, int y2);
void LCD_drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);


#endif /* LCD_H_ */
