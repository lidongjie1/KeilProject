#include "motor.h"

/*
	端口配置
	控制GPIO PB12/PB13/PB14/PB15
	PB12	- GPIO_Mode_Out_PP	-
	PB13	- GPIO_Mode_Out_PP	-
	PB14	- GPIO_Mode_Out_PP	-
	PB15	- GPIO_Mode_Out_PP	-
	PWM输出电机MOTOR  TIM2_CH3/TIM2_CH4
	
	PA2		- TIM2_CH3
	PA3		- TIM2_CH4
*/


void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	/* 配置连接TB6612电机驱动模块的GPIO端口	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	/* 低电平	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14|GPIO_Pin_12 | GPIO_Pin_15);
}


/*	初始化PWM输出，PWM输出电机MOTOR  TIM2_CH3/TIM2_CH4 */
void TIM2_PWM_Init(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	
	/* GPIOA初始化 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	/* 时基单元初始化 */ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct); // 结构体初始化，不用配置的寄存器保持默认值
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//不分频，72MHZ	
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;	// ARR值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	/* 输出比较单元初始化*/
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);	// 结构体初始化，不用配置的寄存器保持默认值
	u16 CCR3_Val = 0;
	u16 CCR4_Val = 0;
	
	/* PWM2 - TIM2_CH3	*/
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStruct.TIM_Pulse = CCR3_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
	
	TIM_OC3Init(TIM2, &TIM_OCInitStruct);	 //使能通道3
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	/* PWM1 - TIM2_CH4	*/
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = CCR4_Val;	  //设置通道2的电平跳变值，输出另外一个占空比的PWM
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

	TIM_OC4Init(TIM2, &TIM_OCInitStruct);	  //使能通道4
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);			 // 使能TIM3重载寄存器ARR

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);                   //使能定时器2	
}

/*	给TIM2_CH3、TIM2_CH3指定的CCR值	*/
void TIM2_PWM_CHANGE(u16 CCR3,u16 CCR4)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 CCR3_Val ;
	u16 CCR4_Val ;
	CCR3_Val = CCR3;
	CCR4_Val = CCR4;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 //使能通道2

	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	  //设置通道2的电平跳变值，输出另外一个占空比的PWM

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);	  //使能通道3
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
}