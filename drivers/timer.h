#ifndef DRIVERS_TIMER_H
#define DRIVERS_TIMER_H

#include <stdint.h>

void timer_init(uint32_t frequency);
uint32_t timer_get_ticks(void);
uint32_t timer_get_uptime_sec(void);
void timer_wait(uint32_t ticks);

#endif /* DRIVERS_TIMER_H */
