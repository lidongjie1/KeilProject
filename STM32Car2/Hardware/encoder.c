#include "encoder.h"

/*
  �������� TIM3_CH1/TIM3_CH2
	TIM3_CH1	- PA6
	TIM3_CH2	- PA7
  �������� TIM4_CH1/TIM4_CH2
    TIM4_CH1	- PB6	(������I2C1_SCL��ͻ����Ӳ��ʵ��I2Cʱ�������������ӳ��)
	TIM4_CH2	- PB7	(������I2C1_SDA��ͻ����Ӳ��ʵ��I2Cʱ�������������ӳ��)

*/



void TIM3_Encoder_Init(void)
{
	/* TIM3�� GPIOA ʱ��ʹ��  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* GPIO��ʼ��	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* NVIC��ʼ��*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //���ȼ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʱ����Ԫ��ʼ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	//�ṹ���ʼ��
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //TIM_ICPolarity_Rising�����ز���
	
	/* ����Ƚϳ�ʼ��*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6; //���˲���
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);	//���TIM3��ʱ���ĸ��±�־λ����ʹ��ʱ���ܹ���������������������һ���ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	  //ʹ���ж�
	TIM_Cmd(TIM3, ENABLE);  	   //ʹ�ܶ�ʱ��3
	 
}





void TIM4_Encoder_Init(void)
{
	/* TIM4�� GPIOB ʱ��ʹ��  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	/* GPIO��ʼ��	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* NVIC��ʼ��*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //���ȼ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ʱ����Ԫ��ʼ��*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //����ʱ�ӷ�Ƶϵ��������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge); //TIM_ICPolarity_Rising�����ز���


	/* ����Ƚϳ�ʼ��*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6; //���˲���
	TIM_ICInit(TIM4, &TIM_ICInitStructure);


	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	  //ʹ���ж�


	TIM_Cmd(TIM4, ENABLE);  	   //ʹ�ܶ�ʱ��3
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
        // �����ж��¼�
        
        // ����жϱ�־λ
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}


void TIM4_IRQHandler(void)
{ 		    		  			    
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        // �����ж��¼�
        
        // ����жϱ�־λ
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}