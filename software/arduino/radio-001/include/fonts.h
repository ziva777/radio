#ifndef _FONTS_H_
#define _FONTS_H_

#include "font_5x3.h"
#include "font_5x5.h"
#include "font_hw_sign.h"
#include "font_lcd16.h"

template<typename T>
struct FontT {
    int width;
    int height;
    bool is_digits;
    const T *data;
};

typedef FontT<uint8_t> Font;
typedef FontT<uint16_t> FontBig;

static const Font FONT_5x3 = {
    .width = 3 + 1,
    .height = 5,
    .is_digits = true,
    .data = FONT_5x3_DATA
};

static const Font FONT_5x5 = {
    .width = 5 + 1,
    .height = 5,
    .is_digits = false,
    .data = FONT_5x5_DATA
};

static Font FONT_HW_SIGN = {
    .width = 5 + 1,
    .height = 6,
    .is_digits = false,
    .data = FONT_HW_SIGN_DATA
};

static FontBig FONT_DOT16 = {
    .width = 9 + 2,
    .height = 16,
    .is_digits = false,
    .data = FONT_DOT16_DATA
};

#endif // _FONTS_H_