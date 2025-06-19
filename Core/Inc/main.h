/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l0xx_hal.h"

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
void Timer_Interrupt_Handler(void);
void User_Button_Interrupt_Handler(void);
void Rotary_Encoder_Interrupt_Handler(void);
uint32_t Get_ADC_Value(uint32_t adc_channel);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CURRENT_IN_Pin GPIO_PIN_3
#define CURRENT_IN_GPIO_Port GPIOA
#define VOLTAGE_IN_Pin GPIO_PIN_4
#define VOLTAGE_IN_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_15
#define LED_RED_GPIO_Port GPIOA
#define USER_BUTTON_Pin GPIO_PIN_3
#define USER_BUTTON_GPIO_Port GPIOB
#define USER_BUTTON_EXTI_IRQn EXTI2_3_IRQn
#define ROT_CHB_Pin GPIO_PIN_4
#define ROT_CHB_GPIO_Port GPIOB
#define ROT_CHB_EXTI_IRQn EXTI4_15_IRQn
#define ROT_CHA_Pin GPIO_PIN_5
#define ROT_CHA_GPIO_Port GPIOB
#define ROT_CHA_EXTI_IRQn EXTI4_15_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
