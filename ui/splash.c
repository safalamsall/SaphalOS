#define _GNU_SOURCE
#include "splash.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define COLOR_NAVY       0xFF0A1128
#define COLOR_DARK_BLUE  0xFF1C2541
#define COLOR_CYAN       0xFF00F5D4
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GREY       0xFFAAAAAA
#define COLOR_GREEN      0xFF38B000
#define COLOR_GOLD       0xFFFFD166

void render_personalized_boot_splash(fb_context_t* ctx, const char* owner_name, const char* avatar_path, int progress) {
    if (!ctx) return;
    if (!owner_name) owner_name = DEFAULT_OWNER_NAME;

    // 1. Deep space navy background
    fb_clear(ctx, COLOR_NAVY);

    // 2. Status Bar at top (Height: 50px for 720x1600 cattail panel)
    fb_fill_rect(ctx, 0, 0, ctx->width, 50, COLOR_DARK_BLUE);
    fb_draw_string(ctx, 20, 15, "12:00", COLOR_WHITE, 2);
    fb_draw_string(ctx, ctx->width / 2 - 110, 15, "[SEC: ENFORCING]", COLOR_GREEN, 2);
    fb_draw_string(ctx, ctx->width - 130, 15, "BAT 98%", COLOR_WHITE, 2);

    // 3. Central Owner Avatar / Photo Circle
    int center_x = ctx->width / 2;
    int avatar_y = ctx->height / 3 - 40;
    int avatar_radius = 75;

    // Check if custom avatar exists in assets or system path
    const char* img_file = NULL;
    if (avatar_path && access(avatar_path, R_OK) == 0) {
        img_file = avatar_path;
    } else if (access("assets/owner_avatar.ppm", R_OK) == 0) {
        img_file = "assets/owner_avatar.ppm";
    }

    // Render avatar photo (or sleek badge with owner's initial if image not provided)
    fb_draw_avatar_or_placeholder(ctx, center_x, avatar_y, avatar_radius, img_file, owner_name);

    // 4. Owner Name & Greetings
    int text_y = avatar_y + avatar_radius + 40;
    char welcome_msg[64];
    snprintf(welcome_msg, sizeof(welcome_msg), "WELCOME, %s", owner_name);

    // Center greeting
    int name_pixel_width = strlen(welcome_msg) * 8 * 3;
    fb_draw_string(ctx, (ctx->width - name_pixel_width) / 2, text_y, welcome_msg, COLOR_CYAN, 3);

    const char* role_title = "AUTHORIZED DEVICE OWNER";
    int role_pixel_width = strlen(role_title) * 8 * 2;
    fb_draw_string(ctx, (ctx->width - role_pixel_width) / 2, text_y + 40, role_title, COLOR_GOLD, 2);

    // 5. System Platform Identity
    const char* sys_title = "SAPHAL OS MOBILE CORE";
    int sys_w = strlen(sys_title) * 8 * 3;
    fb_draw_string(ctx, (ctx->width - sys_w) / 2, text_y + 110, sys_title, COLOR_WHITE, 3);

    const char* hw_info = "Xiaomi Redmi 9 (cattail / MT6765G)";
    int hw_w = strlen(hw_info) * 8 * 2;
    fb_draw_string(ctx, (ctx->width - hw_w) / 2, text_y + 150, hw_info, COLOR_GREY, 2);

    // 6. Boot Progress Bar
    int bar_w = ctx->width - 160;
    int bar_h = 16;
    int bar_x = 80;
    int bar_y = ctx->height - 240;

    // Outer frame
    fb_fill_rect(ctx, bar_x - 3, bar_y - 3, bar_w + 6, bar_h + 6, COLOR_DARK_BLUE);

    // Progress fill
    int fill_w = (bar_w * progress) / 100;
    if (fill_w > 0) {
        fb_fill_rect(ctx, bar_x, bar_y, fill_w, bar_h, COLOR_CYAN);
    }

    const char* status_text = "INITIALIZING HARDENED USERSPACE...";
    int status_w = strlen(status_text) * 8 * 2;
    fb_draw_string(ctx, (ctx->width - status_w) / 2, bar_y - 30, status_text, COLOR_WHITE, 2);

    // 7. Security & Attribution Footer
    const char* footer = "INDEPENDENT NON-ANDROID ARCHITECTURE";
    int footer_w = strlen(footer) * 8 * 2;
    fb_draw_string(ctx, (ctx->width - footer_w) / 2, ctx->height - 70, footer, COLOR_GREY, 2);
}

