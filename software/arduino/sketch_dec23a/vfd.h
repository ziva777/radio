#ifndef _VFD_H_
#define _VFD_H_

#include <Arduino.h>
#include <stdint.h>

#include "defines.h"

#define cbi(reg, bit)   ((reg) &= ~(1 << (bit)))
#define sbi(reg, bit)   ((reg) |= (1 << (bit)))

#define SCREEN_SIZE (256 * 8)

static uint8_t screen_buf[SCREEN_SIZE];

/*
 * Send single byte to the VFD.
 */
static inline void
vfd_send(uint8_t b)
{
  while (!(RDY_PINS & (1 << RDY_PIN)));

  cbi(WR_PORT, WR_PIN);
  D0_PORT = b;
  sbi(WR_PORT, WR_PIN);
}

/*
 * Prepare VFD to recieve data.
 */
static inline void
vfd_start(uint16_t pos, uint16_t size)
{
#define STX 0x02
#define HDR 0x44
#define DAD 0x00
#define CMD 0x46
#define LOW_BYTE(b)     ((b) & 0xFF)
#define HIGH_BYTE(b)    ((b) >> 8)

  vfd_send(STX);
  vfd_send(HDR);
  vfd_send(DAD);
  vfd_send(CMD);
  vfd_send(LOW_BYTE(pos));
  vfd_send(HIGH_BYTE(pos));
  vfd_send(LOW_BYTE(size));
  vfd_send(HIGH_BYTE(size));
}

/*
 * Clear all data.
 */
static inline void
vfd_clear(void)
{
  int i;
  
  vfd_start(0, SCREEN_SIZE);
  for (i = 0; i < SCREEN_SIZE; i++)
    vfd_send(0x00);
}

/*
 * Load data of size SCREEN_SIZE.
 */
static inline void
vfd_load(const uint8_t *src)
{
  int       i;
  uint8_t  *dst;

  dst = screen_buf;
  for (i = 0; i < SCREEN_SIZE; i++)
    *dst++ = pgm_read_byte(src++);
}

/*
 * Sync screen_buf with VFD.
 */
static inline void
vfd_sync(void)
{
  int       i;
  uint8_t  *dst;

  dst = screen_buf;
  vfd_start(0, SCREEN_SIZE);
  for (i = 0; i < SCREEN_SIZE; i++)
    vfd_send(*dst++);
}

static inline void
vfd_setup(void)
{
  /* Setup RDY pin */
  cbi(RDY_PORT, RDY_PIN);
  cbi(RDY_DDR, RDY_PIN);

  /* Setup RW pin */
  sbi(WR_PORT, WR_PIN);
  sbi(WR_DDR, WR_PIN);

  /* Setup DATA pin */
  sbi(D0_DDR, D0_PIN);
  sbi(D1_DDR, D1_PIN);
  sbi(D2_DDR, D2_PIN);
  sbi(D3_DDR, D3_PIN);
  sbi(D4_DDR, D4_PIN);
  sbi(D5_DDR, D5_PIN);
  sbi(D6_DDR, D6_PIN);
  sbi(D7_DDR, D7_PIN);

  /* Fill with zeros */
  vfd_clear();
}

#endif /* _VFD_H_ */
