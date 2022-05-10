/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

enum custom_keycodes {
    KC_EMOD = SAFE_RANGE
};

enum {
    BASE = 0,
    FN = 1,
    EMOD = 2,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,           KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_EMOD, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(FN),  KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [FN] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, _______, _______, _______, _______, KC_PSCR,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RESET,            _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, _______, _______, _______, _______, NK_ON,   NK_OFF,  _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    [EMOD] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT, _______, _______,          _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                            KC_UNDS,                            _______, _______, _______, _______, _______, _______
    ),
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool left_shift_pressed = false;
    static bool right_shift_pressed = false;
    static bool emod_used = false;

    if (record->event.pressed && layer_state_is(EMOD)) {
        emod_used = true;
    }
    
    bool *shift_pressed = &right_shift_pressed;

    switch (keycode) {
        case KC_LSFT:
            shift_pressed = &left_shift_pressed;
            // fallthrough
        case KC_RSFT:
            *shift_pressed = record->event.pressed;
            if (left_shift_pressed && right_shift_pressed) tap_code(KC_CAPS);
            break;

        case KC_EMOD:
            if (record->event.pressed) {
                emod_used = false;
                layer_on(EMOD);
            } else {
                layer_off(EMOD);
                if (!emod_used) tap_code(KC_ESC);
            }
            break;
        
        default:
            break;
    }
    return true;
}

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    clockwise = !clockwise; // my rotary encoder's pinout is backwards
    
    uint8_t mods = get_mods();
    
    if (layer_state_is(EMOD)) {
        uint16_t k = clockwise ? KC_DOWN : KC_UP;
        uint8_t n = mods & MOD_MASK_ALT ? 12 : 1;
        unregister_mods(MOD_MASK_ALT);
        for (uint8_t i = 0; i < n; i++) {
            tap_code(k);
        }
        set_mods(mods);
    } else if (mods & (MOD_MASK_CTRL | MOD_MASK_ALT)) {
        if (!clockwise) {
            register_mods(MOD_MASK_SHIFT);
        }
        tap_code(KC_TAB);
        set_mods(mods);
    } else if (mods & MOD_MASK_SHIFT) {
        unregister_mods(MOD_MASK_SHIFT);
        register_mods(MOD_MASK_CTRL);
        tap_code(clockwise ? KC_EQUAL : KC_MINUS);
        set_mods(mods);
    } else {
        tap_code(clockwise ? KC_VOLU : KC_VOLD);
    }

    return false;
}
#endif // ENCODER_ENABLE
