/* test_sharp_memory_display.c
 *
 * Copyright (C) 2026 wolfSSL Inc.
 *
 * This file is part of wolfHAL.
 *
 * wolfHAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/*
 * Sharp Memory LCD (LS013B7DH03) test.
 *
 * Builds a 1bpp framebuffer in RAM, draws a few primitive shapes into it,
 * and pushes the result to the panel with the driver's Update path. On real
 * hardware the shapes are visible; in all cases the test asserts that each
 * transfer (and each argument-validation case) returns the expected status.
 *
 * The framebuffer uses the driver's documented pixel layout: 1 bit per
 * pixel, packed MSB-first (bit 7 of a byte is the left-most pixel), where a
 * 1 bit is white and a 0 bit is black.
 *
 * The panel is an optional external peripheral. Wire it up with
 *   make TESTS="... sharp_memory_display" PERIPHERALS="sharp_ls013b7dh03"
 * When the peripheral is not present the suite skips.
 */

#include <stdint.h>
#include <stddef.h>
#include <wolfHAL/wolfHAL.h>
#include "test.h"

#ifdef PERIPHERAL_SHARP_LS013B7DH03
#include "display/sharp_ls013b7dh03.h"

/* Largest panel this test allocates a framebuffer for (LS013B7DH03 is 128x128). */
#define SHARP_MAX_W   128
#define SHARP_MAX_H   128
#define SHARP_STRIDE(w) ((w) / 8)
#define SHARP_FB_SZ   (SHARP_STRIDE(SHARP_MAX_W) * SHARP_MAX_H)

#define PIXEL_WHITE 1
#define PIXEL_BLACK 0

static uint8_t g_fb[SHARP_FB_SZ];

static whal_Display *g_dev;
static uint16_t g_w;
static uint16_t g_h;
static size_t g_stride;

/* --- tiny 1bpp drawing primitives (MSB-first packing, 1 = white) --- */

static void Fb_Fill(uint8_t color)
{
    size_t i;
    uint8_t byte = color ? 0xFF : 0x00;
    for (i = 0; i < g_stride * g_h; i++)
        g_fb[i] = byte;
}

static void Fb_SetPixel(int x, int y, uint8_t color)
{
    uint8_t mask;

    if (x < 0 || y < 0 || x >= (int)g_w || y >= (int)g_h)
        return;

    mask = (uint8_t)(0x80 >> (x & 7));
    if (color)
        g_fb[y * g_stride + (x >> 3)] |= mask;
    else
        g_fb[y * g_stride + (x >> 3)] &= (uint8_t)~mask;
}

static void Fb_HLine(int x, int y, int len, uint8_t color)
{
    int i;
    for (i = 0; i < len; i++)
        Fb_SetPixel(x + i, y, color);
}

static void Fb_VLine(int x, int y, int len, uint8_t color)
{
    int i;
    for (i = 0; i < len; i++)
        Fb_SetPixel(x, y + i, color);
}

/* Outlined rectangle. */
static void Fb_Rect(int x, int y, int w, int h, uint8_t color)
{
    Fb_HLine(x, y, w, color);
    Fb_HLine(x, y + h - 1, w, color);
    Fb_VLine(x, y, h, color);
    Fb_VLine(x + w - 1, y, h, color);
}

/* Solid rectangle. */
static void Fb_FillRect(int x, int y, int w, int h, uint8_t color)
{
    int row;
    for (row = 0; row < h; row++)
        Fb_HLine(x, y + row, w, color);
}

