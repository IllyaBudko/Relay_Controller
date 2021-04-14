#include "display.h"

void write_digit(uint8_t pos_val)
{
  if(pos_val >= 0 && pos_val < 9)
  {
    BDC_Port->BSRR |= (0x0F << 16);
    BDC_Port->BSRR |= (pos_val << 0);
  }
  else
  {
    BDC_Port->BSRR |= (0x0F << 16);
    BDC_Port->BSRR |= (0x0F << 0);
  }
}

void choose_anode(uint8_t num_anode)
{
  uint8_t choice = num_anode;
  switch (choice)
  {
    case 0:
      HAL_GPIO_WritePin(Com_Anode_Port,Ten_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Hundred_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Zero_Anode_Pin,GPIO_PIN_SET);
      break;

    case 1:
      HAL_GPIO_WritePin(Com_Anode_Port,Zero_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Hundred_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Ten_Anode_Pin,GPIO_PIN_SET);
      break;
    case 2:
      HAL_GPIO_WritePin(Com_Anode_Port,Zero_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Ten_Anode_Pin,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Com_Anode_Port,Hundred_Anode_Pin,GPIO_PIN_SET);
      break;
  }
}

void write_display(uint8_t hund_pos_val, uint8_t ten_pos_val, uint8_t zero_pos_val)
{
  choose_anode(0);
  write_digit(zero_pos_val);
  choose_anode(1);
  write_digit(ten_pos_val);
  choose_anode(2);
  write_digit(hundred_pos_val);
}




