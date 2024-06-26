#include "stm32f10x.h"                  // Device header
#include <stdio.h>
/*
GPIO������
	PB10	-	USART3(TX)
	PB11	- 	USART3(RX)

*/


void Usart3_Init(void)
{
	/*ʹ��GPIO��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	/*GPIO�ڳ�ʼ��*/
	GPIO_InitTypeDef  GPIO_InitStuct;
	GPIO_InitStuct.GPIO_Mode = GPIO_Pin_10 ;
	GPIO_InitStuct.GPIO_Pin = GPIO_Mode_AF_PP;
	GPIO_InitStuct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStuct);
	
	GPIO_InitStuct.GPIO_Mode = GPIO_Pin_11 ;
	GPIO_InitStuct.GPIO_Pin = GPIO_Mode_IPU;	//��������
	GPIO_InitStuct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStuct);
	
	/*NVIC��ʼ��*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	/*�ж��������*/
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			//�������ڽ������ݵ��ж�
	
	/*USART1��ʼ��*/
	USART_InitTypeDef USART_InitStuct;
	USART_InitStuct.USART_BaudRate = 9600;
	USART_InitStuct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStuct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStuct.USART_Parity = USART_Parity_No;
	USART_InitStuct.USART_StopBits = 1;
	USART_InitStuct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStuct);
	USART_Cmd(USART3,ENABLE);	
	
}


/* ����һ���ֽ� */
void USART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//�ȴ��������

}

/* �����ַ��� */
void USART3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//�����ַ����飨�ַ������������ַ���������־λ��ֹͣ
	{
		USART3_SendByte(String[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}


///* �ض������ */
//int fputc(int ch, FILE *f)
//{
//	USART3_SendByte(ch);			//��printf�ĵײ��ض����Լ��ķ����ֽں���
//	return ch;
//}


void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET)//�Ƿ񴥷������ж�
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//����ж�
	}
	
	
}


