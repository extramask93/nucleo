/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define BTN_LCD_Pin GPIO_PIN_13
#define BTN_LCD_GPIO_Port GPIOC
#define BTN_LCD_EXTI_IRQn EXTI4_15_IRQn
#define BTN_CAL_Pin GPIO_PIN_1
#define BTN_CAL_GPIO_Port GPIOC
#define BTN_CAL_EXTI_IRQn EXTI0_1_IRQn
#define BTN_CAL_CO2_Pin GPIO_PIN_2
#define BTN_CAL_CO2_GPIO_Port GPIOC
#define SOIL_PWR_Pin GPIO_PIN_0
#define SOIL_PWR_GPIO_Port GPIOA
#define SOIL_Pin GPIO_PIN_1
#define SOIL_GPIO_Port GPIOA
#define BATTERY_Pin GPIO_PIN_2
#define BATTERY_GPIO_Port GPIOA
#define BAT_CMP_Pin GPIO_PIN_3
#define BAT_CMP_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define LCD_DIN_Pin GPIO_PIN_7
#define LCD_DIN_GPIO_Port GPIOA
#define TMP_Pin GPIO_PIN_8
#define TMP_GPIO_Port GPIOC
#define CO2_PWR_Pin GPIO_PIN_8
#define CO2_PWR_GPIO_Port GPIOA
#define CO2_TX_Pin GPIO_PIN_9
#define CO2_TX_GPIO_Port GPIOA
#define CO2_RX_Pin GPIO_PIN_10
#define CO2_RX_GPIO_Port GPIOA
#define CO2_HD_Pin GPIO_PIN_11
#define CO2_HD_GPIO_Port GPIOA
#define BAT_CMP_OUT_Pin GPIO_PIN_12
#define BAT_CMP_OUT_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define CHRG_STAT_Pin GPIO_PIN_15
#define CHRG_STAT_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_10
#define LCD_RST_GPIO_Port GPIOC
#define LCD_CE_Pin GPIO_PIN_11
#define LCD_CE_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_12
#define LCD_DC_GPIO_Port GPIOC
#define LCD_CLK_Pin GPIO_PIN_3
#define LCD_CLK_GPIO_Port GPIOB
#define LCD_LED_PWR_Pin GPIO_PIN_4
#define LCD_LED_PWR_GPIO_Port GPIOB
#define BH_SCL_Pin GPIO_PIN_6
#define BH_SCL_GPIO_Port GPIOB
#define BH_SDA_Pin GPIO_PIN_7
#define BH_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
