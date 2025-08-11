# ifndef __MK_PINCTRL_H__
#define __MK_PINCTRL_H__

#include "hal_data.h"
#include "bsp_api.h"

#endif

void digitalWrite(bsp_io_port_pin_t pin, uint8_t level);
uint8_t digitalRead(bsp_io_port_pin_t pin);
void gpio_init();
void delay(uint32_t ms); // Delay ms
void delayMicroseconds(uint32_t us); // Delay us
void millis_tim_init(void);
uint32_t millis(void);
void timer0_callback(timer_callback_args_t *p_args);
void beep(int time_ms);
fsp_err_t adc_init();
uint16_t analogRead(adc_channel_t channel);
fsp_err_t exti_init(void);
void exti7_callback(external_irq_callback_args_t *p_args);
void exti8_callback(external_irq_callback_args_t *p_args);
uint8_t get_key1_value();
uint8_t get_key2_value();
fsp_err_t temp_sensor_init(void);
float get_sys_temp(void);
