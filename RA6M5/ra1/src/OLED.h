/*
 * oled_iic.h
 *
 *  Created on: 2024年3月30日
 *      Author: 22967
 */

#ifndef OLED_H_
#define OLED_H_

#include "hal_data.h"

#define SDA_PIN BSP_IO_PORT_00_PIN_04
#define SCL_PIN BSP_IO_PORT_00_PIN_03

#define OLED_W_SCL(x)  R_IOPORT_PinWrite(&g_ioport_ctrl, SCL_PIN, (bsp_io_level_t)(x));
#define OLED_W_SDA(x)  R_IOPORT_PinWrite(&g_ioport_ctrl, SDA_PIN, (bsp_io_level_t)(x));


void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_SetCursor(uint8_t Y, uint8_t X);

uint8_t SDA_READ(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Read_Byte(unsigned char ack);
void OLED_WriteCommand(unsigned char IIC_Command);
void OLED_WriteData(unsigned char IIC_Data);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t data);


#endif /* OLED_OLED_IIC_H_ */
