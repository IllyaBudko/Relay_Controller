/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "debounce.h"
#include "display.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;

uint32_t up_button_history = 0;
uint32_t down_button_history = 0;
uint32_t set_button_history = 0;

uint8_t pulse_val_idx = 0;
uint8_t prev_pulse_val_idx = 0;
uint32_t pulse_val_arr[11] = {0,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000};
uint8_t tens_arr[11] = {15,1,2,3,4,5,6,7,8,9,0};
uint8_t hund_arr[11] = {15,15,15,15,15,15,15,15,15,15,1};
  
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM14_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void set_digits(uint8_t arr_idx, uint8_t *zeros, uint8_t *tens, uint8_t *hundreds);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  uint8_t current_zeros    = 0;
  uint8_t current_tens     = 0;
  uint8_t current_hundreds = 0;
  
  uint8_t to_set_zeros   = 0;
  uint8_t to_set_tens    = 0;
  uint8_t to_set_hundreds = 0;
  
  volatile uint8_t down_button = 0;
  volatile uint8_t up_button   = 0;
  volatile uint8_t set_button  = 0;
  
  uint8_t set_value = 0;
  uint32_t counter = 0;

  HAL_Init();

  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM14_Init();

  if(HAL_TIM_PWM_Start(&htim14,TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
    if(HAL_TIM_PWM_Start_IT(&htim14,TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  while (1)
  { 
    if(is_button_down(&down_button_history) && (down_button == 0))
    {
      if(pulse_val_idx > 0)
      {
        prev_pulse_val_idx = pulse_val_idx;
        pulse_val_idx -= 1;
      }
      set_digits(pulse_val_idx,&to_set_zeros,&to_set_tens,&to_set_hundreds);
      down_button = 1;
      set_value = 1;
    }
    else if(is_button_up(&down_button_history))
    {
      down_button = 0;
    }
    
    if(is_button_down(&up_button_history) && (up_button == 0))
    {
      if(pulse_val_idx < 10)
      {
        prev_pulse_val_idx = pulse_val_idx;
        pulse_val_idx += 1;
      }
      set_digits(pulse_val_idx,&to_set_zeros,&to_set_tens,&to_set_hundreds);
      up_button = 1;
      set_value = 1;
    }
    else if(is_button_up(&up_button_history))
    {
      up_button = 0;
    }
    
    if(is_button_down(&set_button_history) && (set_button == 0))
    {
      uint32_t tmp = pulse_val_arr[pulse_val_idx];
      __HAL_TIM_SET_COMPARE(&htim14,TIM_CHANNEL_1,tmp);
      set_button = 1;
      set_value = 0;
      prev_pulse_val_idx = pulse_val_idx;
      
      current_zeros    = to_set_zeros;
      current_tens     = to_set_tens;
      current_hundreds = to_set_hundreds;
    }
    else if(is_button_up(&set_button_history))
    {
      set_button = 0;
    }
    
    if(set_value)
    {
      write_display(0x0F,0x0F,0x0F);
      HAL_Delay(60);
      write_display(to_set_hundreds,to_set_tens,to_set_zeros);
    }
    else
    {
      write_display(current_hundreds,current_tens,current_zeros);
    }
    
    if((counter < 100) && set_value)
    {
      counter++;
    }
    else if(set_value)
    { 
      uint8_t tmp = pulse_val_arr[prev_pulse_val_idx];
      __HAL_TIM_SET_COMPARE(&htim14,TIM_CHANNEL_1,tmp);
      counter = 0;
      set_value = 0;
     /* 
      to_set_zeros    = current_zeros;
      to_set_tens     = current_tens;
      to_set_hundreds = current_hundreds;
      */
      
    }
  }
}
  /* USER CODE END 3 */

  void set_digits(uint8_t arr_idx, uint8_t *zeros, uint8_t *tens, uint8_t *hundreds)
  {
    *zeros = 0;
    *tens = tens_arr[arr_idx];
    *hundreds = hund_arr[arr_idx];
  } 

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 7999;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 9999;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Relay_Output_GPIO_Port, Relay_Output_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Down_Arrow_Key_Pin Set_Key_Pin Up_Arrow_Key_Pin */
  GPIO_InitStruct.Pin = Down_Arrow_Key_Pin|Set_Key_Pin|Up_Arrow_Key_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  //common anode
  GPIO_InitStruct.Pin = Zero_Anode_Pin|Ten_Anode_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = Hundred_Anode_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //bdc
  GPIO_InitStruct.Pin = BDC_A_Pin|BDC_B_Pin|BDC_C_Pin|BDC_D_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Relay_Output_Pin 
  GPIO_InitStruct.Pin = Relay_Output_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Relay_Output_GPIO_Port, &GPIO_InitStruct);
*/
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
