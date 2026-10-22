#ifndef DRIVERS_SERIAL_H
#define DRIVERS_SERIAL_H

#include <stdint.h>

#define COM1_PORT 0x3F8

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char* str);
void serial_print_dec(uint32_t val);
void serial_print_hex(uint32_t val);
int  serial_has_char(void);
char serial_getchar(void);
void klog(const char* level, const char* msg);

#endif /* DRIVERS_SERIAL_H */
