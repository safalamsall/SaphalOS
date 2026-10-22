#include "timer.h"
#include "io.h"
#include "../kernel/idt.h"
#include "../kernel/pic.h"

static volatile uint32_t timer_ticks = 0;
static uint32_t timer_freq = 100;

static void timer_callback(registers_t* regs) {
    (void)regs;
    timer_ticks++;
}

void timer_init(uint32_t frequency) {
    timer_freq = frequency;
    register_interrupt_handler(32, timer_callback);

    // The value we send to the PIT is the value to divide its input clock
    // (1193180 Hz) by, to get our required frequency.
    uint32_t divisor = 1193180 / frequency;

    // Send the command byte (Channel 0, Square wave, Access mode lobyte/hibyte)
    outb(0x43, 0x36);

    // Divisor must be sent byte-wise
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);

    // Enable IRQ 0 on PIC
    pic_clear_mask(0);
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

uint32_t timer_get_uptime_sec(void) {
    return timer_ticks / timer_freq;
}

void timer_wait(uint32_t ticks) {
    uint32_t eticks = timer_ticks + ticks;
    while (timer_ticks < eticks) {
        __asm__ volatile ("hlt");
    }
}
