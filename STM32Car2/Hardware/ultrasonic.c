/*	        
 Ӳ�����ӣ�
            PB0  - TRIG      
            PB1  - ECHO      
*/

#include "ultrasonic.h"
#include "usart1.h"
#include "usart3.h"
#include "delay.h"
#include "stm32f10x_exti.h"
 #include "controler.h"
 #include "outputdata.h"
 #include "mpu6050.h"
 #include "I2C_MPU6050.h"


#define	TRIG_PORT      GPIOB		//����TRIG_PORTΪPB     
#define	ECHO_PORT      GPIOB		//����ECHO_PORTΪPB 
#define	TRIG_PIN       GPIO_Pin_0   //����TRIG ��ӦIO�� PB0      
#define	ECHO_PIN       GPIO_Pin_1	//����ECHO ��ӦIO�� PB1  
   
int count;

/*
 * ��������UltrasonicWave_Configuration
 * ����  ��������ģ��ĳ�ʼ��
 */
void UltrasonicWave_Configuration(void)
{

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//�ر�jtag
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	
	
	
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;					 //PC8��TRIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //��Ϊ�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);	                 //��ʼ������GPIO 

	GPIO_InitStructure.GPIO_Pin = ECHO_PIN;				     //PB1��ECH0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //��Ϊ����
	GPIO_Init(ECHO_PORT,&GPIO_InitStructure);						 //��ʼ��GPIOB
	
	 //GPIOB.1	  �ж����Լ��жϳ�ʼ������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitTypeDef NVIC_InitStructure;
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //���ȼ����	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//��ռ���ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//�����ȼ�1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
											    
/***********EXTI1 �жϳ���****************/
void EXTI1_IRQHandler(void)
{
	
     if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
			TIM_SetCounter(TIM1,0);
			TIM_Cmd(TIM1, ENABLE);   //����ʱ��	Tim1
		
			 count=1;               
            while(count)	                 //�ȴ��͵�ƽ
			{
					if(TIM_GetCounter(TIM1)>=10) //9*10us=90us
					{ 
					    TIM_Cmd(TIM1, DISABLE); 	// �ر�ʱ�� Tim1
						count=0;					// ѭ��������
					} 
					else  count=GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN); //���PB1�Ƿ�ߵ�ƽ �� ��countΪ1 �� ��countΪ0  	
						       
			}
			TIM_Cmd(TIM1, DISABLE);			                                 //��ʱ�� Tim1 �ر�
			
	
	EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI1��·����λ
	
}

}

/*
 * ��������UltrasonicWave_StartMeasure
 * ����  ����ʼ��࣬����һ��>10us�����壬Ȼ��������صĸߵ�ƽʱ��
	
 */
void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(TRIG_PORT,TRIG_PIN); 		  //��>10US�ĸߵ�ƽ�T  ��ָ���룺GPIO_SetBits(PB,GPIO_Pin_0) ��ͬ
  delay_us(12);		                      //��ʱ11US
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);	   //��ָ���룺GPIO_SetBits(PB,GPIO_Pin_1) ��ͬ
	
}


void Timerx_Init(u16 arr,u16 psc)
{   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //TIM1ʱ��ʹ��

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 5000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM1�ж�
		TIM1, //TIM1
		TIM_IT_Update  |  //TIM1 �ж�Դ
		TIM_IT_Trigger,   //TIM1 �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�ж�  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
							 
}

void TIM1_UP_IRQHandler(void)   //TIM1�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM1�жϷ������:TIM1 �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM1 �ж�Դ 

		    
	  }
}

