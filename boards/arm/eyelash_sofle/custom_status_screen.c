/*
 * Custom portrait status screen for Eyelash Sofle
 * Designed for vertically-mounted 128x32 SSD1306 OLED
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <lvgl.h>
#include <zmk/display/status_screen.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/battery.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/endpoint_changed.h>

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_BLE)
#include <zmk/ble.h>
#include <zmk/events/ble_active_profile_changed.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_WPM)
#include <zmk/wpm.h>
#include <zmk/events/wpm_state_changed.h>
#endif

LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);

static lv_obj_t *conn_label;
static lv_obj_t *batt_label;
static lv_obj_t *layer_label;
#if IS_ENABLED(CONFIG_ZMK_WPM)
static lv_obj_t *wpm_num_label;
static lv_obj_t *wpm_text_label;
#endif

/* ---- Connection Widget ---- */

struct conn_state {
    struct zmk_endpoint_instance endpoint;
    bool connected;
};

static void conn_update(struct conn_state state) {
    char text[16];
    switch (state.endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        snprintf(text, sizeof(text), LV_SYMBOL_USB "\nUSB");
        break;
    case ZMK_TRANSPORT_BLE:
        if (state.connected) {
            snprintf(text, sizeof(text), LV_SYMBOL_WIFI "\nBT%d",
                     state.endpoint.ble.profile_index + 1);
        } else {
            snprintf(text, sizeof(text), LV_SYMBOL_CLOSE "\nBT%d",
                     state.endpoint.ble.profile_index + 1);
        }
        break;
    default:
        snprintf(text, sizeof(text), "---");
    }
    lv_label_set_text(conn_label, text);
}

static struct conn_state conn_get_state(const zmk_event_t *eh) {
    return (struct conn_state){
        .endpoint = zmk_endpoint_get_selected(),
#if IS_ENABLED(CONFIG_ZMK_BLE)
        .connected = zmk_ble_active_profile_is_connected(),
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(conn_wid, struct conn_state, conn_update, conn_get_state)
ZMK_SUBSCRIPTION(conn_wid, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(conn_wid, zmk_ble_active_profile_changed);
#endif
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(conn_wid, zmk_usb_conn_state_changed);
#endif

/* ---- Battery Widget ---- */

struct batt_state {
    uint8_t level;
    bool charging;
};

static void batt_update(struct batt_state state) {
    char text[16];
    const char *icon;
    if (state.charging) {
        icon = LV_SYMBOL_CHARGE;
    } else if (state.level > 95) {
        icon = LV_SYMBOL_BATTERY_FULL;
    } else if (state.level > 65) {
        icon = LV_SYMBOL_BATTERY_3;
    } else if (state.level > 35) {
        icon = LV_SYMBOL_BATTERY_2;
    } else if (state.level > 5) {
        icon = LV_SYMBOL_BATTERY_1;
    } else {
        icon = LV_SYMBOL_BATTERY_EMPTY;
    }
    snprintf(text, sizeof(text), "%s\n%d%%", icon, state.level);
    lv_label_set_text(batt_label, text);
}

static struct batt_state batt_get_state(const zmk_event_t *eh) {
    return (struct batt_state){
        .level = zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .charging = zmk_usb_is_powered(),
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(batt_wid, struct batt_state, batt_update, batt_get_state)
ZMK_SUBSCRIPTION(batt_wid, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(batt_wid, zmk_usb_conn_state_changed);
#endif

/* ---- Layer Widget ---- */

struct layer_state {
    uint8_t index;
    const char *name;
};

static void layer_update(struct layer_state state) {
    if (state.name && state.name[0]) {
        lv_label_set_text(layer_label, state.name);
    } else {
        char text[8];
        snprintf(text, sizeof(text), "L%d", state.index);
        lv_label_set_text(layer_label, text);
    }
}

static struct layer_state layer_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_state){
        .index = index,
        .name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(layer_wid, struct layer_state, layer_update, layer_get_state)
ZMK_SUBSCRIPTION(layer_wid, zmk_layer_state_changed);

/* ---- WPM Widget ---- */

#if IS_ENABLED(CONFIG_ZMK_WPM)
struct wpm_state {
    int wpm;
};

static void wpm_update(struct wpm_state state) {
    char text[8];
    snprintf(text, sizeof(text), "%d", state.wpm);
    lv_label_set_text(wpm_num_label, text);
}

static struct wpm_state wpm_get_state(const zmk_event_t *eh) {
    return (struct wpm_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(wpm_wid, struct wpm_state, wpm_update, wpm_get_state)
ZMK_SUBSCRIPTION(wpm_wid, zmk_wpm_state_changed);
#endif

/* ---- Screen Layout ---- */

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    /* Rotate for portrait: 128x32 physical -> 32x128 logical */
    lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_90);

    /* Connection - top (y 0-30) */
    conn_label = lv_label_create(screen);
    lv_obj_set_width(conn_label, 32);
    lv_obj_set_style_text_align(conn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(conn_label, &lv_font_montserrat_12, 0);
    lv_obj_align(conn_label, LV_ALIGN_TOP_MID, 0, 0);
    conn_wid_init();

    /* Battery (y 32-62) */
    batt_label = lv_label_create(screen);
    lv_obj_set_width(batt_label, 32);
    lv_obj_set_style_text_align(batt_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(batt_label, &lv_font_montserrat_12, 0);
    lv_obj_align(batt_label, LV_ALIGN_TOP_MID, 0, 32);
    batt_wid_init();

    /* Layer (y 64-84) */
    layer_label = lv_label_create(screen);
    lv_obj_set_width(layer_label, 32);
    lv_label_set_long_mode(layer_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(layer_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(layer_label, &lv_font_montserrat_12, 0);
    lv_obj_align(layer_label, LV_ALIGN_TOP_MID, 0, 68);
    layer_wid_init();

    /* WPM (y 90-128) */
#if IS_ENABLED(CONFIG_ZMK_WPM)
    wpm_num_label = lv_label_create(screen);
    lv_obj_set_width(wpm_num_label, 32);
    lv_obj_set_style_text_align(wpm_num_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(wpm_num_label, &lv_font_montserrat_16, 0);
    lv_obj_align(wpm_num_label, LV_ALIGN_TOP_MID, 0, 92);

    wpm_text_label = lv_label_create(screen);
    lv_obj_set_width(wpm_text_label, 32);
    lv_obj_set_style_text_align(wpm_text_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(wpm_text_label, &lv_font_montserrat_12, 0);
    lv_label_set_text(wpm_text_label, "wpm");
    lv_obj_align(wpm_text_label, LV_ALIGN_TOP_MID, 0, 112);

    wpm_wid_init();
#endif

    return screen;
}
