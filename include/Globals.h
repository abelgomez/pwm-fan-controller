/**
 * Copyright (C) 2020 Abel Gómez
 * 
 * Arduino PWM Fan Controller is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Arduino PWM Fan Controller is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Arduino PWM Fan Controller. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Arduino.h>

struct PIN {
    static constexpr uint8_t TEST = A0;
    static constexpr uint8_t PWM1_10 = 10;
    static constexpr uint8_t PWM2_9 = 9;
    static constexpr uint8_t PWM3_3 = 3;
    static constexpr uint8_t TACHO1 = 7;
    static constexpr uint8_t TACHO2 = 6;
    static constexpr uint8_t TACHO3 = 5;
    static constexpr uint8_t SW = A3;
    static constexpr uint8_t DT = 2;
    static constexpr uint8_t CLK = 4;
};
