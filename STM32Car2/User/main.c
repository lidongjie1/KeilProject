
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
 * 函数名：main
 * 描述  ：主函数
 */
int main(void)
{	
       
	
	SystemInit();                   //=====系统初始化
	//Timerx_Init(5000,7199);				   //定时器TIM1
	//UltrasonicWave_Configuration(); 	   //超声波初始化设置 IO口及中断设置			    

	Usart1_Init();						//串口1初始化 上位机
	//Usart3_Init();						//串口3初始化 蓝牙与USART3公用相同IO口

	TIM2_PWM_Init();					   //PWM输出初始化
	Motor_Init();				  //电机IO口初始化
	Adc_Init();
	TIM3_Encoder_Init();                       //编码器获取脉冲数 PA6 7 
	TIM4_Encoder_Init();                       //编码器获取脉冲数 PB6 7	
	////////////////////DMP/////////////////////////////////
	i2cInit();							   //IIC初始化 用于挂靠在总线上的设备使用
	delay_nms(10);						   //延时10ms
	MPU6050_Init();						   //MPU6050 DMP陀螺仪初始化

	SysTick_Init();						  //SysTick函数初始化	
	CarUpstandInit();					  //小车直立参数初始化
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;	 //使能总算法时钟

	while (1)
	{
		MPU6050_Pose();						 //获取MPU6050角度状态

		if (newLineReceived)
		{
			ProtocolCpyData();
			Protocol();
		}
		/*通过状态控制小车*/
		CarStateOut();		
		SendAutoUp();
		
	 }
 								    
}
