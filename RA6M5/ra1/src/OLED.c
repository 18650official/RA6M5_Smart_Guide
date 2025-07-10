/*
 * oled_iic.c
 *
 *  Created on: 2024年3月30日
 *      Author: 22967
 */
#include "OLED.h"
#include "OLED_Font.h"

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
}

uint8_t SDA_READ(void)
{
    bsp_io_level_t state;
    R_IOPORT_PinRead(&g_ioport_ctrl, SDA_PIN, &state);
    return state;
}

/**
  *  功能：IIC开始信号 SCL保持高电平，SDA从高电平跳变到低电平
  *  入口参数：无
  *  返回值：无
  */
 void IIC_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    R_BSP_SoftwareDelay(6,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    R_BSP_SoftwareDelay(6,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    OLED_W_SCL(0);
}

 /**
   *  功能：IIC结束信号 SCL保持高电平，SDA从低电平跳变到高电平
   *  入口参数：无
   *  返回值：无
   */
void IIC_Stop(void)
{
    OLED_W_SDA(0);
    R_BSP_SoftwareDelay(5,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    R_BSP_SoftwareDelay(5,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SDA(1);
    R_BSP_SoftwareDelay(5,BSP_DELAY_UNITS_MICROSECONDS);
}

/**
  *  功能：等待应答信号ACK
  *  入口参数：无
  *  返回值：0，接受应答成功；1，接受应答失败
  */
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    OLED_W_SDA(1);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    while(SDA_READ())
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    OLED_W_SCL(0);
    return 0;
}

/**
  *  功能：产生ACK应答
  *  入口参数：无
  *  返回值：无
  */
void IIC_Ack(void)
{
    OLED_W_SCL(0);
    OLED_W_SDA(0);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(0);
}

/**
  *  功能：产生非ACK应答
  *  入口参数：无
  *  返回值：无
  */
void IIC_NAck(void)
{
    OLED_W_SCL(0);
    OLED_W_SDA(1);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(1);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_W_SCL(0);
}

/**
  *  功能：IIC发送一个字节（8 bit）
  *  入口参数：无
  *  返回值：返回从机有无应答。0，无应答；1，有应答
  */
void IIC_Send_Byte(uint8_t data)
{
    uint8_t t;
    OLED_W_SCL(0);
    for(t=0;t<8;t++)
    {
        if((data & 0x80)>>7)
        {
               OLED_W_SDA(1);
        }
        else
             OLED_W_SDA(0);
        data<<=1;
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MICROSECONDS);
        OLED_W_SCL(1);
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MICROSECONDS);
        OLED_W_SCL(0);
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MICROSECONDS);
    }

}

uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    for(i=0;i<8;i++ )
    {
        OLED_W_SCL(0);
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MICROSECONDS);
        OLED_W_SCL(1);
        receive<<=1;
        if(SDA_READ())receive++;
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MICROSECONDS);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

void OLED_WriteCommand(unsigned char IIC_Command)
{
    IIC_Start();
    IIC_Send_Byte(0x78);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack();
    IIC_Send_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_WriteData(unsigned char IIC_Data)
{
    IIC_Start();
    IIC_Send_Byte(0x78);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);
    IIC_Wait_Ack();
    IIC_Send_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                    //设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    //设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));           //设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);       //设置光标位置在上半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);          //显示上半部分内容
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);   //设置光标位置在下半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);      //显示下半部分内容
    }
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{

    uint32_t i, j;
    R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MILLISECONDS);//上电延时200ms

    OLED_I2C_Init();            //端口初始化

    OLED_WriteCommand(0xAE);    //关闭显示

    OLED_WriteCommand(0xD5);    //设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);    //设置多路复用率
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);    //设置显示偏移
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);    //设置显示开始行

    OLED_WriteCommand(0xA1);    //设置左右方向，0xA1正常 0xA0左右反置

    OLED_WriteCommand(0xC8);    //设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteCommand(0xDA);    //设置COM引脚硬件配置
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);    //设置对比度控制
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);    //设置预充电周期
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);    //设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);    //设置整个显示打开/关闭

    OLED_WriteCommand(0xA6);    //设置正常/倒转显示

    OLED_WriteCommand(0x8D);    //设置充电泵
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);    //开启显示

    OLED_Clear();               //OLED清屏
}
