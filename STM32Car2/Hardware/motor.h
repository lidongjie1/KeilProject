#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"                  // Device header

void Motor_Init(void);
void TIM2_PWM_Init(void);
void TIM2_PWM_CHANGE(u16 CCR3,u16 CCR4);
#endif
