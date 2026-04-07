/*
 * SSD1306 90-degree rotation layer for vertically-mounted OLEDs.
 *
 * LVGL's built-in sw_rotate doesn't work with SSD1306's VTILED page format
 * (see zmkfirmware/zmk#1749, zephyrproject-rtos/zephyr#46446).
 *
 * This hooks into the LVGL display driver after init, swaps the resolution
 * to portrait (32x128), and rotates the framebuffer in the flush callback.
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

/* Physical SSD1306: 128 cols x 32 rows (4 pages of 8 rows) */
#define PHYS_W    128
#define PHYS_H    32
#define PHYS_PAGES (PHYS_H / 8)
#define PHYS_BUF  (PHYS_W * PHYS_PAGES) /* 512 bytes */

/* Logical portrait: 32 wide x 128 tall */
#define PORT_W    PHYS_H  /* 32 */
#define PORT_H    PHYS_W  /* 128 */

static const struct device *disp_dev;

/*
 * Custom set_px_cb: pack pixels into portrait VTILED format.
 * LVGL calls this for each pixel at logical (x, y) where x=[0,31] y=[0,127].
 */
static void rot_set_px(lv_disp_drv_t *drv, uint8_t *buf, lv_coord_t buf_w,
                       lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
    uint16_t idx = x + (y >> 3) * buf_w;
    uint8_t bit = y & 7;

    if (color.full) {
        buf[idx] |= (1 << bit);
    } else {
        buf[idx] &= ~(1 << bit);
    }
}

/* Round y to 8-pixel boundary for VTILED packing. */
static void rot_rounder(lv_disp_drv_t *drv, lv_area_t *area)
{
    area->y1 &= ~7;
    area->y2 |= 7;
}

/*
 * Rotate the portrait VTILED buffer 90 degrees CW into the SSD1306's
 * landscape VTILED format, then write to hardware.
 *
 * Portrait pixel (px, py) -> Landscape pixel:
 *   col = py              (portrait Y becomes physical column)
 *   row = 31 - px         (portrait X inverted to physical row)
 */
static void rot_flush(lv_disp_drv_t *drv, const lv_area_t *area,
                      lv_color_t *color_p)
{
    static uint8_t rot_buf[PHYS_BUF];
    uint8_t *src = (uint8_t *)color_p;

    memset(rot_buf, 0, sizeof(rot_buf));

    for (int py = 0; py < PORT_H; py++) {
        for (int px = 0; px < PORT_W; px++) {
            /* Read from portrait VTILED buffer */
            uint8_t pixel = (src[px + (py >> 3) * PORT_W] >> (py & 7)) & 1;

            if (pixel) {
                /* Map to landscape VTILED */
                uint16_t col = py;
                uint16_t row = (PORT_W - 1) - px;
                rot_buf[col + (row >> 3) * PHYS_W] |= (1 << (row & 7));
            }
        }
    }

    struct display_buffer_descriptor desc = {
        .buf_size = PHYS_BUF,
        .width = PHYS_W,
        .pitch = PHYS_W,
        .height = PHYS_H,
    };

    display_write(disp_dev, 0, 0, &desc, rot_buf);
    lv_disp_flush_ready(drv);
}

/*
 * Patch LVGL display driver after Zephyr's lvgl_init has run.
 * lvgl_init runs at APPLICATION priority (typically 90).
 * We run at priority 99 to ensure we execute after it.
 */
static int rot_init(void)
{
    disp_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(disp_dev)) {
        return -ENODEV;
    }

    lv_disp_t *disp = lv_disp_get_default();
    if (!disp) {
        return -ENODEV;
    }

    lv_disp_drv_t *drv = disp->driver;

    /* Swap to portrait resolution */
    drv->hor_res = PORT_W;   /* 32 */
    drv->ver_res = PORT_H;   /* 128 */

    /* Install rotation callbacks */
    drv->flush_cb = rot_flush;
    drv->set_px_cb = rot_set_px;
    drv->rounder_cb = rot_rounder;
    drv->full_refresh = 1;

    lv_disp_drv_update(disp, drv);

    return 0;
}

SYS_INIT(rot_init, APPLICATION, 99);
