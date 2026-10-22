#ifndef UI_SPLASH_H
#define UI_SPLASH_H

#include "framebuffer.h"

void render_boot_splash(fb_context_t* ctx, int progress_percent);
void render_smartphone_lockscreen(fb_context_t* ctx);

#endif /* UI_SPLASH_H */
