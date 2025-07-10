#include "hal_data.h"
#include "bsp_api.h"
#include "mk_pinctrl.h"
#include "mk_rtt.h"
#include "OLED.h"
#include "dmx_tof400c.h"
#include "string.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

// --------------------Variables--------------------
#define HIGH 1
#define LOW 0

#define LED_PIN BSP_IO_PORT_04_PIN_00 // IO3
#define BUZ_PIN BSP_IO_PORT_01_PIN_05 // IO2
#define BTN_PIN BSP_IO_PORT_01_PIN_07 // IO1
#define SDA_PIN BSP_IO_PORT_00_PIN_04
#define SCL_PIN BSP_IO_PORT_00_PIN_03
#define TTS_TX_PIN BSP_IO_PORT_06_PIN_02
#define LIGHT_CHANNEL ADC_CHANNEL_7

#define MAX_DIST 200

void setup();
void loop();
void u8_draw_loop();
unsigned int get_distance(void);
void process_distance();
uint16_t get_light();
void process_light();
void read_key();
void process_key();
void serial_daemon();

//extern unsigned char uart_receive_flag;
unsigned int distance = MAX_DIST;
uint8_t tof400c_err = 0;
extern uint8_t Serial_RxFlag; //Serial Receive
extern uint8_t Serial_RxPacket[128];
// Project
int distance_queue[5]; // Array to store distance values
int key_value = 0; // Variable to store the button state
bool is_approach = false;
int light_value = 0; // Variable to store the light level
bool led_state = false;
bool manual_mode = false;//定义接收数据包数组，数据包格式"@MSG\r\n"


void hal_entry(void)
{
    setup();
    while(1) loop();
#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

// ----------Custom functions-----------------//
void setup(void)
{
	// Peripheral Init
	if(uart6_init() != FSP_SUCCESS) while(1);
	if(uart0_init() != FSP_SUCCESS) while(1);
	if(adc_init() != FSP_SUCCESS) while(1);
    millis_tim_init(); // Init the millis timer
    gpio_init();
    // pinLevel Init
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZ_PIN, HIGH);
    digitalWrite(TTS_TX_PIN, HIGH);
    //TOF
    Init_TOF400C();
    // OLED
    OLED_Init();
    OLED_ShowString(1, 1, "Hello,");
    OLED_ShowString(2, 1, "RA6M5!");
    tts_print("欢迎使用Ra6M5");
    beep(500);
    delay(1000);
    OLED_Clear();
    digitalWrite(LED_PIN, HIGH);
}

void loop(void)
{
    static unsigned long t = 0;

    if(millis() - t >= 300)
    {
    		t = millis();
    		// Timing Function
    		process_distance();
    		process_light();
    }
    process_key();
    u8_draw_loop();
    serial_daemon();
}

void u8_draw_loop(void)
{
	OLED_ShowString(1, 1, "Light:");
	OLED_ShowNum(1, 7, light_value, 4);
	OLED_ShowString(2, 1, "Dis:");
	OLED_ShowNum(2, 5, distance, 4);
	OLED_ShowString(2, 9, "cm");
	OLED_ShowString(3, 1, "LED:");
	OLED_ShowString(3, 5, manual_mode ? "MAN," : "AUT,");
	OLED_ShowNum(3, 9, led_state, 1);
}

void serial_daemon(void)
{
	if(Serial_RxFlag)
	{
		Serial_RxFlag = 0;
		// Query
		if(strcmp(Serial_RxPacket, "QUERY") == 0)
		{
			tty6_logi("#OK\r\n");
			tts_print("网络开");
		}
		else if(strcmp(Serial_RxPacket, "NEED_CONFIG") == 0)
		{
			tts_print("请配网");
		}
		else if(strcmp(Serial_RxPacket, "CONFIG_END") == 0)
		{
			tts_print("已保存");
		}
	}
}

void read_key()
{
  static long int original_time;
  static bool is_btn_press = false;
  int long_press_time = 0;
  int t_key_value = digitalRead(BTN_PIN); // Read the button state

  if(t_key_value == 0)
  {
    delay(2);
    t_key_value = digitalRead(BTN_PIN);
    if(t_key_value == 0)
    {
      if(is_btn_press == false) original_time = millis();
      is_btn_press = true; // Button is pressed
    }
  }
  else
  {
    if(is_btn_press){
      long_press_time = millis() - original_time;
      original_time = millis();
      is_btn_press = false;
      if(long_press_time < 700) key_value = 1;
      else key_value = 2;
    }
    else{
      key_value = 0;
    }
  }
}

void process_key()
{
  static unsigned long t;
  read_key();
  if(key_value == 1)
  {
    manual_mode = !manual_mode; // Toggle manual mode
    if(manual_mode)
    {
      tts_print("指示灯手动开启");
      digitalWrite(LED_PIN, LOW); // Turn on the LED
      beep(20);
    }
    else
    {
      tts_print("指示灯自动模式");
      digitalWrite(LED_PIN, HIGH); // Turn off the LED
      beep(20);
    }
  }
  else if(key_value == 2)
  {
    beep(150);
    delay(20);
    beep(150);
    tts_print("正在启动报警");
    delay(1000);
    // Alert Func
    t = millis();
    tty6_logi("#SOS\r\n");
    while(1)
    {
    	if(Serial_RxFlag)
    	{
    		Serial_RxFlag = 0;
    		if(strcmp(Serial_RxPacket, "LOC_INVAILD") == 0)
    		{
    			tts_print("位置无效");
    			break;
    		}
    		else if(strcmp(Serial_RxPacket, "SOS_SENT") == 0)
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
      digitalWrite(LED_PIN, LOW); // Turn on the LED
      led_state = true; // Set the LED state to on
    }

    else if(light_value > 2600 && led_state == true) // If light level is greater than 1000 lux
    {
      tts_print("光线正常,警示灯已关闭");
      beep(200);
      digitalWrite(LED_PIN, HIGH); // Turn off the LED
      led_state = false; // Set the LED state to off
    }
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
