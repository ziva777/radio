#include <Arduino.h>
#include <Wire.h>

#include "vfd.h"
#include "ui.h"
#include "tuner.h"
#include "logic.h"

void setup() {
    vfd::setup();
    ui::setup();
    tuner::setup();
    logic::setup();
}

void loop() {
    logic::loop();
    // static uint8_t dim_counter;
    // vfd::clear_frame();

    // if (ui::button_dim())
        // ++dim_counter;
    // if (dim_counter == 4)
        // dim_counter = 0;
    // vfd::write(FONT_HW_SIGN, 10, 10,  dim_counter);
    // vfd::sync();
    // delay(1000);

    // vfd::clear_frame();
    // vfd::write(FONT_5x5, 10, 20, "HELLOW");
    // vfd::sync();
    // delay(1000);

    // vfd::clear_frame();
    // vfd::write(FONT_DOT16, 10, 30, "012");
    // vfd::sync();
    // delay(1000);

    // vfd::write(FONT_HW_SIGN, 50, 10, ui::enc1_pos());
    // vfd::write(FONT_HW_SIGN, 50, 10+8, ui::enc2_pos());

    // int res = ui::button1();
    // if (res == HIGH)
    //     vfd::write(FONT_HW_SIGN, 48,  32, "H");
    // else
    //     vfd::write(FONT_HW_SIGN, 48,  32, "L");
    
    // res = ui::button1();
    // if (res == HIGH)
    //     vfd::write(FONT_HW_SIGN, 48+6,  32, "H");
    // else
    //     vfd::write(FONT_HW_SIGN, 48+6,  32, "L");

    // static uint8_t prev_enc2_pos;
    // uint8_t enc2_pos = ui::enc2_pos();
    // int8_t enc2_delta;
    // uint8_t p = 0;

    // enc2_delta = enc2_pos - prev_enc2_pos;
    // p += enc2_delta;
    // p %= 16;

    // vfd::write(FONT_5x5, 90, 0, BAND[p]);
    
    // int res = ui::button_mute();
    // digitalWrite(AMP_MUTE_ARDUINO_PIN, res);

    // static uint8_t prev_enc1_pos;
    // uint8_t enc1_pos = ui::enc1_pos();
    // int8_t enc1_delta;

    // if (prev_enc1_pos == enc1_pos)
        // return;

    // enc1_delta = enc1_pos - prev_enc1_pos;
    


    // tuner::_rx.setFrequency()
    
    // prev_enc1_pos = enc1_pos;
    // static uint16_t f = 8910;
    // static uint16_t f_old = 0;
    // int16_t fdela;
    // fdela = enc1_delta;
    // fdela *= 10;
    // f += fdela;
    // if (f != f_old) {
    //     tuner::_rx.setFrequency(f);
    //     f_old = f;
    // }
    // vfd::write(FONT_HW_SIGN, 90+6*3 - (f < 9999 ? 0 : 6), 32, f);
    // vfd::write(FONT_HW_SIGN, 90+6*8, 32, "MHZ");

    // char freq[16];
    // snprintf(freq, sizeof(freq), "%u", )
    // vfd::write(FONT_DOT16, 10, 30, "012");  

    // if (res == HIGH)
    //     vfd::write(FONT_HW_SIGN, 48+6*2,  32, "H");
    // else
    //     vfd::write(FONT_HW_SIGN, 48+6*2,  32, "L");
    
    // res = ui::button_enc1();
    // if (res == HIGH)
    //     vfd::write(FONT_HW_SIGN, 48+6*3,  32, "H");
    // else
    //     vfd::write(FONT_HW_SIGN, 48+6*3,  32, "L");

    // res = ui::button_enc2();
    // if (res == HIGH)
    //     vfd::write(FONT_HW_SIGN, 48+6*4,  32, "H");
    // else
    //     vfd::write(FONT_HW_SIGN, 48+6*4,  32, "L");

    // vfd::sync();
}