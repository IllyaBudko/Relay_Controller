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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UART1_TX_Debug_Pin GPIO_PIN_2
#define UART1_TX_Debug_GPIO_Port GPIOA
#define UART2_RX_Debug_Pin GPIO_PIN_3
#define UART2_RX_Debug_GPIO_Port GPIOA
#define Down_Arrow_Key_Pin GPIO_PIN_5
#define Down_Arrow_Key_GPIO_Port GPIOA
#define Set_Key_Pin GPIO_PIN_6
#define Set_Key_GPIO_Port GPIOA
#define Up_Arrow_Key_Pin GPIO_PIN_7
#define Up_Arrow_Key_GPIO_Port GPIOA
#define Relay_Output_Pin GPIO_PIN_1
#define Relay_Output_GPIO_Port GPIOB
#define I2C1_SCL_Display_Pin GPIO_PIN_9
#define I2C1_SCL_Display_GPIO_Port GPIOA
#define I2C1_SDA_Display_Pin GPIO_PIN_10
#define I2C1_SDA_Display_GPIO_Port GPIOA

#define Com_Anode_Port GPIOA
#define Zero_Anode_Pin GPIO_PIN_9
#define Ten_Anode_Pin GPIO_PIN_10
#define Hundred_Anode_Pin GPIO_PIN_13

#define BDC_Port GPIOA

#define BDC_A_Pin GPIO_PIN_0
#define BDC_B_Pin GPIO_PIN_1
#define BDC_C_Pin GPIO_PIN_2
#define BDC_D_Pin GPIO_PIN_3
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
