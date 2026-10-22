#ifndef UI_SPLASH_H
#define UI_SPLASH_H

#include "framebuffer.h"

#define DEFAULT_OWNER_NAME   "Saphal"
#define DEFAULT_AVATAR_PATH  "/etc/saphal/owner_avatar.ppm"

void render_personalized_boot_splash(fb_context_t* ctx, const char* owner_name, const char* avatar_path, int progress);
void render_smartphone_lockscreen(fb_context_t* ctx, const char* owner_name, const char* avatar_path);

#endif /* UI_SPLASH_H */
