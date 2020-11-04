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

class Fan {
public:
    void load();
    void save();
    virtual void init();
    virtual void setDutyCycle(float f);
    float getDutyCycle();
    unsigned int getRpms();
    bool isConnected();

protected:
    float dutyCycle;
    Fan(uint8_t pwmPin, uint8_t tachoPin);
    uint8_t getFanId();

private:
    static void handleInterrupt(Fan *_this, bool pinstate);
    void handleInterrupt();

    static const unsigned int FANSTUCK_THRESHOLD = 1000;
    static const unsigned long MEASURE_EVERY_X_REVS = 2;

    uint8_t pwmPin;
    uint8_t tachoPin;
    unsigned int count = 0;
    unsigned int rpm = 0;
    volatile unsigned long ts1 = 0;
    volatile unsigned long ts2 = 0;
};

class Fans final {
public:
    static void init();
    static Fan *getCurrentFan();
    static void selectNextFan();
    static uint8_t getRegisteredFans();
    static uint8_t getCurrentFanId();
    static int8_t getFanId(Fan *fan);

private:
    friend class Fan;

    static void addFan(Fan *fan);

    static constexpr unsigned int MAX_FANS = 3;

    static Fan *fans[MAX_FANS];
    static uint8_t currentFan;
    static uint8_t registeredFans;
};
