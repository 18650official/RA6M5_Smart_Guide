#include "hal_data.h"
#include "bsp_api.h"
#include "mk_pinctrl.h"
#include "mk_rtt.h"
#include "OLED.h"
#include "dmx_tof400c.h"
#include "mpu6050.h"
#include "mk_gps.h"
#include "string.h"


FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

// --------------------Variables--------------------
#define HIGH 1
#define LOW 0

#define MAX_DIST 140

#define BOARD_OLED_SCL BSP_IO_PORT_02_PIN_06
#define BOARD_OLED_SDA BSP_IO_PORT_03_PIN_03
#define BOARD_OLED_GND BSP_IO_PORT_03_PIN_06
#define BOARD_OLED_VIN BSP_IO_PORT_04_PIN_11
#define BOARD_KEY_F1 BSP_IO_PORT_00_PIN_01
#define BOARD_KEY_F2 BSP_IO_PORT_00_PIN_02
#define BOARD_LED BSP_IO_PORT_01_PIN_14
#define EXTERN_LED BSP_IO_PORT_04_PIN_00 // GPIO3
#define EXTERN_SCL BSP_IO_PORT_00_PIN_03
#define EXTERN_SDA BSP_IO_PORT_00_PIN_04
#define BUZ_PIN BSP_IO_PORT_01_PIN_05 // IO2
#define LIGHT_CHANNEL ADC_CHANNEL_7
#define TXD0_PIN BSP_IO_PORT_01_PIN_00
#define RXD0_PIN BSP_IO_PORT_01_PIN_01


void setup();
void loop();
unsigned int get_distance();
void process_distance();
void sos_action();
void process_acc_value();
void process_key();
uint16_t get_light();
void process_light();
void u8_draw_loop();
void serial_daemon();

// Variables
// Key
uint8_t key1_value = 0;
uint8_t key2_value = 0;
// Temp
float sys_temp = 0.0f;
//MPU6050
MPU_Result_Typedef MPU_Result;
// TOF400C
uint8_t tof400c_err = 0;
int distance_queue[5]; // Array to store distance values
bool is_approach = false;
uint16_t distance = MAX_DIST;
// light sensor
int light_value = 0; // Variable to store the light level
bool led_state = false;
bool manual_mode = false;//定义接收数据包数组，数据包格式"@MSG\r\n"


// external variables
extern uint8_t Serial6_RxFlag; //Serial Receive
extern uint8_t Serial6_RxPacket[128];
extern uint8_t Serial9_RxFlag; //Serial Receive
extern uint8_t Serial9_RxPacket[512];


