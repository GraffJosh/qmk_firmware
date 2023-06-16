/* Copyright 2022 @ Keychron (https://www.keychron.com)
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

#include QMK_KEYBOARD_H
#include "../../config.h"
#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)
#define ON true
#define OFF false
#define TAPPING_TERM 200

// #define MAX_KEYS_REGISTERED 10
// clang-format off

static int layer_led_indicators[MAX_LAYERS]={
    NUMBER_ROW_0_LED_INDEX,
    NUMBER_ROW_1_LED_INDEX,
    NUMBER_ROW_2_LED_INDEX,
    NUMBER_ROW_3_LED_INDEX,
    NUMBER_ROW_4_LED_INDEX,
    NUMBER_ROW_5_LED_INDEX
    };
enum layers{
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN
};

typedef enum {
    off,
    off_oneshot,
    tap_oneshot,
    tap_down,
    tap,
    doubletap,  
    hold,
    hold_oneshot,
}keystate;

enum custom_keycodes {
    CUST_KEY_ALTTAB = SAFE_RANGE, //64
    CUST_KEY_TOGGLE_KVM,          //65  
    CUST_KEY_SPACE_REVERSE,       //66
    CUST_KEY_TD_KVM,              //67
    CUST_KEY_PGUP,                //68
    CUST_KEY_PGDN,                //69
    CUST_KEY_ESC_FN,              //70
    CUST_KEY_ACC_ENABLE,          //71
    CUST_KEY_WIN_ONE_NEWTAB,      //72
    CUST_KEY_WIN_TWO_CLOSETAB,    //73
    CUST_KEY_FIRST_TAB_NEXT,    //74
    CUST_KEY_BUILD_DEPLOY,        //75
    MAX_CUSTOM_KEYS,
};
// Backlight timeout feature
#define BACKLIGHT_TIMEOUT 1    // in minutes
static uint16_t idle_timer = 0;
static uint8_t halfmin_counter = 0;
static uint8_t old_backlight_level = -1; 
static uint8_t old_rgb_value = -1; 
static bool led_on = true;
static bool rgb_on = true;


static int kvm_selection = 2;
static bool tapdown_space_oneshot = false;

static int num_registered_keys = 0;
static uint16_t registered_key_states[MAX_CUSTOM_KEYS - SAFE_RANGE];
static uint16_t registered_hold_timers[MAX_CUSTOM_KEYS - SAFE_RANGE];
static uint16_t registered_tap_timers[MAX_CUSTOM_KEYS - SAFE_RANGE];
static uint16_t registered_tap_counts[MAX_CUSTOM_KEYS - SAFE_RANGE];
static layer_state_t current_layer_state;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_tkl_f13_ansi(
        CUST_KEY_TD_KVM,   CUST_KEY_TD_KVM,  KC_BRIU,  CUST_KEY_WIN_ONE_NEWTAB,    CUST_KEY_WIN_ONE_NEWTAB,    RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,    KC_MPLY,  CUST_KEY_WIN_ONE_NEWTAB,    CUST_KEY_WIN_TWO_CLOSETAB,    CUST_KEY_FIRST_TAB_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  CUST_KEY_ESC_FN,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 MO(3),  KC_MENU,  KC_RWIN, KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [1] = LAYOUT_tkl_f13_ansi(
        CUST_KEY_TD_KVM,   CUST_KEY_BUILD_DEPLOY,  KC_F2,  CUST_KEY_WIN_ONE_NEWTAB,    CUST_KEY_WIN_ONE_NEWTAB,    RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,    KC_MPLY,  CUST_KEY_WIN_ONE_NEWTAB,    CUST_KEY_WIN_TWO_CLOSETAB,    CUST_KEY_FIRST_TAB_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  CUST_KEY_ESC_FN,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 MO(3),  KC_MENU,  KC_RWIN, KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [2] = LAYOUT_tkl_f13_ansi(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,    RGB_TOG,  _______,  _______,  RGB_TOG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  _______,  KC_UP,  RGB_HUI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  KC_LEFT, KC_DOWN,  KC_RIGHT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  KC_MENU,  _______,    _______,  _______,  _______,  _______),

    [3] = LAYOUT_tkl_f13_ansi(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,    RGB_TOG,  _______,  _______,  RGB_TOG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    CUST_KEY_ACC_ENABLE,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  KC_MENU,  _______,    _______,  _______,  _______,  _______),
    [4] = LAYOUT_tkl_f13_ansi(
        _______,  KC_BRID,  KC_BRIU,  _______,  _______,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,    RGB_TOG,  _______,  _______,  RGB_TOG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______),
    [5] = LAYOUT_tkl_f13_ansi(
        _______,  KC_BRID,  KC_BRIU,  _______,  _______,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,    RGB_TOG,  _______,  _______,  RGB_TOG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][1][2] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_LEFT, KC_RIGHT) },
    [2] = { ENCODER_CCW_CW(KC_LEFT, KC_RIGHT) },
    [3] = { ENCODER_CCW_CW(KC_F14, KC_F15) },
    [4] = { ENCODER_CCW_CW(KC_F14, KC_F15) },
    [5] = { ENCODER_CCW_CW(KC_F14, KC_F15) }
};
#endif // ENCODER_MAP_ENABLE

void toggle_layer(int layer_num, bool enable)
{
    if (enable)
    {
        layer_on(layer_num);
    }else{
        if(IS_LAYER_ON_STATE(current_layer_state, layer_num))
        {
            layer_off(layer_num);
        }
    }
}
void toggle_kvm(void)
{
    if(kvm_selection != 1)
    {
        register_code(KC_LSFT);

        register_code(KC_NUM_LOCK);
        unregister_code(KC_NUM_LOCK);
        register_code(KC_NUM_LOCK);
        unregister_code(KC_NUM_LOCK);

        unregister_code(KC_LSFT);

        register_code(KC_1);
        unregister_code(KC_1);

        rgb_matrix_set_color(MACRO_ROW_2_LED_INDEX, RGB_BLACK);

        layer_on(1);
        kvm_selection = 1;
    }else{
        register_code(KC_LSFT);

        register_code(KC_NUM_LOCK);
        unregister_code(KC_NUM_LOCK);
        register_code(KC_NUM_LOCK);
        unregister_code(KC_NUM_LOCK);

        unregister_code(KC_LSFT);

        register_code(KC_2);
        unregister_code(KC_2);
        rgb_matrix_set_color(MACRO_ROW_1_LED_INDEX, RGB_BLACK);
        
        layer_off(1);
        kvm_selection = 2;
    }
};

int get_registered_key_index(uint16_t keycode)
{
    return keycode - SAFE_RANGE;
}

keystate get_registered_key_state(uint16_t keycode)
{
    return registered_key_states[get_registered_key_index(keycode)];
}


int set_registered_key_state(uint16_t keycode, keyrecord_t *keyevent)
{
    static int curr_reg = -1;
    if (curr_reg == -1)
    {
        for (int key=0; key < num_registered_keys; key++)
        {      
            registered_tap_timers[key] = 0;
            registered_key_states[key] = off;
        }
    }

    curr_reg = get_registered_key_index(keycode);
    if(keyevent->event.pressed) 
    {   
            registered_hold_timers[curr_reg] = timer_read();
            if (tapdown_space_oneshot == true && registered_key_states[curr_reg] != tap_down){
                tapdown_space_oneshot = false;
            }   
            if (registered_key_states[curr_reg] != tap_down){
                registered_key_states[curr_reg] = tap_oneshot;
            }
            if (timer_elapsed(registered_tap_timers[curr_reg]) > TAPPING_TERM || !registered_tap_timers[curr_reg])
            {
                registered_tap_timers[curr_reg] = timer_read();
                registered_tap_counts[curr_reg] = 0;
            }
            registered_tap_counts[curr_reg]++;
 
    }else{
        if( registered_tap_counts[curr_reg] > 1)
        {
            registered_key_states[curr_reg] = doubletap;
        }
        else if (registered_hold_timers[curr_reg] && timer_elapsed(registered_hold_timers[curr_reg]) < TAPPING_TERM)
        {
            registered_key_states[curr_reg] = tap;
        }
        else
        {
            registered_key_states[curr_reg] = off_oneshot;
        }
        registered_hold_timers[curr_reg] = 0;
    }   
    return curr_reg;
};
void poll_registered_key_states(void)
{
    if(get_registered_key_state(CUST_KEY_ALTTAB) == tap) {
        register_code(KC_LALT);
        register_code(KC_LCTL);
        unregister_code(KC_LALT);
        unregister_code(KC_LCTL);

        register_code(KC_LALT);
        register_code(KC_TAB);
        unregister_code(KC_TAB);

        unregister_code(KC_LALT);
    }
    if(get_registered_key_state(CUST_KEY_ALTTAB) == hold_oneshot){            
        register_code(KC_LALT);
        register_code(KC_LCTL);
        unregister_code(KC_LALT);
        unregister_code(KC_LCTL);

        register_code(KC_LALT);
        register_code(KC_TAB);
        unregister_code(KC_TAB);
    }
    if(get_registered_key_state(CUST_KEY_ALTTAB) == off_oneshot){
        unregister_code(KC_LALT);
    }

    if(get_registered_key_state(CUST_KEY_PGUP) == tap) {
        tap_code(KC_PGUP);
    }
    if(get_registered_key_state(CUST_KEY_PGUP) == hold_oneshot) {
        tap_code(KC_HOME);
    }
    if(get_registered_key_state(CUST_KEY_PGDN) == tap) {
        tap_code(KC_PGDN);
    }
    if(get_registered_key_state(CUST_KEY_PGDN) == hold_oneshot) {
        tap_code(KC_END);
    }

    if(get_registered_key_state(CUST_KEY_TOGGLE_KVM) == tap) {
        toggle_kvm();
    }

    if(get_registered_key_state(CUST_KEY_ESC_FN) == tap) {
        tap_code(KC_ESC);
    }
    if(get_registered_key_state(CUST_KEY_ESC_FN) == hold) {
        toggle_layer(3, ON);
    }
    if(get_registered_key_state(CUST_KEY_ESC_FN) == off_oneshot) {
        toggle_layer(3, OFF);
    }

    if(get_registered_key_state(CUST_KEY_TD_KVM) == tap) {
        tap_code(KC_ESC);
    }
    if(get_registered_key_state(CUST_KEY_TD_KVM) == doubletap) {
        toggle_kvm();
    }
    if(get_registered_key_state(CUST_KEY_TD_KVM) == hold) {
        toggle_layer(3, ON);
    }
    if(get_registered_key_state(CUST_KEY_TD_KVM) == off_oneshot) {
        toggle_layer(3, OFF);
    }

    if(get_registered_key_state(CUST_KEY_SPACE_REVERSE) == tap_oneshot) {
        // Copyright 2023 Google LLC.
        // SPDX-License-Identifier: Apache-2.0
        if (process_autocorrect(KC_SPC, &(keyrecord_t){.event = MAKE_KEYEVENT(0, 0, true)})) 
        {
                tap_code(KC_SPC);
        }
    }
    if(get_registered_key_state(CUST_KEY_SPACE_REVERSE) == hold_oneshot) {
        tap_code(KC_BSPC);
    }
    if(get_registered_key_state(CUST_KEY_SPACE_REVERSE) == doubletap) {
        // tap_code(KC_BSPC);
        // tap_code(KC_BSPC);
        // tap_code(KC_DOT);
    }
    if(get_registered_key_state(CUST_KEY_SPACE_REVERSE) == hold) {
        toggle_layer(4, ON);
    }
    if(get_registered_key_state(CUST_KEY_SPACE_REVERSE) == off_oneshot) {
        toggle_layer(4, OFF);
    }
    if(get_registered_key_state(CUST_KEY_ACC_ENABLE) == tap_oneshot) {
        autocorrect_toggle();
    }

    if(get_registered_key_state(CUST_KEY_WIN_ONE_NEWTAB) == tap) {
        register_code(KC_LALT);
        register_code(KC_LCTL);
        unregister_code(KC_LALT);
        unregister_code(KC_LCTL);

        register_code(KC_LWIN);
        register_code(KC_1);
        unregister_code(KC_1);
        unregister_code(KC_LWIN);
    }
    if(get_registered_key_state(CUST_KEY_WIN_ONE_NEWTAB) == hold_oneshot) {
        register_code(KC_LCTL);
        register_code(KC_T);
        unregister_code(KC_T);
        unregister_code(KC_LCTL);
    }
    if(get_registered_key_state(CUST_KEY_WIN_TWO_CLOSETAB) == tap) {
        register_code(KC_LALT);
        register_code(KC_LCTL);
        unregister_code(KC_LALT);
        unregister_code(KC_LCTL);

        register_code(KC_LWIN);
        register_code(KC_2);
        unregister_code(KC_2);
        unregister_code(KC_LWIN);
    }
    if(get_registered_key_state(CUST_KEY_WIN_TWO_CLOSETAB) == hold_oneshot) {
        register_code(KC_LCTL);
        register_code(KC_W);
        unregister_code(KC_W);
        unregister_code(KC_LCTL);
    }
    if(get_registered_key_state(CUST_KEY_BUILD_DEPLOY) == tap) {
        register_code(KC_LCTL);
        tap_code(KC_B);
        unregister_code(KC_LCTL);
        register_code(KC_LCTL);
        tap_code(KC_B);
        unregister_code(KC_LCTL);
    }
    if(get_registered_key_state(CUST_KEY_FIRST_TAB_NEXT) == hold_oneshot) {
        register_code(KC_LCTL);
        register_code(KC_1);
        unregister_code(KC_1);
        unregister_code(KC_LCTL);
    }
    if(get_registered_key_state(CUST_KEY_FIRST_TAB_NEXT) == tap) {
        register_code(KC_LCTL);
        register_code(KC_TAB);
        unregister_code(KC_TAB);
        unregister_code(KC_LCTL);
    }
    if(get_registered_key_state(CUST_KEY_BUILD_DEPLOY) == hold_oneshot) {
        send_string("deployfsw");
        tap_code(KC_ENT);
    }
    
    for (int key=0; key < (MAX_CUSTOM_KEYS - SAFE_RANGE); key++)
    {        
            if (registered_key_states[key] == off_oneshot)
            {
                registered_key_states[key] = off;
            }
            if (registered_key_states[key] == tap)
            {
                registered_key_states[key] = off_oneshot;
            }        
            if (registered_key_states[key] == tap_oneshot)
            {
                registered_key_states[key] = tap_down;
            }
            if (registered_key_states[key] == doubletap)
            {
                registered_key_states[key] = off_oneshot;
            }
            if (registered_key_states[key] == hold_oneshot)
            {
                registered_key_states[key] = hold;
            }
            if (registered_key_states[key] != hold && registered_hold_timers[key] && (timer_elapsed(registered_hold_timers[key]) > TAPPING_TERM) )
            {
                registered_key_states[key] = hold_oneshot;
            }
    }
};



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
if (record->event.pressed) {
        if (led_on == false || old_backlight_level == -1 || rgb_on == false || old_rgb_value == -1) {
            // if (old_backlight_level == -1) old_backlight_level = rgblight_get_val();
            // if (old_rgb_value == -1) old_rgb_value = 1;
           
            // old_rgb_value = 1;
            // rgblight_enable_noeeprom();
            // backlight_set(old_backlight_level); 
				
            led_on = true;
		    rgb_on = true;
            rgblight_enable_noeeprom();
				
        }
        idle_timer = timer_read();
        halfmin_counter = 0;
    }

    switch (keycode) {
    //toggle KVM 1 or 2
    //layer 3 on momentary. 
    case SAFE_RANGE ... MAX_CUSTOM_KEYS:
        set_registered_key_state(keycode, record);
        return false;
    break;
    }
    return true;
};


//updates indicator lights every scan of keys.
//this function turns on lights, turn off lights at interrupt.
//  thus backlight functionality isn't impacted.
void setIndicatorLights(void){
    if (rgb_on)
    {
        for (size_t layer = 0; layer < MAX_LAYERS; layer++)
        {
            /* code */
            if(IS_LAYER_ON_STATE(current_layer_state, layer))
            {
                rgb_matrix_set_color(layer_led_indicators[layer], RGB_GOLD);
            }
        }
        if(kvm_selection == 1)
        {
            rgb_matrix_set_color(MACRO_ROW_1_LED_INDEX, RGB_GOLD);
        }else{
            rgb_matrix_set_color(MACRO_ROW_2_LED_INDEX, RGB_GOLD);
        }
    }
};


