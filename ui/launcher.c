#define _GNU_SOURCE
#include "launcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_NAVY       0xFF0A1128
#define COLOR_DARK_BLUE  0xFF1C2541
#define COLOR_SURFACE    0xFF161F38
#define COLOR_CYAN       0xFF00F5D4
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GREY       0xFFAAAAAA
#define COLOR_GREEN      0xFF38B000
#define COLOR_GOLD       0xFFFFD166
#define COLOR_PURPLE     0xFF9D4EDD
#define COLOR_BLUE       0xFF48BFE3
#define COLOR_RED        0xFFD00000

static const app_tile_t apps[] = {
    {"sec",   "Security Center", "SEC", COLOR_CYAN,   "saphal-secd --audit"},
    {"term",  "Terminal Shell",  ">_",  COLOR_GREEN,  "saphal-sh"},
    {"diag",  "Diagnostics",     "HW",  COLOR_GOLD,   "saphal-sh -c diag"},
    {"sett",  "Device Settings", "CFG", COLOR_BLUE,   "saphal-sh -c sysinfo"},
    {"media", "Audio Player",    "AUD", COLOR_PURPLE, "play_chime"},
    {"power", "Power Controls",  "PWR", COLOR_RED,    "reboot"}
};

#define APP_COUNT (sizeof(apps) / sizeof(apps[0]))

void render_smartphone_homescreen(fb_context_t* ctx, const char* owner_name, const char* avatar_path) {
    if (!ctx) return;
    if (!owner_name) owner_name = "Saphal";

    // 1. Wallpaper background
    fb_clear(ctx, COLOR_NAVY);

    // 2. Status Bar at top (Height: 50px)
    fb_fill_rect(ctx, 0, 0, ctx->width, 50, COLOR_DARK_BLUE);
    fb_draw_string(ctx, 20, 15, "12:00", COLOR_WHITE, 2);
    fb_draw_string(ctx, ctx->width / 2 - 110, 15, "[SEC: ENFORCING]", COLOR_GREEN, 2);
    fb_draw_string(ctx, ctx->width - 130, 15, "BAT 98%", COLOR_WHITE, 2);

    // 3. User Identity & Status Widget Card (Height: 180px)
    int card_w = ctx->width - 60;
    int card_h = 160;
    int card_x = 30;
    int card_y = 80;
    fb_fill_rect(ctx, card_x, card_y, card_w, card_h, COLOR_SURFACE);
    for (int b = 0; b < 2; b++) {
        fb_fill_rect(ctx, card_x + b, card_y + b, card_w - 2 * b, card_h - 2 * b, COLOR_DARK_BLUE);
    }

    // Avatar inside Widget
    int av_cx = card_x + 65;
    int av_cy = card_y + card_h / 2;
    fb_draw_avatar_or_placeholder(ctx, av_cx, av_cy, 45, avatar_path, owner_name);

    // Greeting & Platform metadata
    char name_buf[64];
    snprintf(name_buf, sizeof(name_buf), "HELLO, %s", owner_name);
    fb_draw_string(ctx, card_x + 130, card_y + 25, name_buf, COLOR_CYAN, 3);
    fb_draw_string(ctx, card_x + 130, card_y + 65, "Role: Verified Device Owner (UID 0)", COLOR_GOLD, 2);
    fb_draw_string(ctx, card_x + 130, card_y + 95, "OS  : SAPHAL OS Mobile Core v0.1", COLOR_WHITE, 2);
    fb_draw_string(ctx, card_x + 130, card_y + 125, "Unit: Redmi 9 (cattail / MT6765G)", COLOR_GREY, 2);

    // 4. Application Grid (2 columns x 3 rows)
    int grid_top = card_y + card_h + 40;
    int tile_w = (ctx->width - 90) / 2; // ~315px each
    int tile_h = 170;
    int gap_x = 30;
    int gap_y = 25;

    for (size_t i = 0; i < APP_COUNT; i++) {
        int col = i % 2;
        int row = i / 2;
        int tx = card_x + col * (tile_w + gap_x);
        int ty = grid_top + row * (tile_h + gap_y);

        // Tile Surface
        fb_fill_rect(ctx, tx, ty, tile_w, tile_h, COLOR_DARK_BLUE);

        // Colored Header Accent Bar
        fb_fill_rect(ctx, tx, ty, tile_w, 6, apps[i].color);

        // App Icon Badge
        int icon_box_size = 56;
        int icon_x = tx + (tile_w - icon_box_size) / 2;
        int icon_y = ty + 30;
        fb_fill_rect(ctx, icon_x, icon_y, icon_box_size, icon_box_size, COLOR_SURFACE);
        fb_draw_string(ctx, icon_x + 10, icon_y + 18, apps[i].icon_label, apps[i].color, 3);

        // App Label
        int label_len = strlen(apps[i].name);
        int label_x = tx + (tile_w - (label_len * 8 * 2)) / 2;
        fb_draw_string(ctx, label_x, ty + 105, apps[i].name, COLOR_WHITE, 2);

        // Status pill
        fb_draw_string(ctx, tx + (tile_w - 70) / 2, ty + 135, "ACTIVE", COLOR_GREEN, 1);
    }

    // 5. Bottom Navigation Dock
    int nav_h = 70;
    int nav_y = ctx->height - nav_h;
    fb_fill_rect(ctx, 0, nav_y, ctx->width, nav_h, COLOR_DARK_BLUE);

    // Navigation buttons: Back (<), Home (O), Overview ([])
    fb_draw_string(ctx, ctx->width / 4 - 8, nav_y + 22, "<", COLOR_GREY, 4);
    fb_draw_string(ctx, ctx->width / 2 - 12, nav_y + 22, "O", COLOR_CYAN, 4);
    fb_draw_string(ctx, (3 * ctx->width) / 4 - 12, nav_y + 22, "#", COLOR_GREY, 4);
}

int main(int argc, char* argv[]) {
    const char* name = "Saphal";
    const char* avatar = "assets/owner_avatar.ppm";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--name") == 0 && i + 1 < argc) {
            name = argv[++i];
        } else if (strcmp(argv[i], "--avatar") == 0 && i + 1 < argc) {
            avatar = argv[++i];
        }
    }

    fb_context_t fb;
    if (fb_init(&fb, NULL) != 0) {
        fprintf(stderr, "[SAPHAL-LAUNCHER] Failed to initialize framebuffer\n");
        return 1;
    }

    printf("[SAPHAL-LAUNCHER] Rendering Mobile GUI Home Screen for: %s\n", name);
    render_smartphone_homescreen(&fb, name, avatar);

    if (fb.is_virtual) {
        fb_save_ppm(&fb, "/tmp/saphal_homescreen.ppm");
    }

    fb_close(&fb);
    printf("[SAPHAL-LAUNCHER] Homescreen render completed successfully!\n");
    return 0;
}
