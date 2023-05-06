#ifndef _FONT_AUX_H_
#define _FONT_AUX_H_

#include <stdint.h>

#include <avr/pgmspace.h>

#define REVERSE_BYTE(b) \
  ((((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16) & 0xFF)

#define RB(v) \
  REVERSE_BYTE(0b ## v)

#define RW(x) \
( \
  (REVERSE_BYTE((uint8_t)((0b ## x) & 0xFF))) | \
  (REVERSE_BYTE((uint8_t)((0b ## x) >> 8)) << 8) \
)

#endif // _FONT_AUX_H_