void render_smartphone_lockscreen(fb_context_t* ctx, const char* owner_name, const char* avatar_path) {
    if (!ctx) return;
    if (!owner_name) owner_name = DEFAULT_OWNER_NAME;

    fb_clear(ctx, COLOR_NAVY);

    // Status bar
    fb_fill_rect(ctx, 0, 0, ctx->width, 50, COLOR_DARK_BLUE);
    fb_draw_string(ctx, 20, 15, "12:34", COLOR_WHITE, 2);
    fb_draw_string(ctx, ctx->width / 2 - 110, 15, "[SEC: ENFORCING]", COLOR_GREEN, 2);
    fb_draw_string(ctx, ctx->width - 130, 15, "BAT 98%", COLOR_WHITE, 2);

    // Digital Clock
    fb_draw_string(ctx, ctx->width / 2 - 140, 160, "12:34", COLOR_WHITE, 7);
    fb_draw_string(ctx, ctx->width / 2 - 160, 250, "Thursday, October 22", COLOR_CYAN, 2);

    // Owner Card
    int card_w = ctx->width - 80;
    int card_h = 240;
    int card_x = 40;
    int card_y = 340;
    fb_fill_rect(ctx, card_x, card_y, card_w, card_h, COLOR_DARK_BLUE);

    // Avatar in card
    int av_cx = card_x + 70;
    int av_cy = card_y + card_h / 2;
    fb_draw_avatar_or_placeholder(ctx, av_cx, av_cy, 45, avatar_path, owner_name);

    char owner_line[64];
    snprintf(owner_line, sizeof(owner_line), "Device Owner: %s", owner_name);
    fb_draw_string(ctx, card_x + 140, card_y + 40, owner_line, COLOR_CYAN, 2);
    fb_draw_string(ctx, card_x + 140, card_y + 75, "Identity: Verified Admin", COLOR_WHITE, 2);
    fb_draw_string(ctx, card_x + 140, card_y + 110, "Status  : Authenticated", COLOR_GREEN, 2);
    fb_draw_string(ctx, card_x + 140, card_y + 145, "Security: Level Ring 0", COLOR_GOLD, 2);

    // Security Status Card
    int sec_card_y = card_y + card_h + 30;
    int sec_card_h = 200;
    fb_fill_rect(ctx, card_x, sec_card_y, card_w, sec_card_h, COLOR_DARK_BLUE);
    fb_draw_string(ctx, card_x + 30, sec_card_y + 25, "SAPHAL SECURITY ENGINE", COLOR_CYAN, 2);
    fb_draw_string(ctx, card_x + 30, sec_card_y + 65, "[+] Hardened Mounts   : ACTIVE", COLOR_GREEN, 2);
    fb_draw_string(ctx, card_x + 30, sec_card_y + 100, "[+] Telemetry Modules : PURGED", COLOR_GREEN, 2);
    fb_draw_string(ctx, card_x + 30, sec_card_y + 135, "[+] Android Framework : NONE", COLOR_GREEN, 2);

    // Unlock Hint
    fb_draw_string(ctx, ctx->width / 2 - 130, ctx->height - 100, "^ Swipe Up to Unlock ^", COLOR_GREY, 2);
}

int main(int argc, char* argv[]) {
    const char* name = DEFAULT_OWNER_NAME;
    const char* avatar = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--name") == 0 && i + 1 < argc) {
            name = argv[++i];
        } else if (strcmp(argv[i], "--avatar") == 0 && i + 1 < argc) {
            avatar = argv[++i];
        }
    }

    fb_context_t fb;
    if (fb_init(&fb, NULL) != 0) {
        fprintf(stderr, "[SAPHAL-SPLASH] Failed to initialize framebuffer\n");
        return 1;
    }

    printf("[SAPHAL-SPLASH] Rendering personalized boot animation for: %s\n", name);
    render_personalized_boot_splash(&fb, name, avatar, 90);

    if (fb.is_virtual) {
        fb_save_ppm(&fb, "/tmp/saphal_owner_bootsplash.ppm");
    }

    printf("[SAPHAL-SPLASH] Rendering personalized lockscreen for: %s\n", name);
    render_smartphone_lockscreen(&fb, name, avatar);

    if (fb.is_virtual) {
        fb_save_ppm(&fb, "/tmp/saphal_owner_lockscreen.ppm");
    }

    fb_close(&fb);
    printf("[SAPHAL-SPLASH] Custom boot screen rendering successfully completed!\n");
    return 0;
}
