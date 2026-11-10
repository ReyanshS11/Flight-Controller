/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RF_DIO1_Pin GPIO_PIN_2
#define RF_DIO1_GPIO_Port GPIOE
#define ESC_BAT_Pin GPIO_PIN_1
#define ESC_BAT_GPIO_Port GPIOC
#define RF_DIO0_Pin GPIO_PIN_0
#define RF_DIO0_GPIO_Port GPIOA
#define RF_RESET_Pin GPIO_PIN_1
#define RF_RESET_GPIO_Port GPIOA
#define GPS_RESET_Pin GPIO_PIN_2
#define GPS_RESET_GPIO_Port GPIOA
#define GPS_EXTINT_Pin GPIO_PIN_3
#define GPS_EXTINT_GPIO_Port GPIOA
#define ICM_CS_Pin GPIO_PIN_4
#define ICM_CS_GPIO_Port GPIOA
#define ICM_INT1_Pin GPIO_PIN_4
#define ICM_INT1_GPIO_Port GPIOC
#define ICM_INT2_Pin GPIO_PIN_5
#define ICM_INT2_GPIO_Port GPIOC
#define MAG_CS_Pin GPIO_PIN_0
#define MAG_CS_GPIO_Port GPIOB
#define GPS_CS_Pin GPIO_PIN_1
#define GPS_CS_GPIO_Port GPIOB
#define RF_CS_Pin GPIO_PIN_2
#define RF_CS_GPIO_Port GPIOB
#define ESC_M1_Pin GPIO_PIN_9
#define ESC_M1_GPIO_Port GPIOE
#define ESC_M2_Pin GPIO_PIN_11
#define ESC_M2_GPIO_Port GPIOE
#define ESC_M3_Pin GPIO_PIN_13
#define ESC_M3_GPIO_Port GPIOE
#define ESC_M4_Pin GPIO_PIN_14
#define ESC_M4_GPIO_Port GPIOE
#define BQ_CE_Pin GPIO_PIN_11
#define BQ_CE_GPIO_Port GPIOD
#define BMP_INT_Pin GPIO_PIN_12
#define BMP_INT_GPIO_Port GPIOD
#define BQ_INT_Pin GPIO_PIN_13
#define BQ_INT_GPIO_Port GPIOD
#define SD_CS_Pin GPIO_PIN_0
#define SD_CS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
