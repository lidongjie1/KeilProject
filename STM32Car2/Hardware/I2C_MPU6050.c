#include "stm32f10x.h"                  // Device header
#include "I2C_MPU6050.h"


/* 硬件实现I2C */



/* 配置初始化 */
void I2C_MPU6050_Init(void)
{
	/*时钟使能*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/* 重映射I2C引脚 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	
	/* GPIO初始化 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//配置成复用开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	/* I2C1初始化 */
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	//时钟占空比
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_OwnAddress1 = SlaveAddress;	//自身地址，从机模式下才有效
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1,&I2C_InitStruct);
	I2C_Cmd(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE);		// 允许应答使能
	
}


/* MPU6050寄存器写入 */

void I2C_ByteWrite(uint8_t RegAddress, uint8_t Data)
{
	
	I2C_GenerateSTART(I2C1,ENABLE);		// 产生起始条件
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));		// 检查EV5事件
	
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);	// 发送从机地址
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	// 检查EV6事件
	
	I2C_SendData(I2C1,RegAddress);												// 要写入的地址
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));			// 等待EV8事件
	
	I2C_SendData(I2C1,Data);													// 发送数据
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));				// 等待EV8_2事件
	
	I2C_GenerateSTOP(I2C1,ENABLE);	// 产生终止条件
			
}


/* MPU6050寄存器读入 */
uint8_t I2C_ByteRead(uint8_t RegAddress)
{
	uint8_t ReceiveData;
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));		// 等待EV5事件
	
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);	// 指定地址读，先用发送模式指定其读取的地址
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	// 检查EV6事件，此时事件为发送
	
	I2C_Cmd(I2C1,ENABLE);
	I2C_SendData(I2C1,RegAddress);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1,ENABLE);								// 重新生成起始条件
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);//发送设备地址+读信号（方向为接收）

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(I2C1,DISABLE);

	I2C_GenerateSTOP(I2C1,ENABLE);

	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));

	ReceiveData =I2C_ReceiveData(I2C1);			//读出寄存器数据
	I2C_AcknowledgeConfig(I2C2, ENABLE);			//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
	
	return ReceiveData;
	
}


/* 获得连续16位置数据 */
uint32_t GetData(uint8_t RegAddress)
{
	
	uint8_t H_data,L_data;
	
	H_data = I2C_ByteRead(RegAddress);
	L_data = I2C_ByteRead(RegAddress+1);
	return (H_data<<8)+L_data;   //合成数据
}

