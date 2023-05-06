#ifndef _TUNER_H_
#define _TUNER_H_

#include <Arduino.h>
#include <Wire.h>
#include <SI4735.h>

#include "defines.h"

namespace tuner {

    static SI4735 _rx;
    static const int RESET_PIN = 8;
    static const int FM_FUNCTION = 0;
    static const int AM_FUNCTION = 1;

    static inline void setup(void) {
        digitalWrite(RESET_PIN, HIGH);
        if (_rx.getDeviceI2CAddress(RESET_PIN) == 0)
            while (1);
        
        // delay(500);

        pinMode(AMP_MUTE_ARDUINO_PIN, OUTPUT);
        digitalWrite(AMP_MUTE_ARDUINO_PIN, HIGH);
        
        pinMode(AMP_MODE_ARDUINO_PIN, OUTPUT);
        digitalWrite(AMP_MODE_ARDUINO_PIN, HIGH);
    }

    static inline void set_FM(uint32_t from, uint32_t to, uint32_t curr, uint32_t step) {
        digitalWrite(AMP_MUTE_ARDUINO_PIN, LOW);

        _rx.setup(RESET_PIN, FM_FUNCTION);
        _rx.setFM(from / 10000, to / 10000, curr / 10000, step / 10000);
        _rx.setAutomaticGainControl(0, 0);
        _rx.setVolume(63);
        // delay(100);

        digitalWrite(AMP_MUTE_ARDUINO_PIN, HIGH);
    }

    static inline void set_AM(uint32_t from, uint32_t to, uint32_t curr, uint32_t step) {
        digitalWrite(AMP_MUTE_ARDUINO_PIN, LOW);
        
        _rx.setup(RESET_PIN, AM_FUNCTION);
        _rx.setAM(from / 10000, to / 10000, curr / 10000, step / 10000);
        _rx.setAutomaticGainControl(0, 0);
        _rx.setVolume(63);
        // delay(100);

        digitalWrite(AMP_MUTE_ARDUINO_PIN, HIGH);
    }

    static inline void set_frequency(uint32_t f) {
        _rx.setFrequency(f / 10000);
    }
}

#endif // _TUNER_H_