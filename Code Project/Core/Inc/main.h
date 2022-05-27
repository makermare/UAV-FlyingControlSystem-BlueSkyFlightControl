/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define E3_Pin GPIO_PIN_3
#define E3_GPIO_Port GPIOE
#define RGB_R_Pin GPIO_PIN_4
#define RGB_R_GPIO_Port GPIOE
#define RGB_G_Pin GPIO_PIN_5
#define RGB_G_GPIO_Port GPIOE
#define RGB_B_Pin GPIO_PIN_6
#define RGB_B_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_13
#define KEY1_GPIO_Port GPIOC
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define KEY2_Pin GPIO_PIN_0
#define KEY2_GPIO_Port GPIOC
#define KEY2_EXTI_IRQn EXTI0_IRQn
#define Safe_Pin GPIO_PIN_2
#define Safe_GPIO_Port GPIOC
#define Safe_EXTI_IRQn EXTI2_IRQn
#define SPI6_CS_Pin GPIO_PIN_4
#define SPI6_CS_GPIO_Port GPIOA
#define LCD_BLK_Pin GPIO_PIN_10
#define LCD_BLK_GPIO_Port GPIOE
#define SPI4_CS_Pin GPIO_PIN_11
#define SPI4_CS_GPIO_Port GPIOE
#define LCD_WR_RS_Pin GPIO_PIN_13
#define LCD_WR_RS_GPIO_Port GPIOE
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define SDMMC_DETECT_Pin GPIO_PIN_4
#define SDMMC_DETECT_GPIO_Port GPIOD
#define SPI1_BARO_CS_Pin GPIO_PIN_5
#define SPI1_BARO_CS_GPIO_Port GPIOD
#define SPI1_FLASH_CS_Pin GPIO_PIN_6
#define SPI1_FLASH_CS_GPIO_Port GPIOD
#define SBUS_INV_Pin GPIO_PIN_9
#define SBUS_INV_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
