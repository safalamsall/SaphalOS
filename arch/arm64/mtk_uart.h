#ifndef ARCH_ARM64_MTK_UART_H
#define ARCH_ARM64_MTK_UART_H

#include <stdint.h>

/* MediaTek MT6769V / Helio G80 Debug UART0 Base */
#define MTK_UART0_BASE 0x11002000

#define MTK_UART_RBR   0x00    /* Receive Buffer */
#define MTK_UART_THR   0x00    /* Transmit Holding */
#define MTK_UART_IER   0x04    /* Interrupt Enable */
#define MTK_UART_IIR   0x08    /* Interrupt Identity */
#define MTK_UART_LCR   0x0C    /* Line Control */
#define MTK_UART_LSR   0x14    /* Line Status */

#define MTK_UART_LSR_THRE 0x20  /* Transmit Holding Register Empty */
#define MTK_UART_LSR_DR   0x01  /* Data Ready */

void mtk_uart_init(void);
void mtk_uart_putc(char c);
void mtk_uart_puts(const char* str);

#endif /* ARCH_ARM64_MTK_UART_H */
