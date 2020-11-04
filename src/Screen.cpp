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

#include <Wire.h>
#include <YetAnotherPcInt.h>

#include "Fans.h"
#include "Screen.h"
#include "Globals.h"
#include "Messages.h"

ScreenManager::ScreenManager() : lcd(0x27, 16, 2), timerBacklight(this, &ScreenManager::callbackBacklight), timerSaved(this, &ScreenManager::callbackSaved) {
    timerBacklight.setTimeout(TURN_OFF_DELAY);
    timerSaved.setTimeout(SAVED_DELAY);
    turnOnTimed();
}

boolean ScreenManager::isOn() {
    return backlight;
}

void ScreenManager::turnOn() {
    timerBacklight.stop();
    backlight = true;
}

void ScreenManager::turnOnTimed() {
    timerBacklight.start();
    backlight = true;
}

void ScreenManager::turnOff() {
    timerBacklight.stop();
    backlight = false;
}

void ScreenManager::callbackBacklight() {
    backlight = false;
}

void ScreenManager::callbackSaved() {
    saved = 0;
}

void ScreenManager::init() {
    Wire.begin();
    lcd.init();
    lcd.clear();
}

void ScreenManager::showSaved(uint8_t fan) {
    saved = fan;
    timerSaved.start();
}

void ScreenManager::refresh() {
    lcd.setBacklight(backlight);
    char line1[17];
    char line2[17];
    if (Fans::getRegisteredFans() == 0) {
        strcpy(line1, MESSAGES_FANS);
        strcpy(line2, MESSAGES_DISCONNECTED);
        turnOn();
    } else if (Fans::getCurrentFanId() == saved) {
        snprintf(line1, 17, MESSAGES_FAN_NUMBER, saved);
        snprintf(line2, 17, MESSAGES_FAN_SAVED, (int)round(Fans::getCurrentFan()->getDutyCycle() * 100));
    } else {
        unsigned int percent = (int)round(Fans::getCurrentFan()->getDutyCycle() * 100);
        // Refresh the RPMS only if the fan shown has changed, or if it's the same fan,
        // only once every 5 seconds or if the change is greater than
        // the 10% of the previous value to avoid jittering
        int currentRpms = Fans::getCurrentFan()->getRpms();
        if (lastFan != Fans::getCurrentFanId() || millis() - lastRpmsRefresh > 5000 || abs(rpms - currentRpms) > 0.1f * rpms) {
            rpms = currentRpms;
            lastRpmsRefresh = millis();
        }
        snprintf(line1, 17, MESSAGES_FAN_NUMBER, Fans::getCurrentFanId());
        snprintf(line2, 17, MESSAGES_FAN_INFO, rpms, percent);
    }
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastFan = Fans::getCurrentFanId();
}
