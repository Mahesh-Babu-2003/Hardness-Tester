/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define PHYTEC_U4_TX_Pin GPIO_PIN_0
#define PHYTEC_U4_TX_GPIO_Port GPIOA
#define PHYTEC_U4_RX_Pin GPIO_PIN_1
#define PHYTEC_U4_RX_GPIO_Port GPIOA
#define LIMS_U2_TX_Pin GPIO_PIN_2
#define LIMS_U2_TX_GPIO_Port GPIOA
#define LIMS_U2_RX_Pin GPIO_PIN_3
#define LIMS_U2_RX_GPIO_Port GPIOA
#define STEPPER_PULSE_TIM4_CH1_Pin GPIO_PIN_4
#define STEPPER_PULSE_TIM4_CH1_GPIO_Port GPIOA
#define STEPPER_DIR_OP_Pin GPIO_PIN_5
#define STEPPER_DIR_OP_GPIO_Port GPIOA
#define STEPPER_EN_OP_Pin GPIO_PIN_6
#define STEPPER_EN_OP_GPIO_Port GPIOA
#define NTC_ADC_7_Pin GPIO_PIN_7
#define NTC_ADC_7_GPIO_Port GPIOA
#define RPM_TIM3_CH3_Pin GPIO_PIN_0
#define RPM_TIM3_CH3_GPIO_Port GPIOB
#define HOME_IP_Pin GPIO_PIN_1
#define HOME_IP_GPIO_Port GPIOB
#define END_IP_Pin GPIO_PIN_2
#define END_IP_GPIO_Port GPIOB
#define DOT_MAT_U3_TX_Pin GPIO_PIN_10
#define DOT_MAT_U3_TX_GPIO_Port GPIOB
#define DOT_MAT_U3_RX_Pin GPIO_PIN_11
#define DOT_MAT_U3_RX_GPIO_Port GPIOB
#define COLLING_FAN_Pin GPIO_PIN_15
#define COLLING_FAN_GPIO_Port GPIOB
#define TPM_U1_RX_Pin GPIO_PIN_9
#define TPM_U1_RX_GPIO_Port GPIOA
#define TPM_U1_TX_Pin GPIO_PIN_10
#define TPM_U1_TX_GPIO_Port GPIOA
#define DOP_SW_IP_Pin GPIO_PIN_3
#define DOP_SW_IP_GPIO_Port GPIOB
#define HX711_CLK_2_OP_Pin GPIO_PIN_4
#define HX711_CLK_2_OP_GPIO_Port GPIOB
#define HX711_DATA_2_IP_Pin GPIO_PIN_5
#define HX711_DATA_2_IP_GPIO_Port GPIOB
#define HX711_CLK_1_OP_Pin GPIO_PIN_6
#define HX711_CLK_1_OP_GPIO_Port GPIOB
#define HX711_DATA_1_IP_Pin GPIO_PIN_7
#define HX711_DATA_1_IP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