//triggers every scan of the keys
void matrix_scan_user(void) {
  // idle_timer needs to be set one time
    poll_registered_key_states();
    setIndicatorLights();

    if (idle_timer == 0) idle_timer = timer_read();

    if ( (led_on && timer_elapsed(idle_timer) > 30000) || (rgb_on && timer_elapsed(idle_timer) > 30000)) {
        halfmin_counter++;
        idle_timer = timer_read();
    }

    if ( (led_on && halfmin_counter >= BACKLIGHT_TIMEOUT * 2) || (rgb_on && halfmin_counter >= BACKLIGHT_TIMEOUT * 2)) {
        //old_rgb_hsv[0] = rgblight_get_hue(); 
        //old_rgb_hsv[1] = rgblight_get_sat();
        //old_rgb_hsv[2] = rgblight_get_val(); ;sorry what
        // rgblight_disable();
        rgblight_disable_noeeprom();
        led_on = false; 
        rgb_on = false;
        halfmin_counter = 0;
    }
}
//triggers on layer change
layer_state_t layer_state_set_user(layer_state_t state) {
    for (size_t layer = 0; layer < MAX_LAYERS; layer++)
    {
        /* code */
        if(!IS_LAYER_ON_STATE(state, layer))
        {
            rgb_matrix_set_color(layer_led_indicators[layer], RGB_BLACK);
        }
    }
    current_layer_state = state;
    return state;
};
 