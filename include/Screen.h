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

#include <LiquidCrystal_I2C.h>
#include <timer.h>

class ScreenManager {
    static const unsigned long TURN_OFF_DELAY = 5000;
    static const unsigned long SAVED_DELAY = 5000;
    LiquidCrystal_I2C lcd;
    uint8_t saved = 0;
    uint8_t lastFan = 0;
    int rpms;
    unsigned long lastRpmsRefresh = 0;
    TimerForMethods<ScreenManager> timerBacklight;
    TimerForMethods<ScreenManager> timerSaved;
    volatile boolean backlight = false;

public:
    ScreenManager();
    boolean isOn();
    void turnOn();
    void turnOnTimed();
    void turnOff();
    void init();
    void showSaved(uint8_t fan);
    void refresh();

private:
    void callbackBacklight();
    void callbackSaved();
};
