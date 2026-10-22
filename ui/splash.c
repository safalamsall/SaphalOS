#define _GNU_SOURCE
#include "splash.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define COLOR_NAVY       0xFF0A1128
#define COLOR_DARK_BLUE  0xFF1C2541
#define COLOR_CYAN       0xFF00F5D4
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GREY       0xFFAAAAAA
#define COLOR_GREEN      0xFF38B000
#define COLOR_RED        0xFFD00000

void render_boot_splash(fb_context_t* ctx, int progress_percent) {
    if (!ctx) return;

    // 1. Deep space navy background
    fb_clear(ctx, COLOR_NAVY);

    // 2. Status Bar at the top (Height: 60px)
    fb_fill_rect(ctx, 0, 0, ctx->width, 60, COLOR_DARK_BLUE);
    fb_draw_string(ctx, 30, 20, "12:00", COLOR_WHITE, 2);
    fb_draw_string(ctx, ctx->width / 2 - 120, 20, "[SEC: ENFORCING]", COLOR_GREEN, 2);
    fb_draw_string(ctx, ctx->width - 150, 20, "BAT 98%", COLOR_WHITE, 2);

    // 3. Central Brand & Shield
    int center_y = ctx->height / 3;
    fb_draw_string(ctx, ctx->width / 2 - 200, center_y, "SAPHAL OS", COLOR_CYAN, 5);
    fb_draw_string(ctx, ctx->width / 2 - 180, center_y + 80, "v0.1.0-alpha (ARM64 / x86)", COLOR_GREY, 2);
    fb_draw_string(ctx, ctx->width / 2 - 260, center_y + 120, "Resilient. Transparent. Hardened.", COLOR_WHITE, 2);

    // 4. Boot Progress Bar
    int bar_w = ctx->width - 200;
    int bar_h = 24;
    int bar_x = 100;
    int bar_y = ctx->height - 300;

    // Progress bar frame
    fb_fill_rect(ctx, bar_x - 4, bar_y - 4, bar_w + 8, bar_h + 8, COLOR_DARK_BLUE);
    // Fill progress
    int fill_w = (bar_w * progress_percent) / 100;
    if (fill_w > 0) {
        fb_fill_rect(ctx, bar_x, bar_y, fill_w, bar_h, COLOR_CYAN);
    }

    fb_draw_string(ctx, bar_x, bar_y - 40, "INITIALIZING HARDENED USERSPACE...", COLOR_WHITE, 2);

    // 5. Security & Attribution Footer
    fb_draw_string(ctx, ctx->width / 2 - 280, ctx->height - 100, "INDEPENDENT NON-ANDROID PLATFORM", COLOR_GREY, 2);
}

void render_smartphone_lockscreen(fb_context_t* ctx) {
    if (!ctx) return;

    fb_clear(ctx, COLOR_NAVY);

    // Status bar
    fb_fill_rect(ctx, 0, 0, ctx->width, 60, COLOR_DARK_BLUE);
    fb_draw_string(ctx, 30, 20, "12:34", COLOR_WHITE, 2);
    fb_draw_string(ctx, ctx->width / 2 - 120, 20, "[SEC: ENFORCING]", COLOR_GREEN, 2);
    fb_draw_string(ctx, ctx->width - 150, 20, "BAT 98%", COLOR_WHITE, 2);

    // Large Clock
    fb_draw_string(ctx, ctx->width / 2 - 180, 300, "12:34", COLOR_WHITE, 8);
    fb_draw_string(ctx, ctx->width / 2 - 190, 420, "Thursday, October 22", COLOR_CYAN, 3);

    // Security Status Card
    int card_w = ctx->width - 120;
    int card_h = 240;
    int card_x = 60;
    int card_y = 650;
    fb_fill_rect(ctx, card_x, card_y, card_w, card_h, COLOR_DARK_BLUE);
    fb_draw_string(ctx, card_x + 30, card_y + 30, "SAPHAL SECURITY MONITOR", COLOR_CYAN, 3);
    fb_draw_string(ctx, card_x + 30, card_y + 80, "[+] SELinux / Enforcing Mode: ACTIVE", COLOR_GREEN, 2);
    fb_draw_string(ctx, card_x + 30, card_y + 120, "[+] Telemetry Modules       : PURGED (0%)", COLOR_GREEN, 2);
    fb_draw_string(ctx, card_x + 30, card_y + 160, "[+] Android Runtime / AOSP  : EXCLUDED", COLOR_GREEN, 2);

    // Bottom Unlock Hint
    fb_draw_string(ctx, ctx->width / 2 - 150, ctx->height - 150, "^ Swipe Up to Unlock ^", COLOR_GREY, 2);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    fb_context_t fb;
    if (fb_init(&fb, NULL) != 0) {
        fprintf(stderr, "[SAPHAL-SPLASH] Failed to initialize framebuffer\n");
        return 1;
    }

    printf("[SAPHAL-SPLASH] Rendering boot splash screen...\n");
    render_boot_splash(&fb, 85);

    if (fb.is_virtual) {
        fb_save_ppm(&fb, "/tmp/saphal_bootsplash.ppm");
    }

    printf("[SAPHAL-SPLASH] Rendering lockscreen...\n");
    render_smartphone_lockscreen(&fb);

    if (fb.is_virtual) {
        fb_save_ppm(&fb, "/tmp/saphal_lockscreen.ppm");
    }

    fb_close(&fb);
    printf("[SAPHAL-SPLASH] Splash rendering execution completed successfully.\n");
    return 0;
}
