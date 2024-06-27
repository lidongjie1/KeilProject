#include "stm32f10x.h"                  // Device header
#include "I2C_MPU6050.h"


/* Ӳ��ʵ��I2C */



/* ���ó�ʼ�� */
void I2C_MPU6050_Init(void)
{
	/*ʱ��ʹ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/* ��ӳ��I2C���� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	
	/* GPIO��ʼ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//���óɸ��ÿ�©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	/* I2C1��ʼ�� */
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	//ʱ��ռ�ձ�
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_OwnAddress1 = SlaveAddress;	//�����ַ���ӻ�ģʽ�²���Ч
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1,&I2C_InitStruct);
	I2C_Cmd(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE);		// ����Ӧ��ʹ��
	
}


/* MPU6050�Ĵ���д�� */

void I2C_ByteWrite(uint8_t RegAddress, uint8_t Data)
{
	
	I2C_GenerateSTART(I2C1,ENABLE);		// ������ʼ����
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));		// ���EV5�¼�
	
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);	// ���ʹӻ���ַ
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	// ���EV6�¼�
	
	I2C_SendData(I2C1,RegAddress);												// Ҫд��ĵ�ַ
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));			// �ȴ�EV8�¼�
	
	I2C_SendData(I2C1,Data);													// ��������
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));				// �ȴ�EV8_2�¼�
	
	I2C_GenerateSTOP(I2C1,ENABLE);	// ������ֹ����
			
}


/* MPU6050�Ĵ������� */
uint8_t I2C_ByteRead(uint8_t RegAddress)
{
	uint8_t ReceiveData;
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));		// �ȴ�EV5�¼�
	
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);	// ָ����ַ�������÷���ģʽָ�����ȡ�ĵ�ַ
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	// ���EV6�¼�����ʱ�¼�Ϊ����
	
	I2C_Cmd(I2C1,ENABLE);
	I2C_SendData(I2C1,RegAddress);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1,ENABLE);								// ����������ʼ����
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);//�����豸��ַ+���źţ�����Ϊ���գ�

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(I2C1,DISABLE);

	I2C_GenerateSTOP(I2C1,ENABLE);

	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));

	ReceiveData =I2C_ReceiveData(I2C1);			//�����Ĵ�������
	I2C_AcknowledgeConfig(I2C2, ENABLE);			//��Ӧ��ָ�Ϊʹ�ܣ�Ϊ�˲�Ӱ��������ܲ����Ķ�ȡ���ֽڲ���
	
	return ReceiveData;
	
}


/* �������16λ������ */
uint32_t GetData(uint8_t RegAddress)
{
	
	uint8_t H_data,L_data;
	
	H_data = I2C_ByteRead(RegAddress);
	L_data = I2C_ByteRead(RegAddress+1);
	return (H_data<<8)+L_data;   //�ϳ�����
}

