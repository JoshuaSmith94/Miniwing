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

#pragma once

#include "quantum.h"

#define LAYOUT( \
    K00, K01, K02, K03, K04, \
    K10, K11, K12, K13, K14, \
    K20, K21, K22, K23, K24, \
    K33, K34, \
    K45, K46, K47, K48, K49, \
    K55, K56, K57, K58, K59, \
    K65, K66, K67, K68, K69, \
    K75, K76  \
) { \
    { K00, K01, K02, K03, K04, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
    { K10, K11, K12, K13, K14, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
    { K20, K21, K22, K23, K24, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
    { KC_NO, KC_NO, KC_NO, K33, K34, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K45, K46, K47, K48, K49 }, \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K55, K56, K57, K58, K59 }, \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K65, K66, K67, K68, K69 }, \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K75, K76, KC_NO, KC_NO, KC_NO } \
}
