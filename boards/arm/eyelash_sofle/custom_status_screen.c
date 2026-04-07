/*
 * Custom status screen for Eyelash Sofle
 * Layout for 128x32 SSD1306 OLED (landscape)
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

static lv_obj_t *batt_label;
static lv_obj_t *conn_label;
static lv_obj_t *layer_label;
#if IS_ENABLED(CONFIG_ZMK_WPM)
static lv_obj_t *wpm_label;
#endif

/* ---- Battery Widget ---- */

struct batt_state {
    uint8_t level;
    bool charging;
};

static void batt_update(struct batt_state state) {
    char text[16];
    if (state.charging) {
        snprintf(text, sizeof(text), LV_SYMBOL_CHARGE " %d%%", state.level);
    } else if (state.level > 95) {
        snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_FULL " %d%%", state.level);
    } else if (state.level > 65) {
        snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_3 " %d%%", state.level);
    } else if (state.level > 35) {
        snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_2 " %d%%", state.level);
    } else if (state.level > 5) {
        snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_1 " %d%%", state.level);
    } else {
        snprintf(text, sizeof(text), LV_SYMBOL_BATTERY_EMPTY " %d%%", state.level);
    }
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

/* ---- Connection Widget ---- */

struct conn_state {
    struct zmk_endpoint_instance endpoint;
    bool connected;
};

static void conn_update(struct conn_state state) {
    char text[16];
    switch (state.endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        snprintf(text, sizeof(text), LV_SYMBOL_USB " USB");
        break;
    case ZMK_TRANSPORT_BLE:
        if (state.connected) {
            snprintf(text, sizeof(text), LV_SYMBOL_WIFI " BT%d",
                     state.endpoint.ble.profile_index + 1);
        } else {
            snprintf(text, sizeof(text), LV_SYMBOL_CLOSE " BT%d",
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
        .endpoint = zmk_endpoints_selected(),
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

/* ---- Layer Widget ---- */

struct layer_state {
    uint8_t index;
    const char *name;
};

static void layer_update(struct layer_state state) {
    char text[24];
    if (state.name && state.name[0]) {
        snprintf(text, sizeof(text), "%s", state.name);
    } else {
        snprintf(text, sizeof(text), "L%d", state.index);
    }
    lv_label_set_text(layer_label, text);
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
    char text[12];
    snprintf(text, sizeof(text), "%d wpm", state.wpm);
    lv_label_set_text(wpm_label, text);
}

static struct wpm_state wpm_get_state(const zmk_event_t *eh) {
    return (struct wpm_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(wpm_wid, struct wpm_state, wpm_update, wpm_get_state)
ZMK_SUBSCRIPTION(wpm_wid, zmk_wpm_state_changed);
#endif

/* ---- Screen Layout (128x32 landscape) ---- */

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * 128x32 layout:
     * Row 1 (y=0):  [battery icon+%]        [connection]
     * Row 2 (y=16): [layer name]             [WPM]
     */

    /* Battery - top left */
    batt_label = lv_label_create(screen);
    lv_obj_set_style_text_font(batt_label, &lv_font_montserrat_12, 0);
    lv_obj_align(batt_label, LV_ALIGN_TOP_LEFT, 0, 0);
    batt_wid_init();

    /* Connection - top right */
    conn_label = lv_label_create(screen);
    lv_obj_set_style_text_font(conn_label, &lv_font_montserrat_12, 0);
    lv_obj_align(conn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    conn_wid_init();

    /* Layer - bottom left */
    layer_label = lv_label_create(screen);
    lv_obj_set_style_text_font(layer_label, &lv_font_montserrat_12, 0);
    lv_obj_align(layer_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    layer_wid_init();

    /* WPM - bottom right */
#if IS_ENABLED(CONFIG_ZMK_WPM)
    wpm_label = lv_label_create(screen);
    lv_obj_set_style_text_font(wpm_label, &lv_font_montserrat_12, 0);
    lv_obj_align(wpm_label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    wpm_wid_init();
#endif

    return screen;
}
