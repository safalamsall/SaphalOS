#include "mtk_uart.h"

static inline void write_reg(uint32_t offset, uint32_t val) {
    volatile uint32_t* reg = (volatile uint32_t*)(MTK_UART0_BASE + offset);
    *reg = val;
}

static inline uint32_t read_reg(uint32_t offset) {
    volatile uint32_t* reg = (volatile uint32_t*)(MTK_UART0_BASE + offset);
    return *reg;
}

void mtk_uart_init(void) {
    /* LittleKernel typically configures baud (921600 or 115200) prior to kernel handoff.
     * Ensure line control is 8N1. */
    write_reg(MTK_UART_LCR, 0x03);
}

void mtk_uart_putc(char c) {
    while (!(read_reg(MTK_UART_LSR) & MTK_UART_LSR_THRE)) {
        __asm__ volatile ("nop");
    }
    write_reg(MTK_UART_THR, (uint32_t)c);
}

void mtk_uart_puts(const char* str) {
    if (!str) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            mtk_uart_putc('\r');
        }
        mtk_uart_putc(str[i]);
    }
}
