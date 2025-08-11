/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI6 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI6 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI6 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI6 ERI (Receive error) */
            [4] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
            [5] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [6] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [7] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [8] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [9] = r_icu_isr, /* ICU IRQ7 (External pin interrupt 7) */
            [10] = r_icu_isr, /* ICU IRQ8 (External pin interrupt 8) */
            [11] = sci_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [12] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [13] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [14] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI6_RXI,GROUP0), /* SCI6 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI6_TXI,GROUP1), /* SCI6 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI6_TEI,GROUP2), /* SCI6 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI6_ERI,GROUP3), /* SCI6 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_GPT0_COUNTER_OVERFLOW,GROUP4), /* GPT0 COUNTER OVERFLOW (Overflow) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP5), /* SCI0 RXI (Receive data full) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP6), /* SCI0 TXI (Transmit data empty) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP7), /* SCI0 TEI (Transmit end) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP0), /* SCI0 ERI (Receive error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ7,GROUP1), /* ICU IRQ7 (External pin interrupt 7) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_ICU_IRQ8,GROUP2), /* ICU IRQ8 (External pin interrupt 8) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP3), /* SCI9 RXI (Receive data full) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP4), /* SCI9 TXI (Transmit data empty) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP5), /* SCI9 TEI (Transmit end) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP6), /* SCI9 ERI (Receive error) */
        };
        #endif
        #endif
