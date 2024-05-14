#include "ui.h"

#include <util/atomic.h>
#include <Bounce2.h>
#include <GyverEncoder.h>
#include <TimerOne.h>

#include "wiring.h"

UI ui;

static uint8_t enc1_pos = 0;
static uint8_t enc2_pos = 0;

void
enc1_read()
{
    static uint32_t enc_read_threshold = 0;

    if (enc_read_threshold++ % 2 != 0)
        return;

    if (ENC_B_PINS & (1 << ENC_B_PIN))
        ++enc1_pos;
    else
        --enc1_pos;
}

void
enc2_read()
{
    static Encoder enc2(ENC2_A_ARDUINO_PIN,
                        ENC2_B_ARDUINO_PIN,
                        ENC2_BTN_ARDUINO_PIN,
                        TYPE2);
    enc2.tick();

    if (enc2.isTurn()) {
        if (enc2.isRight())
            ++enc2_pos;

        if (enc2.isLeft())
            --enc2_pos;
    }
}

void
ui_init()
{
    /* setup encoder 1 */
    sbi(ENC_A_PORT, ENC_A_PIN);
    cbi(ENC_A_DDR, ENC_A_PIN);
    attachInterrupt(0, enc1_read, FALLING);

    /* setup encoder 2 */
    Timer1.initialize(1000);
    Timer1.attachInterrupt(enc2_read);
}

void
ui_sync()
{
    uint8_t pos;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        pos = enc1_pos;
    }

    if (pos != ui.enc1.curr) {
        ui.enc1.prev = ui.enc1.curr;
        ui.enc1.curr = pos;
        ui.enc1.changed = true;
    }

    ui.enc1.pressed = digitalRead(ENC_BTN_ARDUINO_PIN) == LOW;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        pos = enc2_pos;
    }

    if (pos != ui.enc2.curr) {
        ui.enc2.prev = ui.enc2.curr;
        ui.enc2.curr = pos;
        ui.enc2.changed = true;
    }

    ui.enc2.pressed = digitalRead(ENC2_BTN_ARDUINO_PIN) == LOW;
}