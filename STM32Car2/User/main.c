
#include "mpu6050.h"
#include "I2C_MPU6050.h"
#include "motor.h"
#include "controler.h"
#include "SysTick.h"
#include "adc.h"
#include "usart1.h"
#include "usart3.h"
#include "i2c.h"
#include "ultrasonic.h"
#include "encoder.h"


float gyz;
int acc;
int acc1;



/*
 * ��������main
 * ����  ��������
 */
int main(void)
{	
       
	
	SystemInit();                   //=====ϵͳ��ʼ��
	//Timerx_Init(5000,7199);				   //��ʱ��TIM1
	//UltrasonicWave_Configuration(); 	   //��������ʼ������ IO�ڼ��ж�����			    

	Usart1_Init();						//����1��ʼ�� ��λ��
	//Usart3_Init();						//����3��ʼ�� ������USART3������ͬIO��

	TIM2_PWM_Init();					   //PWM�����ʼ��
	Motor_Init();				  //���IO�ڳ�ʼ��
	Adc_Init();
	TIM3_Encoder_Init();                       //��������ȡ������ PA6 7 
	TIM4_Encoder_Init();                       //��������ȡ������ PB6 7	
	////////////////////DMP/////////////////////////////////
	i2cInit();							   //IIC��ʼ�� ���ڹҿ��������ϵ��豸ʹ��
	delay_nms(10);						   //��ʱ10ms
	MPU6050_Init();						   //MPU6050 DMP�����ǳ�ʼ��

	SysTick_Init();						  //SysTick������ʼ��	
	CarUpstandInit();					  //С��ֱ��������ʼ��
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;	 //ʹ�����㷨ʱ��

	while (1)
	{
		MPU6050_Pose();						 //��ȡMPU6050�Ƕ�״̬

		if (newLineReceived)
		{
			ProtocolCpyData();
			Protocol();
		}
		/*ͨ��״̬����С��*/
		CarStateOut();		
		SendAutoUp();
		
	 }
 								    
}
