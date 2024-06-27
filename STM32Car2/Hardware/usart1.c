#include "stm32f10x.h"                  // Device header
#include "usart1.h"

/*
GPIO������
	PA9		-	USART1(TX)
	PA10	- 	USART1(RX)

*/


void Usart1_Init(void)
{
	/*ʹ��GPIO��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	/*GPIO�ڳ�ʼ��*/
	GPIO_InitTypeDef  GPIO_InitStuct;
	GPIO_InitStuct.GPIO_Mode = GPIO_Pin_9 ;
	GPIO_InitStuct.GPIO_Pin = GPIO_Mode_AF_PP;
	GPIO_InitStuct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStuct);
	
	GPIO_InitStuct.GPIO_Mode = GPIO_Pin_10 ;
	GPIO_InitStuct.GPIO_Pin = GPIO_Mode_IPU;	//��������
	GPIO_InitStuct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStuct);
	
	/*NVIC��ʼ��*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	/*�ж��������*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//�������ڽ������ݵ��ж�
	
	/*USART1��ʼ��*/
	USART_InitTypeDef USART_InitStuct;
	USART_InitStuct.USART_BaudRate = 115200;
	USART_InitStuct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStuct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStuct.USART_Parity = USART_Parity_No;
	USART_InitStuct.USART_StopBits = 1;
	USART_InitStuct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStuct);
	USART_Cmd(USART1,ENABLE);	
	
}


/* ����һ���ֽ� */
void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//�ȴ��������

}

/* �����ַ��� */
//����ַ���
void PrintChar(char *s)
{
	char *p;
	p=s;
	while(*p != '\0')
	{
		USART1_SendByte(*p);
		p++;
	}
}


/* �ض������ */
int fputc(int ch, FILE *f)
{
	USART1_SendByte(ch);			//��printf�ĵײ��ض����Լ��ķ����ֽں���
	return ch;
}


void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)//�Ƿ񴥷������ж�
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����ж�
	}
	
	
}


