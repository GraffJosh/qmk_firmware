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

#pragma once

/* key matrix pins */
#define MATRIX_ROW_PINS { B5, B4, B3, A15, A14, A13 }
#define MATRIX_COL_PINS { C14, C15, A0, A1, A2, A3, A4, A5, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN, NO_PIN }

/* RGB Matrix Configuration */
#define DRIVER_1_LED_TOTAL 48
#define DRIVER_2_LED_TOTAL 39
#define RGB_MATRIX_LED_COUNT (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)

/* Encoder Configuration */
#define ENCODER_DEFAULT_POS 0x3

#define CKLED2001_CURRENT_TUNE \
    { 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44, 0x9D, 0x9D, 0x44 }

/* Enable CapsLcok LED */
#define CAPS_LOCK_LED_INDEX 50

#define MAX_LAYERS 6
#define DYNAMIC_KEYMAP_LAYER_COUNT 6
#define NUMBER_ROW_0_LED_INDEX 16
#define NUMBER_ROW_1_LED_INDEX 17
#define NUMBER_ROW_2_LED_INDEX 18
#define NUMBER_ROW_3_LED_INDEX 19
#define NUMBER_ROW_4_LED_INDEX 20
#define NUMBER_ROW_5_LED_INDEX 21
#define MACRO_ROW_1_LED_INDEX 13
#define MACRO_ROW_2_LED_INDEX 14
#define MACRO_ROW_3_LED_INDEX 15
