/*	        
 硬件连接：
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


#define	TRIG_PORT      GPIOB		//定义TRIG_PORT为PB     
#define	ECHO_PORT      GPIOB		//定义ECHO_PORT为PB 
#define	TRIG_PIN       GPIO_Pin_0   //定义TRIG 对应IO口 PB0      
#define	ECHO_PIN       GPIO_Pin_1	//定义ECHO 对应IO口 PB1  
   
int count;

/*
 * 函数名：UltrasonicWave_Configuration
 * 描述  ：超声波模块的初始化
 */
void UltrasonicWave_Configuration(void)
{

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//关闭jtag
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	
	
	
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;					 //PC8接TRIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     //设为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	         
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);	                 //初始化外设GPIO 

	GPIO_InitStructure.GPIO_Pin = ECHO_PIN;				     //PB1接ECH0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		 //设为输入
	GPIO_Init(ECHO_PORT,&GPIO_InitStructure);						 //初始化GPIOB
	
	 //GPIOB.1	  中断线以及中断初始化配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitTypeDef NVIC_InitStructure;
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //优先级组别	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//抢占优先级3 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
											    
/***********EXTI1 中断程序****************/
void EXTI1_IRQHandler(void)
{
	
     if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
			TIM_SetCounter(TIM1,0);
			TIM_Cmd(TIM1, ENABLE);   //开启时钟	Tim1
		
			 count=1;               
            while(count)	                 //等待低电平
			{
					if(TIM_GetCounter(TIM1)>=10) //9*10us=90us
					{ 
					    TIM_Cmd(TIM1, DISABLE); 	// 关闭时钟 Tim1
						count=0;					// 循环数清零
					} 
					else  count=GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN); //检测PB1是否高电平 高 则count为1 低 则count为0  	
						       
			}
			TIM_Cmd(TIM1, DISABLE);			                                 //定时器 Tim1 关闭
			
	
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位
	
}

}

/*
 * 函数名：UltrasonicWave_StartMeasure
 * 描述  ：开始测距，发送一个>10us的脉冲，然后测量返回的高电平时间
	
 */
void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(TRIG_PORT,TRIG_PIN); 		  //送>10US的高电平  本指令与：GPIO_SetBits(PB,GPIO_Pin_0) 等同
  delay_us(12);		                      //延时11US
  GPIO_ResetBits(TRIG_PORT,TRIG_PIN);	   //本指令与：GPIO_SetBits(PB,GPIO_Pin_1) 等同
	
}


void Timerx_Init(u16 arr,u16 psc)
{   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //TIM1时钟使能

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 5000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM1中断
		TIM1, //TIM1
		TIM_IT_Update  |  //TIM1 中断源
		TIM_IT_Trigger,   //TIM1 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
							 
}

void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM1中断发生与否:TIM1 中断源 
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM1 中断源 

		    
	  }
}

