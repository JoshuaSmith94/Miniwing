/* Copyright 2020 JoshuaSmith94
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

// Layer definitions
enum ferris_layers {
    _COLEMAK,
    _NAV,
    _NUM,
    _MOU
};

// Home row mod key definitions
typedef enum {
    HR_SHIFT,
    HR_CTRL,
    HR_CMD,
    HR_ALT
} home_row_mod_t;

// Custom keycodes for home row mods
enum custom_keycodes {
    // Left hand space combos
    SP_A = SAFE_RANGE,  // Space + A = Shift
    SP_R,               // Space + R = Ctrl
    SP_S,               // Space + S = Cmd
    SP_T,               // Space + T = Alt
    
    // Right hand backspace combos
    BS_N,               // Backspace + N = Alt
    BS_E,               // Backspace + E = Cmd
    BS_I,               // Backspace + I = Ctrl
    BS_O                // Backspace + O = Shift
};

// State tracking
typedef struct {
    bool space_pressed;
    bool backspace_pressed;
    bool left_home_key_pressed;
    bool right_home_key_pressed;
    bool nav_active;
    bool num_active;
    uint8_t mod_activations[4];  // shift, ctrl, cmd, alt
} keyboard_state_t;

static keyboard_state_t kb_state = {0};

// Base layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_COLEMAK] = LAYOUT(
        UK_Q,    UK_W,    UK_F,    UK_P,    UK_G, 
        SP_A,    SP_R,    SP_S,    SP_T,    UK_D, 
        UK_Z,    UK_X,    UK_C,    UK_V,    UK_B, 
                          KC_SPC,           MO(_NUM),
        UK_J,    UK_L,    UK_U,    UK_Y,    UK_SCLN,
        UK_H,    BS_N,    BS_E,    BS_I,    BS_O,
        UK_K,    UK_M,    UK_COMM, UK_DOT,  UK_SLSH,
        MO(_NAV),         KC_BSPC
    ),
    [_NAV] = LAYOUT(
        UK_Q,    UK_W,    UK_F,    UK_P,    UK_G,     
        SP_A,    SP_R,    SP_S,    SP_T,    UK_D,      
        UK_Z,    UK_X,    UK_C,    UK_V,    UK_B,      
                          KC_SPC,           MO(_MOU),
        KC_ESC,   KC_HOME, KC_UP,   KC_END,  KC_PGUP,  
        KC_TAB,   KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN,
        KC_DEL,   KC_ENT,  UK_LBRC, UK_RBRC, UK_GRV,
        _______,           _______
    ),
    [_NUM] = LAYOUT(
        UK_5,    UK_6,    UK_7,    UK_8,    UK_9,       
        UK_4,    UK_3,    UK_2,    UK_1,    UK_0,       
        UK_BSLS, UK_HASH, UK_EQL,  UK_MINS, UK_QUOT,    
                          _______,          _______,    
        UK_J,    UK_L,    UK_U,    UK_Y,    UK_SCLN,
        UK_H,    BS_N,    BS_E,    BS_I,    BS_O,
        UK_K,    UK_M,    UK_COMM, UK_DOT,  UK_SLSH,
        MO(_MOU),         KC_BSPC
    ),
    [_MOU] = LAYOUT(
        KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,      
        KC_F4,   KC_F3,   KC_F2,   KC_F1,   KC_F10,     
        KC_CAPS, KC_INS,  KC_PSCR, KC_F12,  KC_F11,     
                          _______,          _______,    
        KC_WH_U, KC_WH_L, KC_MS_U, KC_WH_R, KC_BTN3,
        KC_WH_D, KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN1,
        KC_MUTE, KC_MPRV, KC_MPLY, KC_MNXT, KC_BTN2,
        _______,          _______
    )
};

// Modifier management functions
static void toggle_modifier(home_row_mod_t mod, bool activate) {
    uint16_t kc;
    switch (mod) {
        case HR_SHIFT: kc = KC_LSFT; break;
        case HR_CTRL:  kc = KC_LCTL; break;
        case HR_CMD:   kc = KC_LCMD; break;
        case HR_ALT:   kc = KC_LALT; break;
        default: return;
    }
    
    if (activate) {
        if (kb_state.mod_activations[mod] == 0) {
            register_code(kc);
        }
        kb_state.mod_activations[mod]++;
    } else {
        if (kb_state.mod_activations[mod] > 0) {
            kb_state.mod_activations[mod]--;
            if (kb_state.mod_activations[mod] == 0) {
                unregister_code(kc);
            }
        }
    }
}

// Clean up modifiers when releasing space or backspace
static void cleanup_modifiers(void) {
    if (!kb_state.space_pressed && !kb_state.backspace_pressed) {
        for (int i = 0; i < 4; i++) {
            kb_state.mod_activations[i] = 0;
        }
        unregister_code(KC_LSFT);
        unregister_code(KC_LCTL);
        unregister_code(KC_LCMD);
        unregister_code(KC_LALT);
    }
}

// Process space and backspace taps
static bool process_dual_role_key(uint16_t keycode, keyrecord_t *record, 
                                bool *key_pressed, bool *home_key_pressed, 
                                uint16_t tap_code_key) {
    if (record->event.pressed) {
        *key_pressed = true;
    } else {
        *key_pressed = false;
        if (!*home_key_pressed) {
            tap_code(tap_code_key);
        }
        cleanup_modifiers();
        *home_key_pressed = false;
    }
    return false;
}

// Process home row mods
static bool process_home_row_mod(uint16_t keycode, keyrecord_t *record,
                               bool space_mod, bool *home_key_pressed,
                               uint16_t base_keycode, home_row_mod_t mod) {
    if (record->event.pressed) {
        if ((space_mod && kb_state.space_pressed) || (!space_mod && kb_state.backspace_pressed)) {
            toggle_modifier(mod, true);
            *home_key_pressed = true;
        } else {
            register_code(base_keycode);
        }
    } else {
        if ((space_mod && (kb_state.space_pressed || kb_state.left_home_key_pressed)) ||
            (!space_mod && (kb_state.backspace_pressed || kb_state.right_home_key_pressed))) {
            toggle_modifier(mod, false);
        }
        unregister_code(base_keycode);
    }
    return false;
}

// Handle layer switching
static bool process_layer_switch(uint16_t keycode, keyrecord_t *record,
                               bool *active_flag, uint8_t target_layer,
                               bool *other_active, uint8_t other_layer) {
    if (record->event.pressed) {
        *active_flag = true;
        if (*other_active) {
            layer_on(_MOU);
        } else {
            layer_on(target_layer);
        }
    } else {
        *active_flag = false;
        layer_off(target_layer);
        if (*other_active) {
            layer_on(other_layer);
        } else {
            layer_off(_MOU);
        }
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle space and backspace
    if (keycode == KC_SPC) {
        return process_dual_role_key(keycode, record, &kb_state.space_pressed,
                                   &kb_state.left_home_key_pressed, KC_SPC);
    }
    if (keycode == KC_BSPC) {
        return process_dual_role_key(keycode, record, &kb_state.backspace_pressed,
                                   &kb_state.right_home_key_pressed, KC_BSPC);
    }

    // Handle home row mods
    switch (keycode) {
        case SP_A: return process_home_row_mod(keycode, record, true,  &kb_state.left_home_key_pressed,  KC_A, HR_SHIFT);
        case SP_R: return process_home_row_mod(keycode, record, true,  &kb_state.left_home_key_pressed,  KC_R, HR_CTRL);
        case SP_S: return process_home_row_mod(keycode, record, true,  &kb_state.left_home_key_pressed,  KC_S, HR_CMD);
        case SP_T: return process_home_row_mod(keycode, record, true,  &kb_state.left_home_key_pressed,  KC_T, HR_ALT);
        case BS_N: return process_home_row_mod(keycode, record, false, &kb_state.right_home_key_pressed, KC_N, HR_ALT);
        case BS_E: return process_home_row_mod(keycode, record, false, &kb_state.right_home_key_pressed, KC_E, HR_CMD);
        case BS_I: return process_home_row_mod(keycode, record, false, &kb_state.right_home_key_pressed, KC_I, HR_CTRL);
        case BS_O: return process_home_row_mod(keycode, record, false, &kb_state.right_home_key_pressed, KC_O, HR_SHIFT);
        
        case MO(_NAV):
            return process_layer_switch(keycode, record, &kb_state.nav_active, _NAV,
                                      &kb_state.num_active, _NUM);
        case MO(_NUM):
            return process_layer_switch(keycode, record, &kb_state.num_active, _NUM,
                                      &kb_state.nav_active, _NAV);
    }
    
    return true;
}

void matrix_init_user(void) {
    layer_clear();
    layer_on(_COLEMAK_DH);
}