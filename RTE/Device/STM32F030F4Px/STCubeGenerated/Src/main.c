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
  ******************************************************************************/

#include "main.h"
#include "debounce.h"
#include "display.h"

TIM_HandleTypeDef htim14;

uint32_t set_button_history = 0xFFFFFFFFUL;


uint32_t pulse_val_arr[11] = {0,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000};
uint8_t tens_arr[11] = {15,1,2,3,4,5,6,7,8,9,0};
uint8_t hund_arr[11] = {15,15,15,15,15,15,15,15,15,15,1};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM14_Init(void);

void set_digits(uint8_t arr_idx, uint8_t *zeros, uint8_t *tens, uint8_t *hundreds);

int8_t encoder_counter = 0;

int main(void)
{
  uint8_t current_zeros    = 0;
  uint8_t current_tens     = 15;
  uint8_t current_hundreds = 15;
  
  uint8_t to_set_zeros   = 0;
  uint8_t to_set_tens    = 0;
  uint8_t to_set_hundreds = 0;
  
  volatile uint8_t set_button  = 0;
  
  uint8_t set_pulse_idx = 0;
  uint8_t working_pulse_idx = 0;
  
  uint8_t set_value = 0;
  uint32_t counter = 0;
  
  uint8_t encoder_state = R_START;


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
    uint8_t result = encoder_process(&encoder_state);
    if((result == DIR_CW) && (working_pulse_idx < 10))
    {
      working_pulse_idx += 1;
      set_digits(working_pulse_idx,&to_set_zeros,&to_set_tens,&to_set_hundreds);
      set_value = 1;
      counter = 0;
    }
    else if(result == DIR_CCW  && (working_pulse_idx > 0))
    {
      working_pulse_idx -= 1;
      set_digits(working_pulse_idx,&to_set_zeros,&to_set_tens,&to_set_hundreds);
      set_value = 1;
      counter = 0;
    }
    
    
    
    //check set button condition
    if(is_button_down(&set_button_history) && (set_button == 0))
    {
      set_pulse_idx = working_pulse_idx;
      uint32_t tmp = pulse_val_arr[set_pulse_idx];
      __HAL_TIM_SET_COMPARE(&htim14,TIM_CHANNEL_1,tmp);
      set_button = 1;
      set_value = 0;
      
      current_zeros    = to_set_zeros;
      current_tens     = to_set_tens;
      current_hundreds = to_set_hundreds;
    }
    else if(is_button_up(&set_button_history))
    {
      set_button = 0;
    }
    
    //revert back to previous setting if set isnt pressed in time
    if((counter < 50) && set_value)
    {
      counter++;
    }
    else if(set_value)
    {
      working_pulse_idx = set_pulse_idx;
      counter = 0;
      set_value = 0;
      
      to_set_zeros    = current_zeros;
      to_set_tens     = current_tens;
      to_set_hundreds = current_hundreds;
    }
    
    //blinking display for set functionality
    if(set_value)
    {
      uint32_t i = 0;
      for(i = 0; i < 10; i++)
      {
        write_display(0x0F,0x0F,0x0F);
      }
      
      for(i = 0; i < 10; i++)
      {
        write_display(to_set_hundreds,to_set_tens,to_set_zeros);
      }
    }
    else
    {
      write_display(current_hundreds,current_tens,current_zeros);
    }
  }
}

void set_digits(uint8_t arr_idx, uint8_t *zeros, uint8_t *tens, uint8_t *hundreds)
{
  *zeros = 0;
  *tens = tens_arr[arr_idx];
  *hundreds = hund_arr[arr_idx];
} 

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
void Error_Handler(void)
{
  __disable_irq();
  while (1);
}