/* Line via Bresenham's algorithm. */
static void Fb_Line(int x0, int y0, int x1, int y1, uint8_t color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = dx < 0 ? -1 : 1;
    int sy = dy < 0 ? -1 : 1;
    int err, e2;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    err = dx - dy;

    for (;;) {
        Fb_SetPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

/* Circle outline via the midpoint algorithm. */
static void Fb_Circle(int cx, int cy, int r, uint8_t color)
{
    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        Fb_SetPixel(cx + x, cy + y, color);
        Fb_SetPixel(cx + y, cy + x, color);
        Fb_SetPixel(cx - y, cy + x, color);
        Fb_SetPixel(cx - x, cy + y, color);
        Fb_SetPixel(cx - x, cy - y, color);
        Fb_SetPixel(cx - y, cy - x, color);
        Fb_SetPixel(cx + y, cy - x, color);
        Fb_SetPixel(cx + x, cy - y, color);
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        }
        else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

/* Push the whole framebuffer to the panel. */
static whal_Error Fb_Flush(void)
{
    return whal_SharpMemory_Display_Update(g_dev, 0, 0, g_w, g_h,
                                           g_fb, g_stride * g_h);
}

/* --- tests --- */

static void Test_SharpDisplay_Init(void)
{
    whal_Display *dev = &g_whalSharpLs013b7dh03;
    whal_SharpMemory_Display_Cfg *cfg;

    cfg = (whal_SharpMemory_Display_Cfg *)dev->cfg;
    WHAL_ASSERT_NEQ(cfg, NULL);

    g_w = cfg->width;
    g_h = cfg->height;
    g_stride = SHARP_STRIDE(g_w);

    /* This test's static framebuffer only covers up to 128x128. */
    if (g_w == 0 || g_h == 0 || g_w > SHARP_MAX_W || g_h > SHARP_MAX_H ||
        (g_w & 7))
        WHAL_SKIP();

    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Init(dev), WHAL_SUCCESS);

    /* Only now is the panel geometry known to fit g_fb; the tests below run
     * off g_dev being set. */
    g_dev = dev;
}

static void Test_SharpDisplay_DrawShapes(void)
{
    int cx, cy, r;
    int gx, gy;

    if (!g_dev)
        WHAL_SKIP();

    cx = g_w / 2;
    cy = g_h / 2;
    r = (g_w < g_h ? g_w : g_h) / 4;

    /* Start from a white background (1 = white). */
    Fb_Fill(PIXEL_WHITE);

    /* Black border one pixel inside the edges. */
    Fb_Rect(0, 0, g_w, g_h, PIXEL_BLACK);

    /* Crossed diagonals. */
    Fb_Line(0, 0, g_w - 1, g_h - 1, PIXEL_BLACK);
    Fb_Line(g_w - 1, 0, 0, g_h - 1, PIXEL_BLACK);

    /* Solid square in the top-left quadrant. */
    Fb_FillRect(g_w / 8, g_h / 8, g_w / 4, g_h / 4, PIXEL_BLACK);

    /* Circle centered on the panel. */
    Fb_Circle(cx, cy, r, PIXEL_BLACK);

    /* Checkerboard of 8x8 cells in the bottom-right quadrant. */
    for (gy = g_h / 2; gy < (int)g_h; gy += 8) {
        for (gx = g_w / 2; gx < (int)g_w; gx += 8) {
            if (((gx / 8) ^ (gy / 8)) & 1)
                Fb_FillRect(gx, gy, 8, 8, PIXEL_BLACK);
        }
    }

    WHAL_ASSERT_EQ(Fb_Flush(), WHAL_SUCCESS);
}

static void Test_SharpDisplay_PartialUpdate(void)
{
    uint16_t bandY, bandH;

    if (!g_dev)
        WHAL_SKIP();

    bandY = g_h / 4;
    bandH = g_h / 2;

    /* Repaint just the middle band of rows as solid black, full width. */
    Fb_FillRect(0, bandY, g_w, bandH, PIXEL_BLACK);

    WHAL_ASSERT_EQ(
        whal_SharpMemory_Display_Update(g_dev, 0, bandY, g_w, bandH,
                                        g_fb + (size_t)bandY * g_stride,
                                        (size_t)bandH * g_stride),
        WHAL_SUCCESS);
}

static void Test_SharpDisplay_ApiValidation(void)
{
    if (!g_dev)
        WHAL_SKIP();

    /* NULL data. */
    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Update(g_dev, 0, 0, g_w, g_h,
                                                   NULL, g_stride * g_h),
                   WHAL_EINVAL);

    /* Partial-width region is unsupported by the panel. */
    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Update(g_dev, 0, 0, g_w / 2, g_h,
                                                   g_fb, g_stride * g_h),
                   WHAL_ENOTSUP);
    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Update(g_dev, 1, 0, g_w, g_h,
                                                   g_fb, g_stride * g_h),
                   WHAL_ENOTSUP);

    /* Region runs past the bottom edge. */
    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Update(g_dev, 0, 1, g_w, g_h,
                                                   g_fb, g_stride * g_h),
                   WHAL_EINVAL);

    /* dataSz does not match the requested region. */
    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Update(g_dev, 0, 0, g_w, g_h,
                                                   g_fb, g_stride * g_h - 1),
                   WHAL_EINVAL);
}

static void Test_SharpDisplay_Deinit(void)
{
    if (!g_dev)
        WHAL_SKIP();

    WHAL_ASSERT_EQ(whal_SharpMemory_Display_Deinit(g_dev), WHAL_SUCCESS);
}
#endif /* PERIPHERAL_SHARP_LS013B7DH03 */

void whal_Test_SharpMemoryDisplay(void)
{
    WHAL_TEST_SUITE_START("sharp_memory_display");
#ifdef PERIPHERAL_SHARP_LS013B7DH03
    WHAL_TEST(Test_SharpDisplay_Init);
    WHAL_TEST(Test_SharpDisplay_DrawShapes);
    WHAL_TEST(Test_SharpDisplay_PartialUpdate);
    WHAL_TEST(Test_SharpDisplay_ApiValidation);
    WHAL_TEST(Test_SharpDisplay_Deinit);
#else
    whal_Test_Printf("  no sharp_ls013b7dh03 peripheral wired; skipping\n");
#endif
    WHAL_TEST_SUITE_END();
}
