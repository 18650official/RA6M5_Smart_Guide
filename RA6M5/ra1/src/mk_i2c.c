#include "mk_i2c.h"

#define I2C_TIMEOUT_TIMES 100	//超时倍数

void i2c_timeout_delay();
void i2c_delay();
unsigned char I2C_SDA_Read();

//延时 用于等待应答时的超时判断 移植时需修改
void i2c_timeout_delay(void)
{
	delayMicroseconds(1);
}

void i2c_delay()	//每步的间隔 用于等待电平稳定和控制通讯速率
{
	delayMicroseconds(1);
}

//读取SDA 移植时需修改
uint8_t I2C_SDA_Read(void)
{
	return (uint8_t)digitalRead(SW_SDA_PIN);
}

/*******************************************************************************
* 函 数 名       : i2c_start
* 函数功能		 : 产生I2C起始信号
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void i2c_start(void)
{
	I2C_SDA_H();
    I2C_SCL_H();
    i2c_delay();

    I2C_SDA_L();	//当SCL为高电平时，SDA由高变为低
    i2c_delay();
    I2C_SCL_L();	//钳住I2C总线，准备发送或接收数据
}

/*******************************************************************************
* 函 数 名         : i2c_stop
* 函数功能		   : 产生I2C停止信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_stop(void)
{
	I2C_SDA_L();
    I2C_SCL_H();
    i2c_delay();

    I2C_SDA_H();	//当SCL为高电平时，SDA由低变为高
    i2c_delay();
}

/*******************************************************************************
* 函 数 名         : i2c_ack
* 函数功能		   : 产生ACK应答
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_ack(void)
{
    I2C_SCL_L();
    I2C_SDA_L();	//SDA为低电平
    i2c_delay();

    I2C_SCL_H();
    i2c_delay();
    I2C_SCL_L();
	I2C_SDA_H();
}

/*******************************************************************************
* 函 数 名         : i2c_nack
* 函数功能		   : 产生NACK非应答
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void i2c_nack(void)
{
    I2C_SCL_L();
    I2C_SDA_H();	//SDA为高电平
    i2c_delay();

    I2C_SCL_H();
    i2c_delay();
    I2C_SCL_L();
}

/*******************************************************************************
* 函 数 名         : i2c_wait_ack
* 函数功能		   : 等待应答信号到来
* 输    入         : 无
* 输    出         : 1，接收应答失败
        			 0，接收应答成功
*******************************************************************************/
uint8_t i2c_wait_ack(void)
{
    uint16_t time_temp = 0;

    I2C_SCL_H();
    i2c_delay();
    while(I2C_SDA_Read())				//等待SDA为低电平
    {
        time_temp++;
		i2c_timeout_delay();
        if(time_temp > I2C_TIMEOUT_TIMES)	//超时则强制结束I2C通信
        {
            i2c_stop();
            return 1;
        }
    }
    I2C_SCL_L();
    return 0;
}

/*******************************************************************************
* 函 数 名         : i2c_write_byte
* 函数功能		   : I2C发送一个字节
* 输    入         : dat：发送一个字节
* 输    出         : 无
*******************************************************************************/
void i2c_write_byte(uint8_t dat)
{
    uint8_t i = 0;

    I2C_SCL_L();
    for(i = 0; i<8; i++)	//循环8次将一个字节传出，先传高再传低位
    {
        if((dat & 0x80) > 0)
            I2C_SDA_H();
        else
            I2C_SDA_L();
        dat <<= 1;
        i2c_delay();
        I2C_SCL_H();
        i2c_delay();
        I2C_SCL_L();
        i2c_delay();
    }
}

/*******************************************************************************
* 函 数 名         : i2c_read_byte
* 函数功能		   : I2C读一个字节
* 输    入         : ack = 1时，发送ACK，ack = 0，发送nACK
* 输    出         : 应答或非应答
*******************************************************************************/
uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t i = 0, receive = 0;

    for(i = 0; i < 8; i++ )	//循环8次将一个字节读出，先读高再传低位
    {
        I2C_SCL_L();
        i2c_delay();
        I2C_SCL_H();
        receive <<= 1;
        if(I2C_SDA_Read())
			receive++;
        i2c_delay();
    }
    if (!ack)
        i2c_nack();
    else
        i2c_ack();

    return receive;
}

/*******************************************************************************
* 函 数 名         : i2c_mem_write
* 函数功能		   : I2C对指定器件、指定寄存器连续写入
* 输    入         : 器件地址、器件寄存器地址、待输入数据首地址、待输入数据长度
* 输    出         : 0: 成功 1：失败
*******************************************************************************/
uint8_t i2c_mem_write(uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Len)
{
	i2c_start();
	i2c_write_byte(DevAddress << 1);
	if(i2c_wait_ack())
		return 1;
	i2c_write_byte(MemAddress);
	if(i2c_wait_ack())
		return 1;
	while(Len--)
	{
		i2c_write_byte(*pData++);
		if(i2c_wait_ack())
			return 1;
	}
	i2c_stop();
	return 0;
}

