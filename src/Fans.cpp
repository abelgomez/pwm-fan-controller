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

#include <EEPROM.h>
#include <YetAnotherPcInt.h>
#include <timer.h>
#include <timerManager.h>

#include "Fans.h"
#include "Globals.h"

/**
 * Base Fan methods
 */
Fan::Fan(uint8_t pwmPin, uint8_t tachoPin) : pwmPin(pwmPin), tachoPin(tachoPin) {
    Fans::addFan(this);
}

void Fan::init() {
    pinMode(pwmPin, OUTPUT);
    pinMode(tachoPin, INPUT_PULLUP);
    PcInt::attachInterrupt(tachoPin, Fan::handleInterrupt, this, FALLING);
    load();
}

void Fan::setDutyCycle(float f) {
    // do nothing
}

void Fan::load() {
    float data;
    EEPROM.get(getFanId() * sizeof(float), data);
    if (!isnanf(data)) {
        setDutyCycle(data);
    } else {
        setDutyCycle(1.0f);
    }
}

void Fan::save() {
    EEPROM.put(getFanId() * sizeof(float), getDutyCycle());
}

float Fan::getDutyCycle() {
    return dutyCycle;
}

unsigned int Fan::getRpms() {
    if (ts2 > ts1 && millis() - ts2 < FANSTUCK_THRESHOLD) {
        rpm = (30000 * MEASURE_EVERY_X_REVS) / (ts2 - ts1);
    } else {
        rpm = 0;
    }
    return rpm;
}

bool Fan::isConnected() {
    // Make sure the fan is rotating since a stopped fan may appear as disconnected
    float oldDutyCycle = dutyCycle;
    setDutyCycle(1.0f);
    PcInt::detachInterrupt(tachoPin);
    pinMode(tachoPin, INPUT);
    pinMode(PIN::TEST, OUTPUT);
    bool connected = false;
    bool test = LOW;
    for (int i = 0; i < 10; i++) {
        digitalWrite(PIN::TEST, test);
        delay(100);
        if (digitalRead(tachoPin) != test) {
            // The pin does not have the same value than the test pin
            // There's probably something connected to it
            connected = true;
            break;
        }
        test = !test;
    }
    pinMode(tachoPin, INPUT_PULLUP);
    pinMode(PIN::TEST, INPUT);
    PcInt::attachInterrupt(tachoPin, Fan::handleInterrupt, this, FALLING);
    setDutyCycle(oldDutyCycle);
    return connected;
}

void Fan::handleInterrupt(Fan *_this, bool pinstate) {
    _this->handleInterrupt();
}

void Fan::handleInterrupt() {
    if (count == MEASURE_EVERY_X_REVS - 1) {
        ts1 = ts2;
        ts2 = millis();
    }
    count = (count + 1) % MEASURE_EVERY_X_REVS;
}

uint8_t Fan::getFanId() {
    return Fans::getFanId(this);
}

/**
 * FanRegistry
 */
Fan *Fans::fans[Fans::MAX_FANS];
uint8_t Fans::currentFan = 0;
uint8_t Fans::registeredFans = 0;

void Fans::init() {
    for (uint8_t i = 0; i < MAX_FANS; i++) {
        if (fans[i] != NULL) {
            fans[i]->init();
            if (!fans[i]->isConnected()) {
                // Discard fans that are not physically connected
                fans[i] = NULL;
                registeredFans--;
            }
        }
    }
    for (uint8_t i = 0; i < MAX_FANS; i++) {
        if (fans[i] != NULL) {
            currentFan = i;
            break;
        }
    }
}

void Fans::addFan(Fan *fan) {
    // addFan must not be called after Fans::init() ever!
    if (registeredFans < MAX_FANS) {
        fans[registeredFans] = fan;
    }
    registeredFans++;
}

Fan *Fans::getCurrentFan() {
    return fans[currentFan];
}

void Fans::selectNextFan() {
    if (getRegisteredFans() > 0) {
        currentFan = (currentFan + 1) % MAX_FANS;
        if (getCurrentFan() == NULL) {
            selectNextFan();
        }
    }
}

uint8_t Fans::getRegisteredFans() {
    return registeredFans;
}

uint8_t Fans::getCurrentFanId() {
    return currentFan + 1;
}

int8_t Fans::getFanId(Fan *fan) {
    for (uint8_t i = 0; i < MAX_FANS; i++) {
        if (fans[i] == fan) {
            return i + 1;
        }
    }
    return -1;
}

/**
 * Fan 1
 */
class Fan1 : public Fan {
public:
    Fan1() : Fan(PIN::PWM1_10, PIN::TACHO1) {}
    void init() {
        //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
        TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
        TCCR1B = (1 << CS10) | (1 << WGM13);
        ICR1 = 320;
        OCR1B = 320;
        Fan::init();
    }
    virtual void setDutyCycle(float f) {
        dutyCycle = f < 0 ? 0 : f > 1 ? 1 : f;
        OCR1B = (uint16_t)(320 * dutyCycle);
    }
} f1;

/**
 * Fan 2
 */
class Fan2 : public Fan {
public:
    Fan2() : Fan(PIN::PWM2_9, PIN::TACHO2) {}
    void init() {
        //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
        TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
        TCCR1B = (1 << CS10) | (1 << WGM13);
        ICR1 = 320;
        OCR1A = 320;
        Fan::init();
    }
    virtual void setDutyCycle(float f) {
        dutyCycle = f < 0 ? 0 : f > 1 ? 1 : f;
        OCR1A = (uint16_t)(320 * dutyCycle);
    }
} f2;

/**
 * Fan 3
 */
class Fan3 : public Fan {
public:
    Fan3() : Fan(PIN::PWM3_3, PIN::TACHO3) {}
    void init() {
        //Set PWM frequency to about 25khz on pin 3 (timer 2 mode 5, prescale 8, count to 79)
        TIMSK2 = 0;
        TIFR2 = 0;
        TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
        TCCR2B = (1 << WGM22) | (1 << CS21);
        OCR2A = 79;
        OCR2B = 0;
        Fan::init();
    }
    virtual void setDutyCycle(float f) {
        dutyCycle = f < 0 ? 0 : f > 1 ? 1 : f;
        OCR2B = (uint8_t)(79 * dutyCycle);
    }
} f3;
