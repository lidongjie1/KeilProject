#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"
void UltrasonicWave_Configuration(void);               //�Գ�����ģ���ʼ��
void UltrasonicWave_StartMeasure(void);                //��ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��
void Timerx_Init(u16 arr,u16 psc); 
extern float juli;

#endif
