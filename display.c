#include "display.h"

void write_digit(uint8_t pos_val)
{
  if((pos_val < 11 && pos_val > 0) || pos_val == 0)
  {
    BDC_Port->BRR |= (0x0F << 0);
    BDC_Port->BSRR |= (pos_val << 0);
  }
  else
  {
    BDC_Port->BRR  |= (0x0F << 0);
    BDC_Port->BSRR |= (0x0F << 0);
  }
}


void choose_anode(uint8_t num_anode)
{
  
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);
  
  switch (num_anode)
  {
    case 0:
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
      break;

    case 1:
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
      break;
    
    case 2:
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
      break;
  }
}

void write_display(uint8_t hund_pos_val, uint8_t ten_pos_val, uint8_t zero_pos_val)
{
  write_digit(zero_pos_val);
  choose_anode(1);  
  HAL_Delay(1);  
  write_digit(ten_pos_val);
  choose_anode(2);
  HAL_Delay(1);
  write_digit(hund_pos_val);
  choose_anode(0);
  HAL_Delay(1);
}



