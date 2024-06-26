#ifndef __DELAY_H
#define __DELAY_H


#include "stm32f10x.h"

void delay_us(u32 n);
void delay_ms(u32 n);
void get_ms(unsigned long *time);
#endif

