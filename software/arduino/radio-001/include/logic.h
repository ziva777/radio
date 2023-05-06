#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <stdint.h>

#include "vfd.h"
#include "ui.h"
#include "tuner.h"

// static const char *BAND[] = {
//     "BROADCAST FM", "BROADCAST UKW",
//     "BROADCAST 120M", "BROADCAST 90M", "BROADCAST 75M",
//     "BROADCAST 60M", "BROADCAST 49M", "BROADCAST 41M",
//     "BROADCAST 31M", "BROADCAST 25M", "BROADCAST 22M",
//     "BROADCAST 19M", "BROADCAST 16M", "BROADCAST 15M",
//     "BROADCAST 13M", "BROADCAST 11M",
// };

constexpr unsigned long long operator"" _Hz(unsigned long long hz) {
    return hz;
}

constexpr unsigned long long operator"" _kHz(unsigned long long hz) {
    return hz * 1000;
}

constexpr unsigned long long operator"" _MHz(unsigned long long hz) {
    return hz * 1000000;
}

namespace logic {

    enum DIM {
        DIM_100 = 4,
        DIM_75  = 3,
        DIM_50  = 2,
        DIM_25  = 1,
        DIM_0   = 0,
    };

    struct Band {
        int         mode;
        uint32_t    from;
        uint32_t    to;
        uint32_t    step;
        const char *name;
    };
    // Metre Band 	Frequency Range 	Remarks
    // 120 m 	    2.3–2.495 MHz 	    tropical band
    // 90 m 	    3.2–3.4 MHz 	    tropical band
    // 75 m 	    3.9–4 MHz 	        shared with the North American amateur radio 80m band
    // 60 m 	    4.75–5.06 MHz 	    tropical band
    // 49 m 	    5.9–6.2 MHz 	 
    // 41 m 	    7.2–7.6 MHz 	    shared with the amateur radio 40m band
    // 31 m 	    9.4–9.9 MHz 	    currently the most heavily used band
    // 25 m 	    11.6–12.2 MHz 	 
    // 22 m 	    13.57–13.87 MHz 	
    // 19 m 	    15.1–15.8 MHz 	 
    // 16 m 	    17.48–17.9 MHz 	 
    // 15 m 	    18.9–19.02 MHz 	    almost unused, could become a DRM band
    // 13 m 	    21.45–21.85 MHz 	 
    // 11 m 	    25.6–26.1 MHz 	    may be used for local DRM broadcasting 
    static const Band BANDS[] = {
        {.mode = FM_CURRENT_MODE, .from = 87_MHz,    .to = 108_MHz,   .step = 100_kHz, .name = "BROADCAST 87-108 MHZ"},
        {.mode = FM_CURRENT_MODE, .from = 64_MHz,    .to = 74_MHz,    .step = 10_kHz,  .name = "BROADCAST 64-74 MHZ"},

        {.mode = AM_CURRENT_MODE, .from = 25600_kHz, .to = 26100_kHz, .step = 1_kHz,   .name = "BROADCAST 11M"},
        {.mode = AM_CURRENT_MODE, .from = 21450_kHz, .to = 21850_kHz, .step = 1_kHz,   .name = "BROADCAST 13M"},
        {.mode = AM_CURRENT_MODE, .from = 18900_kHz, .to = 19020_kHz, .step = 1_kHz,   .name = "BROADCAST 15M"},
        {.mode = AM_CURRENT_MODE, .from = 17480_kHz, .to = 17900_kHz, .step = 1_kHz,   .name = "BROADCAST 16M"},
        {.mode = AM_CURRENT_MODE, .from = 15100_kHz, .to = 15800_kHz, .step = 1_kHz,   .name = "BROADCAST 19M"},
        {.mode = AM_CURRENT_MODE, .from = 13570_kHz, .to = 13870_kHz, .step = 1_kHz,   .name = "BROADCAST 22M"},
        {.mode = AM_CURRENT_MODE, .from = 11600_kHz, .to = 12200_kHz, .step = 1_kHz,   .name = "BROADCAST 25M"},
        {.mode = AM_CURRENT_MODE, .from = 9400_kHz,  .to = 9900_kHz,  .step = 1_kHz,   .name = "BROADCAST 31M"},
        {.mode = AM_CURRENT_MODE, .from = 7200_kHz,  .to = 7600_kHz,  .step = 1_kHz,   .name = "BROADCAST 41M"},
        {.mode = AM_CURRENT_MODE, .from = 5900_kHz,  .to = 6200_kHz,  .step = 1_kHz,   .name = "BROADCAST 49M"},
        {.mode = AM_CURRENT_MODE, .from = 4750_kHz,  .to = 5060_kHz,  .step = 1_kHz,   .name = "BROADCAST 60M"},
        {.mode = AM_CURRENT_MODE, .from = 3900_kHz,  .to = 4000_kHz,  .step = 1_kHz,   .name = "BROADCAST 75M"},
        {.mode = AM_CURRENT_MODE, .from = 3200_kHz,  .to = 3400_kHz,  .step = 1_kHz,   .name = "BROADCAST 90M"},
        {.mode = AM_CURRENT_MODE, .from = 2300_kHz,  .to = 2495_kHz,  .step = 1_kHz,   .name = "BROADCAST 120M"},
        
        {.mode = AM_CURRENT_MODE, .from = 531_kHz,   .to = 1700_kHz,  .step = 9_kHz,   .name = "BROADCAST MW"},
        {.mode = AM_CURRENT_MODE, .from = 153_kHz,   .to = 279_kHz,   .step = 9_kHz,   .name = "BROADCAST LW"},
        {.mode = AM_CURRENT_MODE, .from = 150_kHz,   .to = 26100_kHz, .step = 1_kHz,   .name = "ALL BANDS AM"},
    };
    static const int N_BANDS = sizeof(BANDS) / sizeof(BANDS[0]);
    // static uint32_t bands_frequency_cache[N_BANDS];

