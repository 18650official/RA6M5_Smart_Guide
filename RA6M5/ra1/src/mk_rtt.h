#ifndef __MK_RTT__
#define __MK_RTT__

#include <stdio.h>
#include <stdarg.h>
#include "hal_data.h"

void dbg_logi(const char *fmt, ...);
void tty6_logi(const char *fmt, ...);
void uart6_callback(uart_callback_args_t *p_args);
void uart0_callback(uart_callback_args_t *p_args);
fsp_err_t uart6_init(void);
fsp_err_t uart0_init(void);
void tts_print(char *str);
void uart6_send_str(char *str);

#endif // DEBUG_LOG_H