/*******************************************************************************
* 函 数 名         : i2c_mem_read
* 函数功能		   : I2C对指定器件、指定寄存器连续读取
* 输    入         : 器件地址、器件寄存器地址、数据缓冲区首地址、数据长度
* 输    出         : 0: 成功 1：失败
*******************************************************************************/
uint8_t i2c_mem_read(uint8_t DevAddress, uint8_t MemAddress, uint8_t *pBuffer, uint16_t Len)
{
    i2c_start();
	i2c_write_byte(DevAddress << 1);		//发送写命令
	if(i2c_wait_ack())
		return 1;
    i2c_write_byte(MemAddress); 			//发送字地址
	if(i2c_wait_ack())
		return 1;
	i2c_start();
	i2c_write_byte(DevAddress << 1 | 1); 	//进入接收模式
	if(i2c_wait_ack())
		return 1;
	while(Len--)
	{
		*pBuffer++ = i2c_read_byte(Len!=0);	//读取字节
	}
    i2c_stop();								//产生一个停止条件
	return 0;
}

/**写入8位寄存器的一个位。
* @参数 DevAddress	I2C从器件地址
* @参数 addr    	I2C从器件内部地址
* @参数 bitNum  	写入的比特位(0-7)
* @参数 data    	写入数据
* @返回值 返回状态 (0=成功)
*/
uint8_t i2c_write_bit(uint8_t DevAddress, uint8_t addr, uint8_t bitNum, uint8_t Data)
{
	uint8_t b;
	if (!i2c_mem_read(DevAddress, addr, &b, 1))
	{
		b = (Data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
		return i2c_mem_write(DevAddress, addr, &b, 1);	//写入数据
	}
	else
		return 1;
}

/**写入8位寄存器的多个位。
* @参数 DevAddress	I2C从器件地址
* @参数 addr     I2C从器件内部地址
* @参数 bitStart 第一位的写入位置（0-7）
* @参数 length   写的比特数(不超过8)
* @参数 Data     写入数据
* @返回值 返回状态 (0=成功)
*/
uint8_t i2c_write_bits(uint8_t DevAddress, uint8_t addr, uint8_t bitStart, uint8_t length, uint8_t Data)
{
	//      010 要写入的值
	// 76543210 比特位
	//    xxx   args: bitStart=4, length=3
	// 00011100 掩码字节
	// 10101111 原始值（样本）
	// 10100011 原始值 & ~掩码
	// 10101011 掩码 | 原始值
	uint8_t b, mask = 0;
	if (!i2c_mem_read(DevAddress, addr, &b, 1))
	{
		mask = (((1 << length) - 1) << (bitStart - length + 1));	//掩码
		Data <<= (bitStart - length + 1);	//把写入的数据移动到位
		Data &= mask;
		b &= ~(mask);
		b |= Data;

		return i2c_mem_write(DevAddress, addr, &b, 1);	//写入数据
	}
	else
		return 1;
}
/**读取一个位从8位器件的寄存器。
* @参数 DevAddress	I2C从器件地址
* @参数 addr    I2C从器件内部地址
* @参数 bitNum  位的位置来读取（0-7）
* @参数 *data   数据存储地址
* @返回值（0=成功）
*/
uint8_t i2c_read_bit(uint8_t DevAddress, uint8_t addr, uint8_t bitNum, uint8_t *Data)
{
	uint8_t b;
	if (!i2c_mem_read(DevAddress, addr, &b, 1))
	{
		*Data = b & (1 << bitNum);
		return 0;
	}
	else
	{
		return 1;
	}
}
/**读取8位寄存器的多个位。
* @参数 DevAddress	I2C从器件地址
* @参数 addr    I2C从器件内部地址
* @参数 bitStart第一位的位置读取（0-7）
* @参数 length  位读取@参数长度数（不超过8）
* @参数 *data   数据存储地址（即'101'任何bitStart位置读取将等于0X05）
* @返回值（0=成功）
*/
uint8_t i2c_read_bits(uint8_t DevAddress, uint8_t addr, uint8_t bitStart, uint8_t length, uint8_t *Data)
{
	// 01101001 读取字节
	// 76543210 比特位
	//    xxx   args: bitStart=4, length=3
	//    010   masked
	//   -> 010 shifted
	uint8_t b, mask = 0;
	if (!i2c_mem_read(DevAddress, addr, &b, 1))
	{

		mask = ((1 << length) - 1) << (bitStart - length + 1);
		b &= mask;
		b >>= (bitStart - length + 1);
		*Data = b;
		return 0;
	}
	else
		return 1;
}
