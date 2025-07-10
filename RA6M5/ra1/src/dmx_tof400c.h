/****************************************************************************************
 *     COPYRIGHT NOTICE
 *     Copyright (C) 2024,AS DAIMXA
 *     copyright Copyright (C) ������DAIMXA,2024
 *     All rights reserved.
 *     ��������QQȺ��710026750
 *
 *     ��ע�������⣬�����������ݰ�Ȩ�������������ܿƼ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�������������Ƽ��İ�Ȩ������
 *      ____    _    ___ __  ____  __    _
 *     |  _ \  / \  |_ _|  \/  \ \/ /   / \
 *     | | | |/ _ \  | || |\/| |\  /   / _ \
 *     | |_| / ___ \ | || |  | |/  \  / ___ \
 *     |____/_/   \_\___|_|  |_/_/\_\/_/   \_\
 *
 * @file       dmx_tof400c.h
 * @brief      ������CH32V307VCT6��Դ��
 * @company    �Ϸʴ��������ܿƼ����޹�˾
 * @author     �������Ƽ���QQ��2453520483��
 * @MCUcore    CH32V307VCT6
 * @Software   MounRicer Stdio V191
 * @version    �鿴˵���ĵ���version�汾˵��
 * @Taobao     https://daimxa.taobao.com/
 * @Openlib    https://gitee.com/daimxa
 * @date       2024-01-04
****************************************************************************************/

#ifndef __DMX_TOF400C_H
#define __DMX_TOF400C_H

#include "mk_pinctrl.h"

// ������Ϊ(0~1.4m)
// ���Ƶ��,��ѡ��100,66,50,30,20,10,5,2(��λHz)
#define TOF400C_FREQ                100

// TOF400C������ģ��ܽź궨��
// TOF400C,SCL��SDA�ܽ�
#define TOF400C_SCL_PIN             BSP_IO_PORT_00_PIN_03
#define TOF400C_SDA_PIN             BSP_IO_PORT_00_PIN_04
// TOF400C���ŵ�ƽ����
#define TOF400C_SCL_LEVEL(level)    digitalWrite(TOF400C_SCL_PIN, level)
#define TOF400C_SDA_LEVEL(level)    digitalWrite(TOF400C_SDA_PIN, level)
// TOF400C��ʱ
#define TOF400C_DELAY               0
#define TOF400C_DELAY_MS(time)      (delay(time))
// TOF400C��ʼ����ʱ��������
#define TOF400C_TIMEOUT_COUNT       10


/**
*
* @brief    TOF400Cģ���ʼ��
* @param    void
* @return   void
* @notes
* Example:  Init_TOF400C();
*
**/
void Init_TOF400C(void);

/**
*
* @brief    ��ȡTOF400Cģ���������
* @param    void
* @return   unsigned int            �������ľ���,��λ(mm)
* @notes
* Example:  Get_Distance_TOF400C();
*
**/
unsigned int Get_Distance_TOF400C(uint8_t* ErrValue);

// VL53L1X��ص�ַ�궨��
#define VL53L1X_DEFAULT_DEVICE_ADDRESS                          0x0052
#define VL53L1_I2C_SLAVE__DEVICE_ADDRESS                        0x0001
#define GPIO_HV_MUX__CTRL                                       0x0030
#define GPIO__TIO_HV_STATUS                                     0x0031
#define RANGE_CONFIG__TIMEOUT_MACROP_A_HI                       0x005E
#define RANGE_CONFIG__TIMEOUT_MACROP_B_HI                       0x0061
#define SYSTEM__INTERRUPT_CLEAR                                 0x0086
#define RESULT__RANGE_STATUS                                    0x0089
#define VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0   0x0096

#endif /* __DMX_TOF400C_H */
