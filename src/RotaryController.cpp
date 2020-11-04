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

#include "RotaryController.h"

#include <YetAnotherPcInt.h>

#include "Fans.h"

RotaryController::RotaryController(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, ScreenManager *screenManager) : clkPin(clkPin), dtPin(dtPin), swPin(swPin), screenManager(screenManager) {
}

void RotaryController::init() {
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);
    PcInt::attachInterrupt(dtPin, RotaryController::handleSpin, this, CHANGE);
    PcInt::attachInterrupt(swPin, RotaryController::handlePush, this, CHANGE);
}

void RotaryController::handleSpin(RotaryController *_this, bool pinstate) {
    _this->handleSpin();
}

void RotaryController::handleSpin() {
    Fan *fan = Fans::getCurrentFan();
    int clkState = digitalRead(clkPin);
    int dtState = digitalRead(dtPin);
    if (clkLastState == LOW && dtLastState == LOW && clkState == HIGH && dtState == HIGH && fan->getDutyCycle() < 1.0f) {
        fan->setDutyCycle(fan->getDutyCycle() + DUTY_CYCLE_INCREMENT);
    } else if (clkLastState == HIGH && dtLastState == LOW && clkState == LOW && dtState == HIGH && fan->getDutyCycle() > 0.0f) {
        fan->setDutyCycle(fan->getDutyCycle() - DUTY_CYCLE_INCREMENT);
    }
    clkLastState = clkState;
    dtLastState = dtState;
    screenManager->turnOnTimed();
}

void RotaryController::handlePush(RotaryController *_this, bool pinstate) {
    if (pinstate == LOW) {
        _this->handlePush();
    } else {
        _this->handleRelease();
    }
}

void RotaryController::handlePush() {
    screenWasOn = screenManager->isOn();
    screenManager->turnOnTimed();
    pushed = millis();
}

void RotaryController::handleRelease() {
    unsigned long elapsed = millis() - pushed;
    if (elapsed < 50) {
        // debounce
    } else if (elapsed < 2000) {
        if (screenWasOn) {
            Fans::selectNextFan();
        }
    } else if (elapsed < 5000) {
        Fans::getCurrentFan()->save();
        screenManager->showSaved(Fans::getCurrentFanId());
    }
    screenManager->turnOnTimed();
}
