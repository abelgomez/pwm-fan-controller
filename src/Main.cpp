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

#include <Arduino.h>
#include <EEPROM.h>
#include <YetAnotherPcInt.h>
#include <timer.h>
#include <timerManager.h>

#include "Fans.h"
#include "Screen.h"
#include "RotaryController.h"
#include "Globals.h"

ScreenManager screenManager;
RotaryController controller(PIN::CLK, PIN::DT, PIN::SW, &screenManager);

void initialize() {
    // We will store the saved duty cycles based on fan IDs (i.e., 1--3)
    // In arduino, floats use 4 bytes. That leaves this memory schema:
    // EEPROM bytes 00--03 -> Available to save a magic number
    // EEPROM bytes 04--07 -> Fan 1 duty cycle
    // EEPROM bytes 08--11 -> Fan 2 duty cycle
    // EEPROM bytes 12--16 -> Fan 3 duty cycle
    char magic[4];
    EEPROM.get(0, magic);
    if (!strncmp(magic, "FAN\0", 4)) {
        EEPROM.put(0 * sizeof(float), "FAN\0");
        EEPROM.put(1 * sizeof(float), 1.0f);
        EEPROM.put(2 * sizeof(float), 1.0f);
        EEPROM.put(3 * sizeof(float), 1.0f);
    }
}

void setup() {
    Serial.begin(9600);
    initialize();
    Fans::init();
    screenManager.init();
    if (Fans::getRegisteredFans() > 0) {
        controller.init();
    }
}

void loop() {
    delay(100);
    screenManager.refresh();
    TimerManager::instance().update();
}