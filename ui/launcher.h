#ifndef UI_LAUNCHER_H
#define UI_LAUNCHER_H

#include "framebuffer.h"

typedef struct {
    const char* id;
    const char* name;
    const char* icon_label;
    uint32_t color;
    const char* command;
} app_tile_t;

void render_smartphone_homescreen(fb_context_t* ctx, const char* owner_name, const char* avatar_path);

#endif /* UI_LAUNCHER_H */