// program start
void hal_entry(void)
{
    setup();
    while(1) loop();
#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

// ----------Main function and main loop-----------------//
void setup(void)
{
	// Peripheral Init
	if(uart_all_init() != FSP_SUCCESS) while(1);
	if(adc_init() != FSP_SUCCESS) while(1);
	if(exti_init() != FSP_SUCCESS) while(1);
    millis_tim_init(); // Init the millis timer
    gpio_init();
    // main setup function
    //led and gpio
    digitalWrite(EXTERN_LED, LOW);
    digitalWrite(BUZ_PIN, HIGH);
    // sensor
    Init_TOF400C(); //tof400c
    delay(1);
    int r = MPU_Init(); //mpu6050
    // display
    OLED_Init();
    OLED_ShowString(1, 1, "Welcome!");
    OLED_ShowString(2, 1, "Sensor:");
    OLED_ShowNum(2, 8, (uint32_t)r, 1);
    beep(200);
    tts_print("欢迎使用智能导盲系统");
    dbg_logi("System Start!\n");
    delay(2000);
    OLED_Clear();
}

void loop(void)
{
    static unsigned long t = 0;

    if(millis() - t >= 300)
    {
    		t = millis();
    		process_light();
    		process_distance();
    		process_acc_value();
    }

    // Fast loop
    u8_draw_loop();
    process_key();
    serial_daemon();
}

void u8_draw_loop(void)
{
	OLED_ShowString(1, 1, "Light:");
	OLED_ShowNum(1, 7, (int32_t)light_value, 4);
	OLED_ShowString(2, 1, "Dis:");
	OLED_ShowNum(2, 5, distance, 4);
	OLED_ShowString(2, 9, "cm");
	OLED_ShowString(3, 1, "LED:");
	OLED_ShowString(3, 5, manual_mode ? "MAN," : "AUT,");
	OLED_ShowNum(3, 9, led_state, 1);
	OLED_ShowString(4, 1, "Temp:");
	OLED_ShowNum(4, 6, (int32_t)MPU_Result.temp, 2);
	OLED_ShowString(4, 8, ".C");
}

void process_key()
{
  key1_value = get_key1_value();
  key2_value = get_key2_value();

  if(key1_value == 1)
  {
    manual_mode = !manual_mode; // Toggle manual mode
    if(manual_mode)
    {
      tts_print("指示灯手动开启");
      digitalWrite(EXTERN_LED, LOW); // Turn on the LED
      beep(20);
    }
    else
    {
      tts_print("指示灯自动模式");
      digitalWrite(EXTERN_LED, HIGH); // Turn off the LED
      beep(20);
    }
  }
  else if(key1_value == 2)
  {
    sos_action();
  }
  // 判断按键2
  if(key2_value == 1) //播报温度
  {
	  tts_printf("当前温度");
	  delay(1700);
	  tts_printf("%d十%d", ((int)MPU_Result.temp)/10, ((int)MPU_Result.temp)%10);
	  delay(1200);
	  tts_print("摄氏度");
  }
}

unsigned int get_distance(void)
{
  // 读取TOF400C模块测距距离
  unsigned int d = Get_Distance_TOF400C(&tof400c_err);
  d /= 10;
  return d;
}

void process_distance()
{
  static int queue_index = 0;
  unsigned int d_t = get_distance(); // Update the distance value
  if(queue_index < 5)
  {
    distance_queue[queue_index] = d_t; // Store the distance in the queue
    queue_index++;
  }
  else
  {
    for(int i = 0; i < 4; i++)
    {
      distance_queue[i] = distance_queue[i + 1]; // Shift the queue
    }
    distance_queue[4] = d_t; // Add the new distance to the end of the queue
  }
  // Process the array
  if(distance_queue[3] == MAX_DIST && distance_queue[4] < 30)
  {
    //消除误差
    distance_queue[3] = distance_queue[4];
  }
  distance = (unsigned int)(distance_queue[0]*0.8 + distance_queue[1]*0.1 + distance_queue[2]*0.05 + distance_queue[3]*0.025 + distance_queue[4]*0.025); // Calculate the average distance

  if(distance < 25 && !is_approach) // If distance is less than 20 cm and not already in approach state
  {
    is_approach = true;
    tts_print("距离过近");
    beep(500);
  }
  else if(distance >= 25 && is_approach) // If distance is greater than or equal to 20 cm and in approach state
  {
    is_approach = false;
    tts_print("距离正常");
    beep(500);
  }
  //持续发出报警声
  if(is_approach)
  {
    beep(20);
  }
}

void process_acc_value(void)
{
	int i;
	unsigned long long j;
	bool acc_alert = false;
	static int count = 10;
	MPU6050_ConvertToResult(&MPU_Result);
	// 找出最大数据
	for(i=0; i<3; i++)
	{
		float r = MPU_Result.gyro[i];
		if(r<0) r = -r;
		if(r>110){
			acc_alert = true;
			break;
		}
	}
	if(acc_alert)
	{
		beep(800);
		tts_print("检测到您摔倒了，请按任意键解除报警");
		delay(3500);
		OLED_Clear();
		OLED_ShowString(1, 1, "Warning!");
		OLED_ShowNum(2, 1, (uint32_t)count, 2);
		j = millis();
		key1_value = get_key1_value();
	    key2_value = get_key2_value();
	    key1_value = 0;
	    key2_value = 0; //清空按键缓存
		while(1)
		{
			key1_value = get_key1_value();
			key2_value = get_key2_value();
			if(millis() - j >= 1000) //倒计时
			{
				count--;
				OLED_ShowNum(2, 1, (uint32_t)count, 2);
				beep(20);
				j = millis();
			}
			if(key1_value!=0 || key2_value!=0) //警报解除
			{
				count = 10;
				beep(100);
				key1_value = 0;
				key2_value = 0;
				tts_print("警报解除");
				OLED_Clear();
				delay(800);
				return;
			}
			if(count <= 0)
			{
				count = 10;
				OLED_Clear();
				sos_action();
				return;
			}
		}
	}
}

void sos_action()
{
	  	static unsigned long t;
		beep(100);
	    delay(20);
	    beep(100);
	    tts_print("正在发送位置信息");
	    OLED_Clear();
	    OLED_ShowString(1, 1, "Sending...");
	    delay(1500);
	    // Alert Func
	    t = millis();
	    tty6_logi("#SOS\r\n");
	    while(1)
	    {
	    	if(Serial6_RxFlag)
	    	{
	    		Serial6_RxFlag = 0;
	    		if(strcmp(Serial6_RxPacket, "LOC_INVAILD") == 0)
	    		{
	    			tts_print("位置无效");
	    			break;
	    		}
	    		else if(strcmp(Serial6_RxPacket, "SOS_SENT") == 0)
	    		{
	    			tts_print("已发送");
	    			break;
	    		}
	    	}
	    	else if(millis() - t > 40000)
	    	{
	    		tty6_logi("#SOS\r\n"); //再次尝试
	    		tts_print("网络超时");
	    		break;
	    	}
	}
	OLED_Clear();
}


uint16_t get_light()
{
  uint16_t lux = 4096 - analogRead(LIGHT_CHANNEL); // Read the light level
  return lux; // Return the light level
}

void process_light()
{
  // This function can be used to process the light level if needed
  // Currently, it just returns the light value read from the sensor
  light_value = get_light(); // Get the light level
  if(!manual_mode)
  {
    if(light_value < 2300 && led_state == false) // If light level is less than 100 lux
    {
      tts_print("光线过暗,警示灯已开启");
      beep(200);
      digitalWrite(EXTERN_LED, LOW); // Turn on the LED
      led_state = true; // Set the LED state to on
    }

    else if(light_value > 2600 && led_state == true) // If light level is greater than 1000 lux
    {
      tts_print("光线正常,警示灯已关闭");
      beep(200);
      digitalWrite(EXTERN_LED, HIGH); // Turn off the LED
      led_state = false; // Set the LED state to off
    }
  }
}

void serial_daemon(void)
{
	if(Serial6_RxFlag)
	{
		Serial6_RxFlag = 0;
		// Query
		if(strcmp(Serial6_RxPacket, "QUERY") == 0)
		{
			tty6_logi("#OK\r\n");
			tts_print("网络已启动");
		}
		else if(strcmp(Serial6_RxPacket, "NEED_CONFIG") == 0)
		{
			tts_print("请配置网络");
		}
		else if(strcmp(Serial6_RxPacket, "WL_CONNECTED") == 0)
				{
					tts_print("连接成功");
				}
		else if(strcmp(Serial6_RxPacket, "CONFIG_END") == 0)
		{
			tts_print("密码已保存");
		}
	}
	if(Serial9_RxFlag)
	{
    		Serial9_RxFlag = 0;
#ifdef GPS_DEBUG
    		dbg_logi("GPS Received:%s\n", Serial9_RxPacket);
#endif
    }
}







// ---------------Custom functions End-------------------- //












/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);

#if BSP_CFG_SDRAM_ENABLED

        /* Setup SDRAM and initialize it. Must configure pins first. */
        R_BSP_SdramInit(true);
#endif
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif
