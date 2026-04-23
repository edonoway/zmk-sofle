# ZMK Keymap Reference Guide

Complete reference for key mapping on the Eyelash Sofle (ZMK v0.3.0).

---

## Table of Contents

- [Behaviors (Key Actions)](#behaviors)
- [Key Codes](#key-codes)
- [Modifier Combinations](#modifier-combinations)
- [Layer Navigation](#layer-navigation)
- [Macros](#macros)
- [Combos](#combos)
- [Conditional Layers](#conditional-layers)
- [Bluetooth & System](#bluetooth--system)
- [Mouse Emulation](#mouse-emulation)
- [RGB Underglow](#rgb-underglow)
- [Encoder / Sensor Bindings](#encoder--sensor-bindings)
- [macOS Function Keys](#macos-function-keys)

---

## Behaviors

Behaviors are actions assigned to keys. Every key binding in the keymap uses a behavior.

### Basic

| Behavior | Syntax | Description |
|----------|--------|-------------|
| Key Press | `&kp <KEY>` | Press and release a key |
| Transparent | `&trans` | Falls through to next active layer |
| None | `&none` | Swallows the keypress (does nothing) |

### Tap vs Hold (Dual-Function Keys)

| Behavior | Syntax | Tap | Hold |
|----------|--------|-----|------|
| Layer-Tap | `&lt <layer> <key>` | Sends `key` | Activates `layer` |
| Mod-Tap | `&mt <modifier> <key>` | Sends `key` | Holds `modifier` |

```
// Examples:
&lt 2 SPACE       // Tap=Space, Hold=Layer 2
&mt LSHFT A        // Tap=A, Hold=Left Shift
&mt LGUI ESC       // Tap=Escape, Hold=Cmd
```

### Sticky (One-Shot)

| Behavior | Syntax | Description |
|----------|--------|-------------|
| Sticky Key | `&sk <modifier>` | Modifier active for the next keypress only |
| Sticky Layer | `&sl <layer>` | Layer active for the next keypress only |

```
&sk LSHFT          // Next key will be shifted, then Shift releases
&sl 2              // Next key comes from layer 2, then returns
```

### Special Keys

| Behavior | Syntax | Description |
|----------|--------|-------------|
| Grave Escape | `&gresc` | ESC normally; `` ` `` with Shift or GUI held |
| Caps Word | `&caps_word` | Auto-shift letters until a non-alpha key is pressed |
| Key Repeat | `&key_repeat` | Repeats the last keycode sent |
| Key Toggle | `&kt <key>` | First press holds key, second press releases |

### Advanced Behaviors

**Tap Dance** — different actions based on number of taps:
```
behaviors {
    td_caps: tap_dance_caps {
        compatible = "zmk,behavior-tap-dance";
        #binding-cells = <0>;
        tapping-term-ms = <200>;
        bindings = <&kp LSHFT>, <&kp CAPS>;
    };
};
// Usage: &td_caps  (1 tap = Shift, 2 taps = Caps Lock)
```

**Mod-Morph** — different action depending on active modifiers:
```
behaviors {
    bspc_del: backspace_delete {
        compatible = "zmk,behavior-mod-morph";
        #binding-cells = <0>;
        bindings = <&kp BACKSPACE>, <&kp DELETE>;
        mods = <(MOD_LSFT|MOD_RSFT)>;
    };
};
// Usage: &bspc_del  (normal = Backspace, with Shift = Delete)
```

Modifier flags for mod-morph: `MOD_LCTL`, `MOD_LSFT`, `MOD_LALT`, `MOD_LGUI`, `MOD_RCTL`, `MOD_RSFT`, `MOD_RALT`, `MOD_RGUI`

---

## Key Codes

All key codes are used with `&kp`, e.g., `&kp A`, `&kp SPACE`.

### Letters
```
A  B  C  D  E  F  G  H  I  J  K  L  M
N  O  P  Q  R  S  T  U  V  W  X  Y  Z
```

### Numbers (and their shifted symbols)

| Key | Aliases | Shifted | Shifted Aliases |
|-----|---------|---------|-----------------|
| `NUMBER_1` | `N1` | `EXCLAMATION` | `EXCL` |
| `NUMBER_2` | `N2` | `AT_SIGN` | `AT` |
| `NUMBER_3` | `N3` | `HASH` | `POUND` |
| `NUMBER_4` | `N4` | `DOLLAR` | `DLLR` |
| `NUMBER_5` | `N5` | `PERCENT` | `PRCNT` |
| `NUMBER_6` | `N6` | `CARET` | — |
| `NUMBER_7` | `N7` | `AMPERSAND` | `AMPS` |
| `NUMBER_8` | `N8` | `ASTERISK` | `STAR` |
| `NUMBER_9` | `N9` | `LEFT_PARENTHESIS` | `LPAR` |
| `NUMBER_0` | `N0` | `RIGHT_PARENTHESIS` | `RPAR` |

### Symbols / Punctuation

| Unshifted | Aliases | Shifted | Shifted Aliases |
|-----------|---------|---------|-----------------|
| `MINUS` | — | `UNDERSCORE` | `UNDER` |
| `EQUAL` | `EQL` | `PLUS` | — |
| `LEFT_BRACKET` | `LBKT` | `LEFT_BRACE` | `LBRC` |
| `RIGHT_BRACKET` | `RBKT` | `RIGHT_BRACE` | `RBRC` |
| `BACKSLASH` | `BSLH` | `PIPE` | — |
| `SEMICOLON` | `SEMI` | `COLON` | — |
| `SINGLE_QUOTE` | `APOS`, `SQT` | `DOUBLE_QUOTES` | `DQT` |
| `GRAVE` | — | `TILDE` | — |
| `COMMA` | — | `LESS_THAN` | `LT` |
| `PERIOD` | `DOT` | `GREATER_THAN` | `GT` |
| `SLASH` | `FSLH` | `QUESTION` | `QMARK` |

### Modifiers

| Key | Aliases |
|-----|---------|
| `LEFT_CONTROL` | `LCTRL` |
| `LEFT_SHIFT` | `LSHFT`, `LSFT` |
| `LEFT_ALT` | `LALT` |
| `LEFT_GUI` | `LGUI`, `LCMD`, `LMETA`, `LWIN` |
| `RIGHT_CONTROL` | `RCTRL` |
| `RIGHT_SHIFT` | `RSHFT`, `RSFT` |
| `RIGHT_ALT` | `RALT` |
| `RIGHT_GUI` | `RGUI`, `RCMD`, `RMETA`, `RWIN` |

### Function Keys
```
F1  F2  F3  F4  F5  F6  F7  F8  F9  F10  F11  F12
F13 F14 F15 F16 F17 F18 F19 F20 F21 F22  F23  F24
```

### Navigation

| Key | Aliases |
|-----|---------|
| `UP_ARROW` | `UP` |
| `DOWN_ARROW` | `DOWN` |
| `LEFT_ARROW` | `LEFT` |
| `RIGHT_ARROW` | `RIGHT` |
| `HOME` | — |
| `END` | — |
| `PAGE_UP` | `PG_UP` |
| `PAGE_DOWN` | `PG_DN` |

### Editing / Special

| Key | Aliases |
|-----|---------|
| `BACKSPACE` | `BSPC`, `BKSP` |
| `DELETE` | `DEL` |
| `INSERT` | `INS` |
| `TAB` | — |
| `RETURN` / `ENTER` | `RET` |
| `SPACE` | `SPC` |
| `ESCAPE` | `ESC` |
| `CAPSLOCK` | `CAPS` |
| `PRINTSCREEN` | `PSCRN` |
| `SCROLLLOCK` | `SLCK` |
| `PAUSE_BREAK` | `PAUS` |
| `K_APPLICATION` | `K_APP` (context menu) |

### Keypad

| Key | Aliases |
|-----|---------|
| `KP_NUMLOCK` | `KP_NUM` |
| `KP_DIVIDE` | `KP_SLASH` |
| `KP_MULTIPLY` | `KP_ASTERISK` |
| `KP_MINUS` | `KP_SUBTRACT` |
| `KP_PLUS` | — |
| `KP_ENTER` | — |
| `KP_NUMBER_0` – `KP_NUMBER_9` | `KP_N0` – `KP_N9` |
| `KP_DOT` | — |
| `KP_EQUAL` | — |

---

## Modifier Combinations

Wrap a key code in a modifier function to send a chord with a single binding:

| Function | Modifier |
|----------|----------|
| `LC(key)` | Left Control |
| `LS(key)` | Left Shift |
| `LA(key)` | Left Alt/Option |
| `LG(key)` | Left GUI/Cmd |
| `RC(key)` | Right Control |
| `RS(key)` | Right Shift |
| `RA(key)` | Right Alt/Option |
| `RG(key)` | Right GUI/Cmd |

**These can be nested** to combine multiple modifiers:

```
&kp LS(A)              // Shift+A (uppercase A)
&kp LG(C)              // Cmd+C (copy)
&kp LG(V)              // Cmd+V (paste)
&kp LG(X)              // Cmd+X (cut)
&kp LG(Z)              // Cmd+Z (undo)
&kp LG(LS(Z))          // Cmd+Shift+Z (redo)
&kp LG(SPACE)          // Cmd+Space (Spotlight)
&kp LG(LS(N4))         // Cmd+Shift+4 (screenshot)
&kp LA(BACKSPACE)       // Option+Backspace (delete word)
&kp LG(BACKSPACE)       // Cmd+Backspace (delete line / delete to start)
&kp LG(LC(Q))          // Cmd+Ctrl+Q (lock screen)
&kp LC(LS(LG(N4)))     // Ctrl+Shift+Cmd+4 (screenshot to clipboard)
&kp LA(LG(ESC))        // Option+Cmd+Escape (force quit)
```

---

## Layer Navigation

| Behavior | Syntax | Description |
|----------|--------|-------------|
| Momentary | `&mo <n>` | Layer active **while key is held** |
| Toggle | `&tog <n>` | Tap to turn layer on; tap again to turn off |
| To | `&to <n>` | Switch to layer, deactivating all others |
| Layer-Tap | `&lt <n> <key>` | Hold = activate layer; Tap = send key |
| Sticky Layer | `&sl <n>` | Layer active for the **next keypress only** |

**Important**: Higher-numbered layers override lower ones. If layer 3 and layer 1 are both active, layer 3's bindings take priority. Use `&trans` on higher layers to let keys fall through.

### Toggle vs Momentary vs To

- **`&mo`**: Layer is active only while the key is physically held down. Release = back to base. Best for temporary layers (function keys, nav).
- **`&tog`**: Tap once to activate, tap again to deactivate. Layer stays on when you let go. Best for alternate base layouts (QWERTY ↔ Colemak).
- **`&to`**: Switches to a layer and deactivates everything else except layer 0. More aggressive than toggle — rarely needed.

---

## Macros

Macros let you assign a sequence of key actions to a single key.

### Basic Structure

```
macros {
    my_macro: my_macro {
        compatible = "zmk,behavior-macro";
        #binding-cells = <0>;
        wait-ms = <40>;           // delay between actions (optional, default 100)
        tap-ms = <40>;            // how long each tap is held (optional, default 100)
        bindings = <...>;
    };
};
// Usage in keymap: &my_macro
```

### Macro Control Keywords

| Keyword | Description |
|---------|-------------|
| `&macro_tap` | (Default) Press and release each subsequent key |
| `&macro_press` | Only press (hold down) subsequent keys |
| `&macro_release` | Only release subsequent keys |
| `&macro_pause_for_release` | Pause until the macro key itself is released |
| `&macro_wait_time <ms>` | Change delay between actions mid-macro |
| `&macro_tap_time <ms>` | Change tap hold duration mid-macro |

### Common Patterns

**Type a string:**
```
bindings = <&kp H &kp E &kp L &kp L &kp O>;
```

**Keyboard shortcut (press modifiers → tap key → release modifiers):**
```
// Cmd+Ctrl+Q (lock screen)
bindings =
    <&macro_press &kp LGUI &kp LCTRL>,
    <&macro_tap &kp Q>,
    <&macro_release &kp LGUI &kp LCTRL>;
```

**Hold-while-pressed (modifier active while macro key held):**
```
// Hold Shift while macro key is held
bindings =
    <&macro_press &kp LSHFT>,
    <&macro_pause_for_release>,
    <&macro_release &kp LSHFT>;
```

**Multi-step sequence with delays:**
```
bindings =
    <&macro_wait_time 100>,
    <&kp L &kp O &kp L>,
    <&macro_wait_time 500>,
    <&kp ENTER>;
```

**Note**: For simple modifier+key combos, you don't need a macro — just use `&kp LG(LC(Q))`. Macros are for multi-step sequences or hold-while-pressed patterns.

---

## Combos

Combos trigger an action when multiple keys are pressed simultaneously.

```
/ {
    combos {
        compatible = "zmk,combos";

        my_combo {
            key-positions = <0 1>;         // key indices to press together
            bindings = <&kp ESC>;          // action to trigger
            timeout-ms = <50>;             // ms window for all keys (default 50)
            layers = <0 1>;                // (optional) restrict to these layers
            require-prior-idle-ms = <150>; // (optional) prevent during fast typing
            slow-release;                  // (optional) hold until ALL keys released
        };
    };
};
```

Key positions are 0-indexed, matching the order keys appear in your keymap's `bindings` array.

---

## Conditional Layers

Automatically activate a layer when multiple other layers are all active (e.g., "tri-layer"):

```
/ {
    conditional_layers {
        compatible = "zmk,conditional-layers";

        adjust_layer {
            if-layers = <2 3>;      // when BOTH layer 2 AND layer 3 are active
            then-layer = <4>;       // activate layer 4
        };
    };
};
```

Common use: `&mo 2` on left thumb + `&mo 3` on right thumb → holding both activates layer 4.

---

## Bluetooth & System

Requires: `#include <dt-bindings/zmk/bt.h>`

| Action | Binding |
|--------|---------|
| Select profile 0–4 | `&bt BT_SEL 0` through `&bt BT_SEL 4` |
| Clear current profile | `&bt BT_CLR` |
| Clear all profiles | `&bt BT_CLR_ALL` |
| Next profile | `&bt BT_NXT` |
| Previous profile | `&bt BT_PRV` |
| Disconnect profile n | `&bt BT_DISC 0` |

Requires: `#include <dt-bindings/zmk/outputs.h>`

| Action | Binding |
|--------|---------|
| Force USB output | `&out OUT_USB` |
| Force BLE output | `&out OUT_BLE` |
| Toggle USB/BLE | `&out OUT_TOG` |

### System Controls

| Action | Binding |
|--------|---------|
| Reset keyboard | `&sys_reset` |
| Enter bootloader | `&bootloader` |
| Power off (deep sleep) | `&soft_off` |

---

## Mouse Emulation

Requires: `#include <dt-bindings/zmk/pointing.h>` and `CONFIG_ZMK_POINTING=y`

### Mouse Buttons

| Button | Binding |
|--------|---------|
| Left click | `&mkp LCLK` (or `MB1`) |
| Right click | `&mkp RCLK` (or `MB2`) |
| Middle click | `&mkp MCLK` (or `MB3`) |
| Back | `&mkp MB4` |
| Forward | `&mkp MB5` |

### Mouse Movement

```
&mmv MOVE_UP       &mmv MOVE_DOWN
&mmv MOVE_LEFT     &mmv MOVE_RIGHT
```

### Mouse Scroll

```
&msc SCRL_UP       &msc SCRL_DOWN
&msc SCRL_LEFT     &msc SCRL_RIGHT
```

---

## RGB Underglow

Requires: `#include <dt-bindings/zmk/rgb.h>`

| Action | Binding |
|--------|---------|
| Toggle on/off | `&rgb_ug RGB_TOG` |
| Turn on | `&rgb_ug RGB_ON` |
| Turn off | `&rgb_ug RGB_OFF` |
| Brightness up | `&rgb_ug RGB_BRI` |
| Brightness down | `&rgb_ug RGB_BRD` |
| Hue up | `&rgb_ug RGB_HUI` |
| Hue down | `&rgb_ug RGB_HUD` |
| Saturation up | `&rgb_ug RGB_SAI` |
| Saturation down | `&rgb_ug RGB_SAD` |
| Animation speed up | `&rgb_ug RGB_SPI` |
| Animation speed down | `&rgb_ug RGB_SPD` |
| Next effect | `&rgb_ug RGB_EFF` |
| Previous effect | `&rgb_ug RGB_EFR` |
| Set specific color | `&rgb_ug RGB_COLOR_HSB(h,s,b)` |

---

## Encoder / Sensor Bindings

Each layer can define what the rotary encoder does:

```
// Volume control
sensor-bindings = <&inc_dec_kp C_VOLUME_UP C_VOL_DN>;

// Scroll wheel
sensor-bindings = <&scroll_encoder>;   // uses custom behavior defined in keymap

// Page up/down
sensor-bindings = <&inc_dec_kp PG_UP PG_DN>;

// Arrow keys
sensor-bindings = <&inc_dec_kp RIGHT LEFT>;
```

Custom encoder behavior:
```
scroll_encoder: scroll_encoder {
    compatible = "zmk,behavior-sensor-rotate";
    #sensor-binding-cells = <0>;
    bindings = <&msc SCRL_DOWN>, <&msc SCRL_UP>;
    tap-ms = <100>;
};
```

---

## macOS Function Keys

ZMK codes for the Mac F-key row functions:

| Mac Function | ZMK Binding | Notes |
|-------------|-------------|-------|
| Brightness Down | `&kp C_BRI_DN` | Works natively |
| Brightness Up | `&kp C_BRI_UP` | Works natively |
| Mission Control | `&kp C_AC_DESKTOP_SHOW_ALL_WINDOWS` | Or `&kp LC(UP)` |
| Launchpad | `&kp C_AC_DESKTOP_SHOW_ALL_APPLICATIONS` | — |
| Media Previous | `&kp C_PREV` | Works natively |
| Play/Pause | `&kp C_PP` | Works natively |
| Media Next | `&kp C_NEXT` | Works natively |
| Mute | `&kp C_MUTE` | Works natively |
| Volume Down | `&kp C_VOL_DN` | Works natively |
| Volume Up | `&kp C_VOL_UP` | Works natively |
| Spotlight | `&kp C_AC_SEARCH` | Or `&kp LG(SPACE)` |
| Globe / Fn | `&kp GLOBE` | Input source / emoji panel |
| Lock Screen | `&kp LG(LC(Q))` | Cmd+Ctrl+Q |
| Force Quit | `&kp LG(LA(ESC))` | Cmd+Option+Escape |
| Screenshot (file) | `&kp LG(LS(N3))` | Cmd+Shift+3 |
| Screenshot (select) | `&kp LG(LS(N4))` | Cmd+Shift+4 |
| Screenshot (clipboard) | `&kp LC(LS(LG(N4)))` | Ctrl+Shift+Cmd+4 |
| Do Not Disturb | — | No HID code; set a shortcut in System Settings |
| Dictation | — | No HID code; default is double-press Globe |

**Note**: Extended consumer keys like `C_AC_DESKTOP_SHOW_ALL_WINDOWS` require that `CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC` is **not** set to `y` in your `.conf` file.

---

## Quick Reference: Your Current Layer Map

| Layer | # | Access | Purpose |
|-------|---|--------|---------|
| Colemak | 0 | Default | Base typing layout |
| QWERTY | 1 | `tog 1` (ESC key) | Alternate base layout |
| FN/NAV | 2 | `MO(2)` (left thumb) | Function keys, navigation, macros |
| (Free) | 3 | `MO(3)` (right thumb) | Available for customization |
| BT/SYS | 4 | Needs `&mo 4` binding | Bluetooth, system controls |
| (Spare) | 5 | — | Empty |
