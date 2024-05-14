#pragma once

#include <Arduino.h>

#include "sprite.h"

struct Point
{
    uint16_t    x;
    uint8_t     y;
};

enum class Blend {OVER, ADD, XOR};

void vfd_init();
void vfd_sync();
void vfd_clb();
void vfd_draw(Point p, Sprite sp, Blend b = Blend::OVER, bool invert = false);