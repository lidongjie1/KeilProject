#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"
void UltrasonicWave_Configuration(void);               //对超声波模块初始化
void UltrasonicWave_StartMeasure(void);                //开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
void Timerx_Init(u16 arr,u16 psc); 
extern float juli;

#endif
