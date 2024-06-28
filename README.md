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
      /*
         I2C1引脚重映射
            PB8   - I2C1_SCL  - GPIO配置成复用开漏输出
            PB9   - I2C1_SDA  - GPIO配置成复用开漏输出
      */

      /* 
      MPU6050寄存器写入 
         1.产生起始条件，等待EV5事件
         2.发送模式指定其读取的地址，等待EV6事件
         3.发送数据，等待EV8事件
         4.结束，等待EV8_2事件
         5. 产生终止条件
      */
      void I2C_ByteWrite(uint8_t RegAddress, uint8_t Data) 

      /* 
      MPU6050寄存器读入 
         1.产生起始条件，等待EV5事件
         2.发送模式指定其读取的地址，等待EV6事件
         3.重新产生起始条件，等待EV5事件
         4.发送设备地址+读信号（方向为接收）
         5.提前产生终止条件
         6.接收数据，同时等待EV7事件
      */
      uint8_t I2C_ByteRead(uint8_t RegAddress)
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
1. PWM(脉冲宽度调制)输出
   
![pwm](https://github.com/lidongjie1/KeilProject/blob/dev-1/pwm.png)


   1. 根据占空比的值调节电机的转速
   ```c
      /*
      控制GPIO PB12/PB13/PB14/PB15
      PB12	- GPIO_Mode_Out_PP	-
      PB13	- GPIO_Mode_Out_PP	-
      PB14	- GPIO_Mode_Out_PP	-
      PB15	- GPIO_Mode_Out_PP	-  
      */
      void Motor_Init(void)

      /*	
      初始化PWM输出，PWM输出电机MOTOR  TIM2_CH3/TIM2_CH4 
         PWM2 - TIM2_CH3
         PWM1 - TIM2_CH4
      */
      void TIM2_PWM_Init(void)
   ```
2. 编码器模块
   1. 编码器用于测量电机旋转的角度和速度，从而提供反馈信号，帮助平衡车实现精确的控制和平衡。
   2. 过对编码器输出脉冲信号进行计数，可以计算出电机的转速
   ```c
         /*
         光电编码器 TIM3_CH1/TIM3_CH2
            TIM3_CH1	- PA6
            TIM3_CH2	- PA7
         光电编码器 TIM4_CH1/TIM4_CH2
            TIM4_CH1	- PB6	(这里与I2C1_SCL冲突，在硬件实现I2C时候采用了引脚重映射)
            TIM4_CH2	- PB7	(这里与I2C1_SDA冲突，在硬件实现I2C时候采用了引脚重映射)
         */

        /* 初始化TIM3时基单元、TIM3_IRQn中断、输入比较模块 */
        void TIM3_Encoder_Init(void)

        /*初始化TIM4时基单元、TIM4_IRQn中断、输入比较模块 */
        void TIM4_Encoder_Init(void)
   ```

#### 5 超声波模块
1. HC-SR04工作原理
   1. 当持续时间至少为10 µS（10微秒）的脉冲施加到触发引脚时，传感器以40KHz发射八个脉冲的声音脉冲。
   2. 如果这些脉冲没有被反射回来，则回波信号将在38毫秒（38毫秒）后超时并返回低电平。因此38 ms的脉冲表示在传感器范围内没有阻塞。
   3. 如果这些脉冲被反射回去，则在收到信号后，Echo引脚就会变低。这会产生一个脉冲，其宽度在150 µS至25 mS之间变化，具体取决于接收信号所花费时间。
   4. 通过距离-速度-时间方程，将接收到的脉冲的宽度用于计算到反射物体的距离。
   5. 测试距离=(高电平时间*声速(340M/S))/2;
2. 相关代码说明
   ```c
   /*
      PB0  - TRIG      
      PB1  - ECHO   
      1.将外部中断线与PB1进行关联
      2.初始化TIM1定时器，用于记录超声波返回的高电平时间
      3.接收到反射超声波时，触发外部中断，在外部中断内统计高电平持续时间
   */
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
         ```
#### 6 蓝牙模块
1. 引脚说明
   1. VCC：这个引脚用于连接模块的电源正极，通常接5V的电源
   2. GND：这个引脚用于连接模块的电源地，也就是电源的负极。
   3. TXD：这个引脚是串口发送数据引脚，用于将数据从HC-05模块发送到其他设备或主控制器。通常连接到主控制器的RX（接收）引脚。
   4. RXD：这个引脚是串口接收数据引脚，用于接收其他设备或主控制器发送的数据。通常连接到主控制器的TX（发送）引脚
   5. EN（或KEY）：这个引脚是模块的使能引脚。通过对此引脚的电平控制，可以实现模块的启用和禁用。
   6. STATE：这个引脚是模块的状态引脚，用于指示模块的工作状态。通常将STATE引脚连接到主控制器的数字引脚，可以通过读取引脚的电平来获取模块的状态信息，比如连接状态、工作模式等。  连接时输出高电平，断开时输出低电平
2. 蓝牙模块调试
   1. HC-05蓝牙串口通讯模块具有两种工作模式：命令响应工作模式和自动连接工作模式。在自动连接工作模式下模块又可分为主、从和回环三种工作角色。
      1. 当模块处于自动连接工作模式时，将自动根据事先设定的方式连接的数据传输；
      2. 当模块处于命令响应工作模式时能执行AT命令，用户可向模块发送各种AT 指令，为模块设定控制参数或发布控制命令。
   2. 进入命令响应工作模式
      1. 模块上电，未配对情况下就是AT模式，波特率为模块本身的波特率，默认：9600，发送一次AT指令时需要置高一次PIO11；
      2. PIO11 置高电平后，再给模块上电，此时模块进入AT 模式，波特率固定为：38400，可以直接发送AT指令。
   3. 什么叫做置高一次PIO11
      1. 在蓝牙模块中有一个小按键，按一下就置高一次PIO11。也就是说，第一种方法需要每发送一次AT指令按一次；而第二种方式是长按的过程中上电，之后就无需再管了，直接发送AT命令即可。
      2. 在蓝牙模块上有灯，当灯快闪的时候，就是自动连接工作模式；当灯慢闪的时候，就是命令响应工作模式。
3. AT命令
   1. AT指令不区分大小写，均以回车、换行结尾。
   
|AT命令|响应|参数|含义|
|:---:|:---:|:---:|:---:|
|AT|OK|None|测试指令|
|AT+RESET|OK|None|模块复位|
|AT+ORGL|OK|None|恢复默认|
|AT+ADDR?|+ADDR:< Param > OK|Param：模块蓝牙地址|获得蓝牙模块地址|
|AT+NAME=< Param >|OK|Param：模块蓝牙地址|设置设备名称|
|AT+PSWD?|OK|None|获得模块密码|
|AT+PSWD=< Param >|OK|Param：模块蓝牙地址|设置模块密码|
|AT+UART=< Param1 >,< Param2 >,< Param3 >|OK|None|设置串口参数|
|AT+UART?|+UART:< Param1 >,< Param2 >,< Param3 > OK|Param1：波特率 Param2：停止位 Param3：检验位|获得串口参数|
|AT+CMODE=Param|OK|Param:0——指定蓝牙地址连接模式（指定蓝牙地址由绑定指令设置）1——任意蓝牙地址连接模式（不受绑定指令设置地址的约束）2——回环角色（Slave-Loop）默认连接模式：0|设置模块模式|
|AT+ROLE?|+ROLE:Param OK|Param：参数取值如下：0——从角色（Slave）1——主角色（Master）2——回环角色（Slave-Loop）默认值：0|获取当前模式|

4. 相关代码说明


#### 7 平衡车控制模块
1. PID算法
   1. P:proportion，比例，就是用户期望值，与传感器返回值的差值，偏差。
   2. I：integral，积分，记录了从某一时刻开始，所有的偏差的累积。
   3. D：derivative，微分，是偏差的偏差，相当于偏差的导数，偏差变化的速率。

![](https://pic3.zhimg.com/80/v2-3d374655a126485563bc1068cee267fa_1440w.jpg)

连续公式
![](https://pic1.zhimg.com/80/v2-62c56b1eb4c053d58894740a8ce250a0_1440w.jpg)

离散公式
![](https://pic1.zhimg.com/80/v2-25965fdad08db6f281dee7e0903229f0_1440w.webp)

   4. 比例控制算法
      1. 会有稳态误差，无论Kp的值取多少，都不能达到预期目标值
   5. 积分控制算法
      1. 在比例控制算法的基础上加入积分控制算法，可以消除稳态误差
      2. 即在比例控制处于稳定值的时候，通过积分控制加大输入
   6. 微分控制算法
      1. 就是误差的倒数，变化速率，一般是负数
      2. 微分控制用于减少震荡
2. 卡兹曼滤波
   1. 卡尔曼滤波算法是一种基于统计学原理的滤波算法，能够准确估计系统的状态，并能够处理测量噪声和过程噪声的影响。在平衡车中，卡尔曼滤波算法被用来对加速度计和陀螺仪的测量数据进行滤波和融合，从而实现对平衡车姿态的准确估计。
   2. 陀螺仪具精确但有零点漂移特性，其测量误差会随着时间的累加而不断的累积，从而影响测量精度。因此，短时间测量应信任陀螺仪。加速度计一直受到平衡车振动的影响，混叠额外的高频振动量干扰，但是漂移小。因此，长时间测量应信任加速度计。所以单一的传感器测量难以得到精确的姿态角度。需采用多传感器信号融合的方法，来获得准确的姿态角度量。
3. 相关模块代码
   1. 角度环控制函数
   ```C
   
   ```
   2. 速度环控制函数
   ```C
   
   ```
   3. 电机控制函数
   ```c
   
   ```
   4. 



#### 8代码启动流程