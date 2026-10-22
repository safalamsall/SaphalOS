#include "serial.h"
#include "io.h"
#include "vga.h"

static int is_transmit_empty(void) {
    return inb(COM1_PORT + 5) & 0x20;
}

void serial_init(void) {
    outb(COM1_PORT + 1, 0x00);    // Disable interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x01);    // Set divisor to 1 (low byte) -> 115200 baud
    outb(COM1_PORT + 1, 0x00);    //                  (high byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit (8N1)
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear TX/RX, 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_putc(char c) {
    while (!is_transmit_empty()) {
        io_wait();
    }
    outb(COM1_PORT, (uint8_t)c);
}

void serial_puts(const char* str) {
    if (!str) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            serial_putc('\r');
        }
        serial_putc(str[i]);
    }
}

void serial_print_dec(uint32_t val) {
    char buf[12];
    int i = 0;

    if (val == 0) {
        serial_putc('0');
        return;
    }

    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        serial_putc(buf[j]);
    }
}

void serial_print_hex(uint32_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    serial_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        serial_putc(hex_chars[nibble]);
    }
}

void klog(const char* level, const char* msg) {
    // 1. Output to Serial COM1 (for headless test runner assertions)
    serial_puts("[");
    serial_puts(level);
    serial_puts("] ");
    serial_puts(msg);
    serial_puts("\n");

    // 2. Output to VGA console
    if (level[0] == 'E' && level[1] == 'R') {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    } else if (level[0] == 'S' && level[1] == 'E') {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else if (level[0] == 'W' && level[1] == 'A') {
        vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    }

    vga_puts("[");
    vga_puts(level);
    vga_puts("] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts(msg);
    vga_puts("\n");
}
