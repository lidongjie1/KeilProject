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


/*****************������************************/

u8 BST_u8MainEventCount;						  //��ѭ���жϼ���  ��SysTick_Handler(void)��ʹ�� ÿ1ms��1
u8 BST_u8SpeedControlCount;						  //�ٶȿ���ѭ������  ��SysTick_Handler(void)��ʹ�� ÿ5ms��1
u8 BST_u8SpeedControlPeriod;
u8 BST_u8DirectionControlPeriod;
u8 BST_u8DirectionControlCount;					  //ת�����ѭ������  ��SysTick_Handler(void)��ʹ�� ÿ5ms��1 
u8 BST_u8trig;
u8 ucBluetoothValue;                      //������������
float volt = 12.0;

/******������Ʋ���******/
float BST_fSpeedControlOut;						   //�ٶȿ���PWM
float BST_fSpeedControlOutOld;
float BST_fSpeedControlOutNew;
float BST_fAngleControlOut;
float BST_fLeftMotorOut;
float BST_fRightMotorOut;

float BST_fCarAngle;						 //�Ƕȿ���PWM
float gyro_z;
float gyrx;
float gy0;

/*-----�ǶȻ����ٶȻ�PID���Ʋ���-----*///���²ο�Ϊ�ص���Բο���ͬ��ص�ѹ�йأ������õ��ٵ���
float  BST_fCarAngle_P =91.3;//	91.3 //����Сʱ�����Ұڣ�����ʱ����  ����������ܹ�վ�� P=91.3�����ڸ�С�����˶�����ʹ��
float  BST_fCarAngle_D =0.21;	// 0.001 0.002 0.004 0.008 0.0010 0.011	 ��Сʱ��Ӧ��������ʱ�����

float  BST_fCarSpeed_P=5.1;
float  BST_fCarSpeed_I=0.10;

const double PID_Original[4] ={91.3, 0.21, 5.1, 0.10}; 
char  alldata[80];
char *iap;

/******�ٶȿ��Ʋ���******/
s16   BST_s16LeftMotorPulse;					  //����������
s16	  BST_s16RightMotorPulse;					   //�ҵ��������

s32   BST_s32LeftMotorPulseOld;
s32   BST_s32RightMotorPulseOld;
s32   BST_s32LeftMotorPulseSigma;				  //50ms��������ֵ
s32   BST_s32RightMotorPulseSigma;				 //50ms�ҵ������ֵ

float BST_fCarSpeed;							 //�������̵ó��ĳ���
float BST_fCarSpeedOld;

float BST_fCarPosition;						   //��������ͨ������õ���С��λ��

/*-----��ͣ����-----*/
int leftstop=0;
int rightstop=0;
int stopflag=0;

/************��ת*****************/
float BST_fBluetoothDirectionL;				   //����ת��־λ  ����PWM��ת���ʹ���ж������ʽ ����Ҫ��־λ
float BST_fBluetoothDirectionR;				   //����ת��־λ  ����PWM��ת���ʹ���ж������ʽ ����Ҫ��־λ
int driectionxco=800;


//******����������************
		
float  Q_angle=0.001;  
float  Q_gyro=0.003;
float  R_angle=0.5;
float  dt=0.005;	                  //dtΪkalman�˲�������ʱ��;
char   C_0 = 1;
float  Q_bias, Angle_err;
float  PCt_0=0, PCt_1=0, E=0;
float  K_0=0, K_1=0, t_0=0, t_1=0;
float  Pdot[4] ={0,0,0,0};
float  PP[2][2] = { { 1, 0 },{ 0, 1 } };


void SendAutoUp(void);

/*��ʱ�Ӻ���*/
void delay_nms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;  
      while(i--) ;    
   }
}