    static struct {
        uint8_t     curr_band;
        uint32_t    frequency;
        uint32_t    step;
        DIM         dim;

        // ui inputs
        uint8_t prev_enc1_pos;
        uint8_t prev_enc2_pos;
    } values;

    static inline void setup(void) {
        uint8_t enc1_pos = ui::enc1_pos();
        uint8_t enc2_pos = ui::enc2_pos();
        // int enc1_btn = ui::button_enc1();
        // int enc2_btn = ui::button_enc2();
        // int bnt1 = ui::button1();
        // int bnt2 = ui::button2();
        // int mute = ui::button_mute();

        vfd::dim(DIM_100);
        vfd::clear_frame();
        vfd::sync();
        
        // On power on default level is 100%
        values.dim = DIM_100;
        values.curr_band = 0;
        values.frequency = BANDS[values.curr_band].from;
        values.step = BANDS[values.curr_band].step;
        values.prev_enc1_pos = enc1_pos;
        values.prev_enc2_pos = enc2_pos;

        if (BANDS[values.curr_band].mode == FM_CURRENT_MODE)
            tuner::set_FM(BANDS[values.curr_band].from,
                        BANDS[values.curr_band].to,
                        values.frequency,
                        BANDS[values.curr_band].step);
        else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
            tuner::set_AM(BANDS[values.curr_band].from,
                        BANDS[values.curr_band].to,
                        values.frequency,
                        BANDS[values.curr_band].step);
        
        // bands_frequency_cache[values.curr_band] = values.frequency;
    }

    static inline void loop_mute(void) {
        int res = ui::button_mute();
        digitalWrite(AMP_MUTE_ARDUINO_PIN, res);
    }

    static inline void loop_dim(void) {
        static uint8_t level = DIM_100;
        uint8_t new_level = level;

        if (ui::button_dim())
            if (--new_level == DIM_0)
                new_level = DIM_100;
        
        if (level != new_level) {
            level = new_level;
            vfd::dim(new_level);
        }
    }

    static inline void loop_aux(void) {
        int aux = ui::switch_aux();

        vfd::write(FONT_5x5, 22, 0, aux ? "RADIO" : "AUX");
    }

