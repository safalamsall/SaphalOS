#ifndef UI_FRAMEBUFFER_H
#define UI_FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    int fd;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    size_t screensize;
    uint8_t* fbp;
    int is_virtual;
} fb_context_t;

int  fb_init(fb_context_t* ctx, const char* device);
void fb_close(fb_context_t* ctx);
void fb_draw_pixel(fb_context_t* ctx, int x, int y, uint32_t color);
void fb_fill_rect(fb_context_t* ctx, int x, int y, int w, int h, uint32_t color);
void fb_clear(fb_context_t* ctx, uint32_t color);
void fb_draw_char(fb_context_t* ctx, int x, int y, char c, uint32_t color, int scale);
void fb_draw_string(fb_context_t* ctx, int x, int y, const char* str, uint32_t color, int scale);
int  fb_save_ppm(fb_context_t* ctx, const char* filename);

#endif /* UI_FRAMEBUFFER_H */
