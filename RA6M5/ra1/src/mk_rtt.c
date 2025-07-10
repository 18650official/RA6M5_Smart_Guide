#include "mk_rtt.h"
#include "SEGGER_RTT.h"
#include "hal_data.h"
#include "mk_pinctrl.h"

char Serial_RxPacket[128];				//定义接收数据包数组，数据包格式"@MSG\r\n"
uint8_t Serial_RxFlag = 0;					//定义接收数据包标志位

volatile bool g_uart0_tx_busy = false;
volatile bool g_uart6_tx_busy = false;

void s_delay_us(uint32_t us);

// Send data by JTAG-RTT
void dbg_logi(const char *fmt, ...)
{
    char buffer[128];  // 输出缓冲区
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    SEGGER_RTT_WriteString(0, buffer);  // RTT 通道 0 输出
}

void tty6_logi(const char *fmt, ...)
{
    char buffer[128];  // 输出缓冲区
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    uart6_send_str(buffer);
}

void tts_print(char *str)
{
    while (*str)
    {
    		g_uart0_tx_busy = true;
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)str, 1);  // 逐字节发送
        while (g_uart0_tx_busy);  // 等待发送完成标志
        str++;
    }
}

void uart6_send_str(char *str)
{
    while (*str)
    {
    		g_uart6_tx_busy = true;
        R_SCI_UART_Write(&g_uart6_ctrl, (uint8_t *)str, 1);  // 逐字节发送
        while (g_uart6_tx_busy);  // 等待发送完成标志
        str++;
    }
}

// HardWare Serial Peripherals
fsp_err_t uart6_init(void)
{
	uart_cfg_t uart6_cfg = g_uart6_cfg;
		uart6_cfg.p_callback = uart6_callback;
		uart6_cfg.p_context = NULL;
	    fsp_err_t err = R_SCI_UART_Open(&g_uart6_ctrl, &uart6_cfg);
	    if (FSP_SUCCESS != err)
	    {
	        return err;
	    }
	    return FSP_SUCCESS;
}

fsp_err_t uart0_init(void)
{
	uart_cfg_t uart0_cfg = g_uart0_cfg;
	uart0_cfg.p_callback = uart0_callback;
	uart0_cfg.p_context = NULL;
    fsp_err_t err = R_SCI_UART_Open(&g_uart0_ctrl, &uart0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    return FSP_SUCCESS;
}

void uart0_callback(uart_callback_args_t *p_args)
{
    if (p_args->event == UART_EVENT_TX_COMPLETE)
    {
        g_uart0_tx_busy = false;
    }
}

void uart6_callback(uart_callback_args_t *p_args)
{
	static uint8_t RxState = 0;		//定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket = 0;	//定义表示当前接收数据位置的静态变量

	if (p_args->event == UART_EVENT_TX_COMPLETE)
	    {
	        g_uart6_tx_busy = false; //Send complete
	    }

	else if (p_args->event == UART_EVENT_RX_CHAR)	//判断是否是USART1的接收事件触发的中断
	{
		uint8_t RxData = (uint8_t)p_args->data;			//读取数据寄存器，存放在接收的数据变量

		/*使用状态机的思路，依次处理数据包的不同部分*/

		/*当前状态为0，接收数据包包头*/
		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag == 0)		//如果数据确实是包头，并且上一个数据包已处理完毕
			{
				RxState = 1;			//置下一个状态
				pRxPacket = 0;			//数据包的位置归零
			}
		}
		/*当前状态为1，接收数据包数据，同时判断是否接收到了第一个包尾*/
		else if (RxState == 1)
		{
			if (RxData == '\r')			//如果收到第一个包尾
			{
				RxState = 2;			//置下一个状态
			}
			else						//接收到了正常的数据
			{
				Serial_RxPacket[pRxPacket] = RxData;		//将数据存入数据包数组的指定位置
				pRxPacket ++;			//数据包的位置自增
			}
		}
		/*当前状态为2，接收数据包第二个包尾*/
		else if (RxState == 2)
		{
			if (RxData == '\n')			//如果收到第二个包尾
			{
				RxState = 0;			//状态归0
				Serial_RxPacket[pRxPacket] = '\0';			//将收到的字符数据包添加一个字符串结束标志
				Serial_RxFlag = 1;		//接收数据包标志位置1，成功接收一个数据包
			}
		}
		// End
	}
}




