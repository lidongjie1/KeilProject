#include "controler.h"
#include "I2C_MPU6050.h"
#include "motor.h"
#include "encoder.h"
#include "usart1.h"
#include "usart3.h"
#include "MPU6050.H"
#include "stm32f10x_gpio.h"
#include "math.h" 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*****************多数据************************/

u8 BST_u8MainEventCount;						  //主循环判断计数  在SysTick_Handler(void)中使用 每1ms加1
u8 BST_u8SpeedControlCount;						  //速度控制循环计数  在SysTick_Handler(void)中使用 每5ms加1
u8 BST_u8SpeedControlPeriod;
u8 BST_u8DirectionControlPeriod;
u8 BST_u8DirectionControlCount;					  //转向控制循环计数  在SysTick_Handler(void)中使用 每5ms加1 
u8 BST_u8trig;
u8 ucBluetoothValue;                      //蓝牙接收数据
float volt = 12.0;

/******电机控制参数******/
float BST_fSpeedControlOut;						   //速度控制PWM
float BST_fSpeedControlOutOld;
float BST_fSpeedControlOutNew;
float BST_fAngleControlOut;
float BST_fLeftMotorOut;
float BST_fRightMotorOut;

float BST_fCarAngle;						 //角度控制PWM
float gyro_z;
float gyrx;
float gy0;

/*-----角度环和速度环PID控制参数-----*///以下参考为重点调试参考，同电池电压有关，建议充好电再调试
float  BST_fCarAngle_P =91.3;//	91.3 //调大小时会左右摆，调大时会振动  请调到基本能够站立 P=91.3是用于给小车在运动过程使用
float  BST_fCarAngle_D =0.21;	// 0.001 0.002 0.004 0.008 0.0010 0.011	 调小时反应慢，调大时会干扰

float  BST_fCarSpeed_P=5.1;
float  BST_fCarSpeed_I=0.10;

const double PID_Original[4] ={91.3, 0.21, 5.1, 0.10}; 
char  alldata[80];
char *iap;

/******速度控制参数******/
s16   BST_s16LeftMotorPulse;					  //左电机脉冲数
s16	  BST_s16RightMotorPulse;					   //右电机脉冲数

s32   BST_s32LeftMotorPulseOld;
s32   BST_s32RightMotorPulseOld;
s32   BST_s32LeftMotorPulseSigma;				  //50ms左电机叠加值
s32   BST_s32RightMotorPulseSigma;				 //50ms右电机叠加值

float BST_fCarSpeed;							 //测速码盘得出的车速
float BST_fCarSpeedOld;

float BST_fCarPosition;						   //测速码盘通过计算得到的小车位移

/*-----悬停参数-----*/
int leftstop=0;
int rightstop=0;
int stopflag=0;

/************旋转*****************/
float BST_fBluetoothDirectionL;				   //左旋转标志位  由于PWM旋转输出使用判断输出方式 固需要标志位
float BST_fBluetoothDirectionR;				   //右旋转标志位  由于PWM旋转输出使用判断输出方式 固需要标志位
int driectionxco=800;


//******卡尔曼参数************
		
float  Q_angle=0.001;  
float  Q_gyro=0.003;
float  R_angle=0.5;
float  dt=0.005;	                  //dt为kalman滤波器采样时间;
char   C_0 = 1;
float  Q_bias, Angle_err;
float  PCt_0=0, PCt_1=0, E=0;
float  K_0=0, K_1=0, t_0=0, t_1=0;
float  Pdot[4] ={0,0,0,0};
float  PP[2][2] = { { 1, 0 },{ 0, 1 } };


void SendAutoUp(void);

/*延时子函数*/
void delay_nms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;  
      while(i--) ;    
   }
}