#include "encoder.h"

/*
  光电编码器 TIM3_CH1/TIM3_CH2
	TIM3_CH1	- PA6
	TIM3_CH2	- PA7
  光电编码器 TIM4_CH1/TIM4_CH2
    TIM4_CH1	- PB6	(这里与I2C1_SCL冲突，在硬件实现I2C时候采用了引脚重映射)
	TIM4_CH2	- PB7	(这里与I2C1_SDA冲突，在硬件实现I2C时候采用了引脚重映射)

*/



void TIM3_Encoder_Init(void)
{
	/* TIM3、 GPIOA 时钟使能  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* GPIO初始化	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* NVIC初始化*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //优先级组别
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	//结构体初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //TIM_ICPolarity_Rising上升沿捕获
	
	/* 输入比较初始化*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);	//清除TIM3定时器的更新标志位，以使定时器能够正常继续计数并触发下一次中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	  //使能中断
	TIM_Cmd(TIM3, ENABLE);  	   //使能定时器3
	 
}





void TIM4_Encoder_Init(void)
{
	/* TIM4、 GPIOB 时钟使能  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	/* GPIO初始化	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* NVIC初始化*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //优先级组别
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 时基单元初始化*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //TIM_ICPolarity_Rising上升沿捕获


	/* 输入比较初始化*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6; //无滤波器
	TIM_ICInit(TIM4, &TIM_ICInitStructure);


	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	  //使能中断


	TIM_Cmd(TIM4, ENABLE);  	   //使能定时器3
}


void Encoder_Init(void)
{
	TIM3_Encoder_Init();
	
	TIM4_Encoder_Init();
	
}

void TIM3_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        // 处理中断事件
        
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}


void TIM4_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        // 处理中断事件
        
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}