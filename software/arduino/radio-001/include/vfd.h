#ifndef _VFD_H_
#define _VFD_H_

#include <Arduino.h>
#include <stdint.h>

#include "defines.h"
#include "fonts.h"

namespace vfd {

    static const int FRAME_SIZE = (256 * 8);
    static uint8_t frame[FRAME_SIZE];
    static const int STX = 0x02;
    static const int HDR = 0x44;
    static const int DAD = 0x00;
    static const int CMD_SEND = 0x46;
    static const int CMD_DIM = 0x58;

    static inline void _send(uint8_t b);
    static inline void _prepare(uint16_t pos, uint16_t size);
    static inline void sync(void);
    static inline void clear_frame(void) {
        memset(frame, 0, sizeof(frame));
    }

    static inline void setup(void) {
        // Setup RDY pin
        cbi(RDY_PORT, RDY_PIN);
        cbi(RDY_DDR, RDY_PIN);

        // Setup RW pin
        sbi(WR_PORT, WR_PIN);
        sbi(WR_DDR, WR_PIN);

        // Setup DATA pin
        sbi(D0_DDR, D0_PIN);
        sbi(D1_DDR, D1_PIN);
        sbi(D2_DDR, D2_PIN);
        sbi(D3_DDR, D3_PIN);
        sbi(D4_DDR, D4_PIN);
        sbi(D5_DDR, D5_PIN);
        sbi(D6_DDR, D6_PIN);
        sbi(D7_DDR, D7_PIN);

        // Zero frame buffer
        memset(frame, 0, sizeof(frame));
        sync();
    }

    static inline void sync(void) {
        int i;
    
        _prepare(0, FRAME_SIZE);
        for (i = 0; i < FRAME_SIZE; i++)
            _send(frame[i]);
    }

    static inline void dim(uint8_t level) {  
        _send(STX);
        _send(HDR);
        _send(DAD);
        _send(CMD_DIM);
        _send(level);
    }

    // Send single byte to the VFD.
    static inline void _send(uint8_t b) {
        while (!(RDY_PINS & (1 << RDY_PIN)));

        cbi(WR_PORT, WR_PIN);
        D0_PORT = b;
        sbi(WR_PORT, WR_PIN);
    }

    // Prepare VFD to recieve data.
    static inline void _prepare(uint16_t pos, uint16_t size) {
        _send(STX);
        _send(HDR);
        _send(DAD);
        _send(CMD_SEND);
        _send(lowByte(pos));
        _send(highByte(pos));
        _send(lowByte(size));
        _send(highByte(size));
    }

    static inline void write(Font font, int16_t x, int16_t y, const char *str) {
        uint64_t   *scr;
        int16_t     y_int,
                    y_frac;
        int         i;
        unsigned    j;
        uint8_t     b;
        char        ch;
        union {
            uint64_t    u64;
            uint8_t     u8[8];
        } cast;

        scr = (uint64_t *)frame;
        y_int = y / 8;
        y_frac = y % 8;

        for (j = 0; j != strlen(str); ++j) {
            for (i = 0; i != font.width; ++i) {
                if (x >= 0 && x < 255) {
                    ch = str[j];

                    if (font.is_digits)
                        ch -= '0';
                    else
                        ch -= ' ';

                    b = pgm_read_byte(&font.data[ch * font.width + i]);

                    cast.u64 = 0;
                    cast.u8[0 + y_int] |= b >> y_frac;
                    cast.u8[1 + y_int] |= b << (8 - y_frac);

                    scr[x] |= cast.u64;
                }

                if (++x > 256)
                    break;
            }
        }
    }

    static inline void write(FontBig font, int16_t x, int16_t y, const char *str) {
        uint64_t   *scr;
        int16_t     y_int,
                    y_frac;
        int         i;
        unsigned    j;
        char        ch;
        union {
            uint64_t    u64;
            uint8_t     u8[8];
        } cast;
        union {
            uint16_t    u16;
            uint8_t     u8[2];
        } tag;

        scr = (uint64_t *)frame;
        y_int = y / 8;
        y_frac = y % 8;

        for (j = 0; j != strlen(str); ++j) {
            for (i = 0; i != font.width; ++i) {
                if (x >= 0 && x < 255) {
                    ch = str[j];

                    if (font.is_digits)
                        ch -= '0';
                    else
                        ch -= ' ';

                    tag.u16 = pgm_read_word(&font.data[ch * font.width + i]);

                    cast.u64 = 0;
                    cast.u8[0 + y_int] |= tag.u8[0] >> y_frac;
                    cast.u8[1 + y_int] |= tag.u8[0] << (8 - y_frac);
                    cast.u8[1 + y_int] |= tag.u8[1] >> y_frac;
                    cast.u8[2 + y_int] |= tag.u8[1] << (8 - y_frac);

                    scr[x] |= cast.u64;
                }

                if (++x > 256)
                    break;
            }
        }
    }

    template <typename T>
    static inline void write(T font, int16_t x, int16_t y, uint32_t val) {
        char buf[12];

        snprintf(buf, sizeof(buf), "%" PRIu32 "", val);
        write(font, x, y, buf);
    }

    constexpr unsigned long long operator"" _kHz(unsigned long long hz) {
        return hz * 1000;
    }

