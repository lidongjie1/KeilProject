#include "motor.h"

/*
	�˿�����
	����GPIO PB12/PB13/PB14/PB15
	PB12	- GPIO_Mode_Out_PP	-
	PB13	- GPIO_Mode_Out_PP	-
	PB14	- GPIO_Mode_Out_PP	-
	PB15	- GPIO_Mode_Out_PP	-
	PWM������MOTOR  TIM2_CH3/TIM2_CH4
	
	PA2		- TIM2_CH3
	PA3		- TIM2_CH4
*/


void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	/* ��������TB6612�������ģ���GPIO�˿�	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	/* �͵�ƽ	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14|GPIO_Pin_12 | GPIO_Pin_15);
}


/*	��ʼ��PWM�����PWM������MOTOR  TIM2_CH3/TIM2_CH4 */
void TIM2_PWM_Init(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	
	/* GPIOA��ʼ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	/* ʱ����Ԫ��ʼ�� */ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct); // �ṹ���ʼ�����������õļĴ�������Ĭ��ֵ
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//����Ƶ��72MHZ	
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;	// ARRֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	/* ����Ƚϵ�Ԫ��ʼ��*/
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);	// �ṹ���ʼ�����������õļĴ�������Ĭ��ֵ
	u16 CCR3_Val = 0;
	u16 CCR4_Val = 0;
	
	/* PWM2 - TIM2_CH3	*/
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_Pulse = CCR3_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
	
	TIM_OC3Init(TIM2, &TIM_OCInitStruct);	 //ʹ��ͨ��3
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	/* PWM1 - TIM2_CH4	*/
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = CCR4_Val;	  //����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

	TIM_OC4Init(TIM2, &TIM_OCInitStruct);	  //ʹ��ͨ��4
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);                   //ʹ�ܶ�ʱ��2	
}

/*	��TIM2_CH3��TIM2_CH3ָ����CCRֵ	*/
void TIM2_PWM_CHANGE(u16 CCR3,u16 CCR4)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 CCR3_Val ;
	u16 CCR4_Val ;
	CCR3_Val = CCR3;
	CCR4_Val = CCR4;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��2

	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	  //����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);	  //ʹ��ͨ��3
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
}