    static inline void loop(void) {
        loop_mute();
        loop_dim();
        

        uint8_t enc1_pos = ui::enc1_pos();
        uint8_t enc2_pos = ui::enc2_pos();
        int8_t enc1_delta = enc1_pos - values.prev_enc1_pos;
        int8_t enc2_delta = enc2_pos - values.prev_enc2_pos;
        bool band_changed = false;
        bool freq_changed = false;
        static bool first_run = true;

        if (enc2_delta != 0) {
            uint8_t prev_band = values.curr_band;

            if (enc2_delta > values.curr_band)
                values.curr_band = 0;
            else if (values.curr_band - enc2_delta > N_BANDS - 1)
                values.curr_band = N_BANDS - 1;
            else
                values.curr_band -= enc2_delta;
            
            if (prev_band != values.curr_band) {
                if (BANDS[values.curr_band].mode == FM_CURRENT_MODE)
                    tuner::set_FM(BANDS[values.curr_band].from,
                                BANDS[values.curr_band].to,
                                values.frequency,
                                BANDS[values.curr_band].step);
                else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
                    tuner::set_AM(BANDS[values.curr_band].from,
                                BANDS[values.curr_band].to,
                                values.frequency,
                                BANDS[values.curr_band].step);
            }
            
            values.frequency = BANDS[values.curr_band].from;
            values.step = BANDS[values.curr_band].step;

            if (enc1_delta == 0)
                band_changed = true;
                // tuner::set_frequency(values.frequency);
        }

        if (enc1_delta != 0 || band_changed) {
            uint32_t target;

            target = values.frequency + enc1_delta * BANDS[values.curr_band].step;
            if (target > BANDS[values.curr_band].to)
                target = BANDS[values.curr_band].to;
            else if (target < BANDS[values.curr_band].from)
                target = BANDS[values.curr_band].from;
            
            if (target != values.frequency) {
                values.frequency = target;
                tuner::set_frequency(values.frequency);
                freq_changed = true;
            }
        }

        if (band_changed || freq_changed || first_run) {
            char buf1[128];
            char buf2[128];
            char buf3[128];

            first_run = false;

            // snprintf(buf, sizeof(buf), "FM %lu MHZ", values.frequency);
            if (BANDS[values.curr_band].mode == FM_CURRENT_MODE)
                snprintf(buf1, sizeof(buf1), "FM            MHZ");
            else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
                snprintf(buf1, sizeof(buf1), "AM           KHZ");
            
            vfd::clear_frame();
            loop_aux();
            vfd::write(FONT_HW_SIGN, 74, 24, buf1);

            if (BANDS[values.curr_band].mode == FM_CURRENT_MODE) {
                snprintf(buf2, sizeof(buf2), "%03u", (unsigned) (values.frequency / 1_MHz));
                if (values.frequency < 100_MHz)
                    buf2[0] = ' ';
                snprintf(buf3, sizeof(buf3), "%02u", (unsigned) ((values.frequency - (values.frequency / 1_MHz) * 1_MHz) / 10_kHz));

                vfd::write(FONT_DOT16, 93, 16, buf2);
                vfd::write(FONT_5x5, 90 + 6 * 3 + FONT_DOT16.width * 2 - 5, 26, ".");
                vfd::write(FONT_DOT16, 90 + 6 * 3 + FONT_DOT16.width * 2, 16, buf3);
            } else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE) {
                char *ch = buf2;

                snprintf(buf2, sizeof(buf2), "%05u", (unsigned) (values.frequency / 1_kHz));
                while (*ch == '0') {
                    *ch = ' ';
                    ch++;
                }

                vfd::write(FONT_DOT16, 93, 16, buf2);
            }



            // uint32_t f;

            // vfd::clear_frame();

            // f = values.frequency;
            // f /= 10000;
            // char buf1[32];
            // char buf2[32];

            // snprintf(buf1, sizeof(buf1), "%02u", values.frequency / 1_MHz);
            // if (BANDS[values.curr_band].mode == FM_CURRENT_MODE)
            //     snprintf(buf2, sizeof(buf2), "%02u", (values.frequency - (values.frequency / 1_MHz) * 1_MHz) / 10_kHz);
            // else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
            //     snprintf(buf2, sizeof(buf2), "%03u", (values.frequency - (values.frequency / 1_MHz) * 1_MHz) / 1_kHz);

            // vfd::write(FONT_DOT16, 90+6*3 - (f < 9999 ? 0 : FONT_DOT16.width), 16, buf1);
            // vfd::write(FONT_5x5, 90+6*3 + FONT_DOT16.width * 2 - 2, 26, ".");
            // vfd::write(FONT_DOT16, 90+6*3 + FONT_DOT16.width * 2 + 3, 16, buf2);
            // vfd::write(FONT_HW_SIGN, 86, 16+8, "FM");

            // if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
            //     vfd::write(FONT_HW_SIGN, 160 + FONT_DOT16.width, 16+8, "MHZ");
            // else
            //     vfd::write(FONT_HW_SIGN, 160, 16+8, "MHZ");

            // vfd::write(FONT_5x3, 0, 0, "0123456789");

            {
                if (values.curr_band == 0) {
                    vfd::draw_scale_itu(values.frequency / 100_kHz);
                    vfd::write(FONT_5x5, 72, 64 - FONT_5x5.width, BANDS[values.curr_band].name);
                } else if (values.curr_band == 1) {
                    vfd::draw_scale_oirt(values.frequency / 10_kHz);
                    vfd::write(FONT_5x5, 72, 64 - FONT_5x5.width, BANDS[values.curr_band].name);
                } else {
                    vfd::draw_scale_oirt(values.frequency / 10_kHz);
                    vfd::write(FONT_5x5, 72, 64 - FONT_5x5.width, BANDS[values.curr_band].name);
                }
            }
            vfd::sync();
        }
        /*if (band_changed || freq_changed)
        {
            uint32_t f;

            vfd::clear_frame();
            vfd::write(FONT_5x5, 80, 0, BANDS[values.curr_band].name);
            f = values.frequency;
            f /= 10000;
            char buf1[32];
            char buf2[32];
            char buf[64];
            snprintf(buf1, sizeof(buf1), "%2u", values.frequency / 1_MHz);
            if (BANDS[values.curr_band].mode == FM_CURRENT_MODE)
                snprintf(buf2, sizeof(buf2), "%02u", (values.frequency - (values.frequency / 1_MHz) * 1_MHz) / 10_kHz);
            else if (BANDS[values.curr_band].mode == AM_CURRENT_MODE)
                snprintf(buf2, sizeof(buf2), "%03u", (values.frequency - (values.frequency / 1_MHz) * 1_MHz) / 1_kHz);
            snprintf(buf, sizeof(buf), "%s.%s MHZ", buf1, buf2);
            vfd::write(FONT_HW_SIGN, 90+6*3 - (f < 9999 ? 0 : 6), 32, buf);

            vfd::write(FONT_DOT16, 10, 10, values.frequency / 100_kHz);
            vfd::sync();
        }*/

        values.prev_enc1_pos = enc1_pos;
        values.prev_enc2_pos = enc2_pos;
    }
}

#endif // _LOGIC_H_