    static inline void draw_scale_itu(uint32_t frequency) {
        uint64_t *u8_frame = (uint64_t *) frame;
        union {
            uint64_t    u64;
            uint8_t     u8[8];
        } cast;
        static uint64_t smallest, small, big, scale;
        int up = 7;

        cast.u64 = 0b11111111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        big = cast.u64;

        cast.u64 = 0b11111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        small = cast.u64;

        cast.u64 = 0b1ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        smallest = cast.u64;

        cast.u64 = 0b11111111111111111111111ULL << 33;
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        scale = cast.u64;

        uint32_t off = frequency % 100;
        off += 7 + 16;
        uint32_t f_off = frequency - 127;
        vfd::write(FONT_5x3, 0, 0, frequency);

        for (int i = 0; i < 255; ++i, ++f_off) {
            int k = i + off;

            if (f_off < 870 || f_off > 1080) {
                u8_frame[i] |= smallest;
                continue;
            }

            if (k % 10 == 0)
                u8_frame[i] |= big;
            else if (k % 2 == 0)
                u8_frame[i] |= small;
        }

        Font f = FONT_5x3;
        off = frequency - 860;
        int w = 127 - (f.width * 2) / 2  + 1 - off;
        int y = 52 - up;

        vfd::write(f, w += 20, y, "88");
        vfd::write(f, w += 20, y, "90");
        vfd::write(f, w += 20, y, "92");
        vfd::write(f, w += 20, y, "94");
        vfd::write(f, w += 20, y, "96");
        vfd::write(f, w += 20, y, "98");
        vfd::write(f, w += 20 - f.width / 2, y, "100");
        vfd::write(f, w += 20, y, "102");
        vfd::write(f, w += 20, y, "104");
        vfd::write(f, w += 20, y, "106");
        vfd::write(f, w += 20, y, "108");

        // central scale
        u8_frame[127] |= scale;
        u8_frame[127+1] &= ~scale;
        u8_frame[127-1] &= ~scale;

        for (int i = 127-50-5; i < 127 + 50 + 2; ++i)
            u8_frame[i] |= 1ULL << 63;
    }

    static inline void draw_scale_oirt(uint32_t frequency) {
        uint64_t *u8_frame = (uint64_t *) frame;
        union {
            uint64_t    u64;
            uint8_t     u8[8];
        } cast;
        static uint64_t smallest, small, mid, big, biggest, scale;
        int up = 7;

        cast.u64 = 0b111111111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        biggest = cast.u64;

        cast.u64 = 0b11111111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        big = cast.u64;

        cast.u64 = 0b111111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        mid = cast.u64;

        cast.u64 = 0b11111ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        small = cast.u64;

        cast.u64 = 0b1ULL << (42 - up);
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        smallest = cast.u64;

        cast.u64 = 0b11111111111111111111111ULL << 33;
        for (int j = 0; j < 8; ++j)
            cast.u8[j] = REVERSE_BYTE(cast.u8[j]);
        scale = cast.u64;

        uint32_t off = frequency % 100;
        off += 7 + 16;
        uint32_t f_off = frequency - 127;

        for (int i = 0; i < 255; ++i, ++f_off) {
            int k = i + off;

            if (f_off < 6400 || f_off > 7400) {
                u8_frame[i] |= smallest;
                continue;
            }

            if (k % 100 == 0) {
                u8_frame[i] |= big;
            } else if (k % 50 == 0) {
                u8_frame[i] |= biggest;
            } else if (k % 10 == 0) {
                u8_frame[i] |= mid;
            }
            
            if (k % 2 == 0)
                u8_frame[i] |= small;
        }

        Font f = FONT_5x3;
        off = frequency - 6400;
        int w = 127 - (f.width * 2) / 2  + 1 - off;
        int w2 = 127 - (f.width * 3) / 2  + 1 - off - 2;
        int y = 52 - up;
        vfd::write(f, w, y, "64");
        vfd::write(f, w2 + 50, y, "64");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "65");
        vfd::write(f, w2 + 50, y, "65");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "66");
        vfd::write(f, w2 + 50, y, "66");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "67");
        vfd::write(f, w2 + 50, y, "67");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "68");
        vfd::write(f, w2 + 50, y, "68");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "69");
        vfd::write(f, w2 + 50, y, "69");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "70");
        vfd::write(f, w2 + 50, y, "70");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "71");
        vfd::write(f, w2 + 50, y, "71");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;
        
        vfd::write(f, w += 100, y, "72");
        vfd::write(f, w2 + 50, y, "72");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "73");
        vfd::write(f, w2 + 50, y, "73");
        vfd::write(FONT_5x5, w2 + 50 + (f.width * 2) - 2, y, ".");
        vfd::write(f, w2 + 50 + (f.width * 2) + 2, y, "5");
        w2 += 100;

        vfd::write(f, w += 100, y, "74");

        // central scale
        u8_frame[127] |= scale;
        u8_frame[127+1] &= ~scale;
        u8_frame[127-1] &= ~scale;

        for (int i = 127-50-5; i < 127 + 50 + 2; ++i)
            u8_frame[i] |= 1ULL << 63;
    }
}

#endif // _VFD_H_