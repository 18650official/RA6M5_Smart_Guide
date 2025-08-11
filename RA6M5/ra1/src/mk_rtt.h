#ifndef __MK_RTT__
#define __MK_RTT__

#include <stdio.h>
#include <stdarg.h>
#include "hal_data.h"
#include "SEGGER_RTT.h"
#include "mk_pinctrl.h"

#define SEGGER_INDEX            (0)
#define RTT_printf(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);
#define APP_ERR_PRINT(fn_, ...)  if(LVL_ERR)\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__);


void dbg_logi(const char *fmt, ...);
void dbg_rtt_init();
void tty6_logi(const char *fmt, ...);
void uart6_callback(uart_callback_args_t *p_args);
void tty9_logi(const char *fmt, ...);
void uart9_callback(uart_callback_args_t *p_args);
void uart0_callback(uart_callback_args_t *p_args);
fsp_err_t uart6_init(void);
fsp_err_t uart0_init(void);
fsp_err_t uart9_init(void);
fsp_err_t uart_all_init(void);
void tts_print(char *str);
void tts_printf(const char *fmt, ...);
void uart6_send_str(char *str);
void uart9_send_str(char *str);

#endif // DEBUG_LOG_H
