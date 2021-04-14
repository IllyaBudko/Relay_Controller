#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include "stm32f0xx_hal.h"
#include "main.h"

void write_digit(uint8_t pos_val);
void choose_anode(uint8_t num_anode);
void write_display(uint8_t hund_pos_val, uint8_t ten_pos_val, uint8_t zero_pos_val);




#endif /*_DISPLAY_H_*/
