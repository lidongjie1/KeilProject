# KeilProject
## STM32平衡小车
#### 1 材料购买
1. 材料购买前建议选择适合自己的原件引脚，比如在电机的选择上，不同厂家对于电机的排线插口不同，所以买之前规划好小车排线问题。（购买电机和扩展版时建议购买同一家，避免非必要的麻烦）
2. 本人购买的是亚博智能的相关原件
    |原件|型号|数量|
    |:-:|:-:|:-:|
    |轮子|65mm|2|
    |电机|亚博智能520编码器直流减速电机|2|
    |STM32最小系统板|STM32F103C8T6|1|
    |MPU6050模块|--|1|
    |TB6612电机驱动模块|--|1|
    |蓝牙模块|HC-05|1|
    |超声波模块|HC-SR04|1|
    |电池|一般12v即可|1|
    |电机排线|6pi|2|
    |底盘|亚克力版|2|
    |铜柱|3mm-单头六角|若干|
    |螺栓|3mm|若干|
    |扩展版（可自己设计打板）|亚博智能|1|
    |插拔式接线端子|2p-插头+插座|1|
#### 2 MPU6050模块设计
1. 软件实现IIC通讯
   1. 相关文件如下图所示
   ![I2C](https://github.com/lidongjie1/KeilProject/blob/dev-1/I2C.png)
   ```C
      /*
         端口配置
         PB8 - SCL
         PB9 - SDA
      */

      /*  
         开始条件
            SCL高电平期间，SDA从高电平切换到低电平
      */
      I2C_Start(void);

      /*     
         终止条件
            SCL高电平期间，SDA从低电平切换到高电平
      */
      void I2C_Stop(void);

      /*    
         发送ACK（应答）信号 
            SDA给低电平表示应答    
      */
      I2C_Ack(void);

      /*
         发送NACK（非应答）信号
            SDA给高电平表示非应答
      */
      I2C_NoAck(void);

      /*
         等待接收从设备发送的ACK（应答）信号
            根据读取SDA是高电平还是低电平判断是否应答
      */
      I2C_WaitAck(void);

      /*
         发送一个字节
            主机将数据放在SDA线上，在SCL的高电平期间从机读取数据
      */
      I2C_SendByte(uint8_t byte);
      
      /*
         接收一个字节
            从机将数据放到SDA线上，在SCL的高电平期间主机读取数据
      */
      I2C_ReceiveByte(void);
   ```
2. 硬件实现I2C
   1. 相关文件如下图所示
   
   ![I2C_2](https://github.com/lidongjie1/KeilProject/blob/dev-1/I2C_2.png)

   ```C
   
   ```
3. MPU6050模块
   1. 相关文件如下图所示
   
   ![MPU6050](https://github.com/lidongjie1/KeilProject/blob/dev-1/MPU6050.png)

   2. MPU6050模块解析
      1. MPU6050包含一个三轴陀螺仪，三轴加速度计，内部设有一个可扩展的数字运动处理器DMP，可以将欧拉角以四元数的形式输出。
   3. 移植DMP固件库（方便获得四元数和欧拉角）
      1. 在项目录下创建MPU6050DMP文件夹
   
      ![dmp](https://github.com/lidongjie1/KeilProject/blob/dev-1/dmp.png)
      
   4. MPU6050模块初始化
      1. 在MPU6050.c文件下进行初始化
         1. 详情初始化函数在void MPU6050_Init(void)函数


#### 3 USART模块设计
1. USART1 模块
   1. 用于和串口调试助手进行通讯
   ```c
      /*
         端口配置
            PA9	-	USART1(TX)  - GPIO_Mode_AF_PP（复用推挽输出）
	         PA10	- 	USART1(RX)  - GPIO_Mode_IPU(上拉输入)
      */
     USART_InitStuct.USART_BaudRate = 115200;   // 波特率与上位机保持一致即可

      /* 重定向输出 */
      int fputc(int ch, FILE *f)
      {
         USART1_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
         return ch;
      }
   ```
2. USART3 模块
   1. 用于与蓝牙模块通讯
   ```c
      /*
         端口配置
         PB10	-	USART3(TX)  - GPIO_Mode_AF_PP（复用推挽输出）
	      PB11	- 	USART3(RX)  - GPIO_Mode_IPU(上拉输入)
      */
      USART_InitStuct.USART_BaudRate = 9600; //波特率配置成与蓝牙模块相同
   ```
#### 4 电机驱动模块设计
1. PWM输出
2. 编码器模块