#include "vga.h"
#include "io.h"

static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;
static volatile uint16_t* const vga_buffer = VGA_MEMORY;

static void update_cursor(size_t row, size_t col) {
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_set_cursor(uint8_t row, uint8_t col) {
    if (row < VGA_HEIGHT && col < VGA_WIDTH) {
        vga_row = row;
        vga_column = col;
        update_cursor(vga_row, vga_column);
    }
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

void vga_clear(void) {
    uint8_t blank_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    uint16_t blank = vga_entry(' ', blank_color);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = blank;
        }
    }
    vga_row = 0;
    vga_column = 0;
    update_cursor(0, 0);
}

static void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }
    vga_row = VGA_HEIGHT - 1;
}

void vga_putc(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\r') {
        vga_column = 0;
    } else if (c == '\t') {
        size_t tab_stop = (vga_column + 4) & ~3;
        while (vga_column < tab_stop && vga_column < VGA_WIDTH) {
            vga_buffer[vga_row * VGA_WIDTH + vga_column++] = vga_entry(' ', vga_color);
        }
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    } else if (c == '\b') {
        vga_backspace();
    } else {
        vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry((unsigned char)c, vga_color);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    update_cursor(vga_row, vga_column);
}

void vga_backspace(void) {
    if (vga_column > 0) {
        vga_column--;
        vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
        update_cursor(vga_row, vga_column);
    }
}

void vga_puts(const char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i]);
    }
}

void vga_print_dec(uint32_t val) {
    char buf[12];
    int i = 0;

    if (val == 0) {
        vga_putc('0');
        return;
    }

    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        vga_putc(buf[j]);
    }
}

void vga_print_hex(uint32_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    vga_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        vga_putc(hex_chars[nibble]);
    }
}

void vga_init(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_draw_banner(void) {
    // Header bar in Dark Cyan with Bright White text
    uint8_t banner_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_CYAN);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[0 * VGA_WIDTH + x] = vga_entry(' ', banner_color);
    }
    
    const char title[] = " [ SAPHAL OS v0.1.0 ] - Security-Aware Layered Kernel (Ring 0 / Protected) ";
    size_t start_col = (VGA_WIDTH > sizeof(title)) ? (VGA_WIDTH - sizeof(title)) / 2 : 0;
    for (size_t i = 0; title[i] != '\0'; i++) {
        vga_buffer[0 * VGA_WIDTH + (start_col + i)] = vga_entry(title[i], banner_color);
    }

    vga_row = 2;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  ===============================================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("   _____            _____  _    _          _        ____   _____ \n");
    vga_puts("  / ____|   /\\     |  __ \\| |  | |   /\\   | |      / __ \\ / ____|\n");
    vga_puts(" | (___    /  \\    | |__) | |__| |  /  \\  | |     | |  | | (___  \n");
    vga_puts("  \\___ \\  / /\\ \\   |  ___/|  __  | / /\\ \\ | |     | |  | |\\___ \\ \n");
    vga_puts("  ____) |/ ____ \\  | |    | |  | |/ ____ \\| |____ | |__| |____) |\n");
    vga_puts(" |_____//_/    \\_\\ |_|    |_|  |_/_/    \\_\\______| \\____/|_____/ \n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("  ===============================================================\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("  Resilient. Transparent. Hardened. Non-Android Independent OS\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
