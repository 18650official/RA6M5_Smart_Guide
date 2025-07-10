#include "mk_pinctrl.h"

uint32_t millis_count = 0;

void gpio_init()
{
	R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
}

void digitalWrite(bsp_io_port_pin_t pin, uint8_t level)
{
    bsp_io_level_t lv;
    if(level == 1) lv = BSP_IO_LEVEL_HIGH;
    else lv = BSP_IO_LEVEL_LOW;
    R_IOPORT_PinWrite(&g_ioport_ctrl, pin, lv);
}

uint8_t digitalRead(bsp_io_port_pin_t pin)
{
    bsp_io_level_t level;
    R_IOPORT_PinRead(&g_ioport_ctrl, pin, &level);

    if (level == BSP_IO_LEVEL_LOW) {
        return 0;
    }
    else
    {
        return 1;
    }
}

void delay(uint32_t ms) // Delay ms
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}

void delayMicroseconds(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}

void millis_tim_init(void)
{
    R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_GPT_Start(&g_timer0_ctrl);
}

uint32_t millis(void)
{
    return millis_count;
}

void beep(int time_ms) // 无源蜂鸣器驱动
{
	#define BUZ_PIN BSP_IO_PORT_01_PIN_05
	int i;
	for(i=0; i<time_ms/2; i++)
	{
		digitalWrite(BUZ_PIN, 1);
		delay(1);
		digitalWrite(BUZ_PIN, 0);
		delay(1);
	}
}

fsp_err_t adc_init(void)
{
	fsp_err_t e;
    e = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    return e;
}

uint16_t analogRead(adc_channel_t channel)
{
    uint16_t result;
    // 启动一次扫描（软件触发）
    R_ADC_ScanStart(&g_adc0_ctrl);
    R_BSP_SoftwareDelay(30, BSP_DELAY_UNITS_MICROSECONDS);
    R_ADC_Read(&g_adc0_ctrl, channel, &result);

    return result;
}


void timer0_callback(timer_callback_args_t *p_args) // INT
{
    if (p_args->event == TIMER_EVENT_CREST)
    {
        millis_count += 1;
    }
}
