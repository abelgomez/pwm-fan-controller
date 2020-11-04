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

#include "Screen.h"

class RotaryController {
    static constexpr float DUTY_CYCLE_INCREMENT = 0.05f;
    uint8_t clkPin;
    uint8_t dtPin;
    uint8_t swPin;
    ScreenManager *screenManager;
    int clkLastState = HIGH;
    int dtLastState = HIGH;
    unsigned long pushed = 0;
    boolean screenWasOn = false;

public:
    RotaryController(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, ScreenManager *screenManager);
    void init();

private:
    static void handleSpin(RotaryController *_this, bool pinstate);
    void handleSpin();
    static void handlePush(RotaryController *_this, bool pinstate);
    void handlePush();
    void handleRelease();
};
