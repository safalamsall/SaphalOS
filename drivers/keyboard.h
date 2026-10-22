#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
int  keyboard_has_char(void);

#endif /* DRIVERS_KEYBOARD_H */
