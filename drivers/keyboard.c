#include "keyboard.h"
#include "io.h"
#include "../kernel/idt.h"
#include "../kernel/pic.h"

#define KEYBOARD_DATA_PORT 0x60
#define BUFFER_SIZE        128

static char key_buffer[BUFFER_SIZE];
static volatile int buf_head = 0;
static volatile int buf_tail = 0;
static int shift_pressed = 0;

static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right Shift */
    '*',
    0, /* Alt */
    ' ', /* Space */
    0, /* Caps Lock */
};

static const char kbd_us_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,
    '*',
    0,
    ' ',
    0,
};

static void keyboard_callback(registers_t* regs) {
    (void)regs;
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

    // Ignore key release codes
    if (scancode & 0x80) {
        return;
    }

    char c = 0;
    if (scancode < 128) {
        c = shift_pressed ? kbd_us_shift[scancode] : kbd_us[scancode];
    }

    if (c != 0) {
        int next_head = (buf_head + 1) % BUFFER_SIZE;
        if (next_head != buf_tail) {
            key_buffer[buf_head] = c;
            buf_head = next_head;
        }
    }
}

void keyboard_init(void) {
    register_interrupt_handler(33, keyboard_callback);
    pic_clear_mask(1);
}

int keyboard_has_char(void) {
    return (buf_head != buf_tail);
}

char keyboard_getchar(void) {
    while (buf_head == buf_tail) {
        __asm__ volatile ("hlt");
    }
    char c = key_buffer[buf_tail];
    buf_tail = (buf_tail + 1) % BUFFER_SIZE;
    return c;
}
