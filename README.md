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
1. 移植DMP固件库（方便获得四元数和欧拉角）
   1. 在项目录下创建MPU6050DMP文件夹
   ![dmp](https://github.com/lidongjie1/KeilProject/blob/dev-1/dmp.png)
2. 软件实现IIC通讯
   1. 相关文件如下图所示
   ![I2C](https://github.com/lidongjie1/KeilProject/blob/dev-1/I2C.png)
   ```C
      /*
         端口配置
         PB8 - SCL
         PB9 - SDA
      */

      /*          */
      I2C_Start(void);

      /*          */
      void I2C_Stop(void);

      /*          */
      I2C_Ack(void);

      /*          */
      I2C_NoAck(void);

      /*          */
      I2C_WaitAck(void);

      /*          */
      I2C_SendByte(uint8_t byte);
      
      /*          */
      I2C_ReceiveByte(void);

   ```
3. MPU6050模块