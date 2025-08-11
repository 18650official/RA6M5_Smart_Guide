#include "mk_pinctrl.h"

uint32_t millis_count = 0;
uint8_t user_key1_value;
uint8_t user_key2_value;

#define KEY1 BSP_IO_PORT_00_PIN_01
#define KEY2 BSP_IO_PORT_00_PIN_02

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

fsp_err_t temp_sensor_init(void)
{
	fsp_err_t e;
		e = R_ADC_Open(&g_adc_temp_ctrl, &g_adc_temp_cfg);
	    assert(FSP_SUCCESS == e);
	    // 启用温度传感器输入（内部电路）
	    e = R_ADC_ScanCfg(&g_adc_temp_ctrl, &g_adc_temp_channel_cfg);
	    assert(FSP_SUCCESS == e);
	    return e;
}

// exti
fsp_err_t exti_init(void)
{
	fsp_err_t e;
	 /* 初始化外部中断7 */
	    R_ICU_ExternalIrqOpen(&g_external_irq7_ctrl, &g_external_irq7_cfg);
	    e = R_ICU_ExternalIrqEnable(&g_external_irq7_ctrl);
	    /* 初始化外部中断8 */
	    R_ICU_ExternalIrqOpen(&g_external_irq8_ctrl, &g_external_irq8_cfg);
	    e = R_ICU_ExternalIrqEnable(&g_external_irq8_ctrl);
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

void exti7_callback(external_irq_callback_args_t *p_args)
{
	static unsigned long press_time;
	unsigned long duration;
	if (p_args->channel == 7)
	{
		if(digitalRead(KEY1) == 0)
		{
			press_time = millis();
		}
		else if(digitalRead(KEY1) == 1)
		{
			duration = millis() - press_time;
			if(duration > 600) user_key1_value = 2;
			else if(duration < 1) user_key1_value = 0;
			else user_key1_value = 1;
		}
	}
}

void exti8_callback(external_irq_callback_args_t *p_args)
{
	static unsigned long press_time;
	unsigned long duration;
	if (p_args->channel == 8)
	{
		if(digitalRead(KEY2) == 0)
		{
			press_time = millis();
		}
		else if(digitalRead(KEY2) == 1)
		{
			duration = millis() - press_time;
			if(duration > 600) user_key2_value = 2;
			else if(duration < 1) user_key2_value = 0;
			else user_key2_value = 1;
		}
	}
}

uint8_t get_key1_value(void)
{
	uint8_t v = user_key1_value;
	user_key1_value = 0;
	return v;
}

uint8_t get_key2_value(void)
{
	uint8_t v = user_key2_value;
	user_key2_value = 0;
	return v;
}

float get_sys_temp(void)
{
	uint16_t adc_result = 0;
	float voltage, temperature;

	R_ADC_ScanStart(&g_adc_temp_ctrl);
	R_ADC_Read(&g_adc_temp_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_result);

	// 22 1.2408
	// 25 1.4300
	// k=0.063 t = 0.063*(t-1.24)+22

	voltage = (adc_result / 4095.0f) * 3.3f;
	temperature = (voltage - 1.2408f) * 200.0f + 22.0f;

	return temperature;
}

