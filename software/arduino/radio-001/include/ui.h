#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <stdint.h>

#include <Arduino.h>
#include <Bounce2.h>
#include <GyverEncoder.h>
#include <TimerOne.h>
#include <util/atomic.h>

#include "defines.h"

namespace ui {
    
    static uint8_t _enc1_pos = 0;
    static uint8_t _enc2_pos = 0;

    static Bounce2::Button _button;

    uint8_t enc1_pos(void) {
        uint8_t pos;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            pos = _enc1_pos;
        }

        return pos;
    }

    uint8_t enc2_pos(void) {
        uint8_t pos;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            pos = _enc2_pos;
        }

        return pos;
    }

    static inline bool button_dim(void) {
        _button.update();

        return _button.pressed();
    }

    static inline int button1(void) {
        return digitalRead(BTN1_ARDUINO_PIN);
    }

    static inline int button2(void) {
        return digitalRead(BTN2_ARDUINO_PIN);
    }

    static inline int button_mute(void) {
        return digitalRead(BTN_MUTE_ARDUINO_PIN);
    }

    static inline int button_enc1(void) {
        return digitalRead(ENC_BTN_ARDUINO_PIN);
    }

    static inline int button_enc2(void) {
        return digitalRead(ENC2_BTN_ARDUINO_PIN);
    }

    static inline int switch_aux(void) {
        return digitalRead(AUX_ARDUINO_PIN);
    }

    void _enc1_read(void) {
        static uint32_t enc_read_threshold = 0;

        if (enc_read_threshold++ % 2 != 0)
            return;

        if (ENC_B_PINS & (1 << ENC_B_PIN))
            ++_enc1_pos;
        else
            --_enc1_pos;
    }

    void _enc2_read(void) {
        static Encoder enc2(ENC2_A_ARDUINO_PIN,
                            ENC2_B_ARDUINO_PIN,
                            ENC2_BTN_ARDUINO_PIN,
                            TYPE2);
        enc2.tick();

        if (enc2.isTurn()) {
            if (enc2.isRight()) {
                ++_enc2_pos;
            }

            if (enc2.isLeft()) {
                --_enc2_pos;
            }
        }
    }

    static inline void setup(void) {
        // setup encoder 1
        sbi(ENC_A_PORT, ENC_A_PIN);
        cbi(ENC_A_DDR, ENC_A_PIN);
        attachInterrupt(0, _enc1_read, FALLING);

        sbi(ENC_B_PORT, ENC_B_PIN);
        cbi(ENC_B_DDR, ENC_B_PIN);

        sbi(ENC_BTN_PORT, ENC_BTN_PIN);
        cbi(ENC_BTN_DDR, ENC_BTN_PIN);

        // setup encoder 2
        Timer1.initialize(1000);
        Timer1.attachInterrupt(_enc2_read);

        // setup buttons
        _button.attach(BTN_DIM_ARDUINO_PIN, INPUT_PULLUP);
        _button.interval(5);
        _button.setPressedState(LOW);

        // setup switches
        pinMode(BTN1_ARDUINO_PIN, INPUT_PULLUP);
        pinMode(BTN2_ARDUINO_PIN, INPUT_PULLUP);
        pinMode(BTN_MUTE_ARDUINO_PIN, INPUT_PULLUP);
        pinMode(AUX_ARDUINO_PIN, INPUT_PULLUP);

        pinMode(ENC_BTN_ARDUINO_PIN, INPUT_PULLUP);
        pinMode(ENC2_BTN_ARDUINO_PIN, INPUT_PULLUP);
    }
}

#endif // _BUTTONS_H_