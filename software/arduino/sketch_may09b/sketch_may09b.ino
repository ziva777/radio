#include "defines.h"
#include "font1.h"

#define cbi(reg, bit)   ((reg) &= ~(1 << (bit)))
#define sbi(reg, bit)   ((reg) |= (1 << (bit)))

#define LOW_BYTE(b)     ((b) & 0xFF)
#define HIGH_BYTE(b)    ((b) >> 8)

static void encoder_setup(void)
{
  sbi(ENC_A_PORT, ENC_A_PIN);
  cbi(ENC_A_DDR, ENC_A_PIN);

  sbi(ENC_B_PORT, ENC_B_PIN);
  cbi(ENC_B_DDR, ENC_B_PIN);

  sbi(ENC_BTN_PORT, ENC_BTN_PIN);
  cbi(ENC_BTN_DDR, ENC_BTN_PIN);
}

static inline void vfd_send_byte(uint8_t b)
{ 
  cbi(WR_PORT, WR_PIN);
  D0_PORT = b;
  sbi(WR_PORT, WR_PIN);
  _delay_us(40);
}

static inline void vfd_prepare_send_bytes(uint16_t x, uint16_t size)
{
#define STX 0x02
#define HDR 0x44
#define DAD 0x00
#define CMD 0x46

  vfd_send_byte(STX);
  vfd_send_byte(HDR);
  vfd_send_byte(DAD);
  vfd_send_byte(CMD);
  vfd_send_byte(LOW_BYTE(x));
  vfd_send_byte(HIGH_BYTE(x));
  vfd_send_byte(LOW_BYTE(size));
  vfd_send_byte(HIGH_BYTE(size));
}

static void vfd_setup(void)
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

  {
    /* Fill with zeros */
    int x, n;

    x = 0;
    n = 512 * 8;
    vfd_prepare_send_bytes(x, n);

    for (x = 0; x != n; ++x)
      vfd_send_byte(0x00);
      
    delay(10);
  }
}

static inline void vfd_print_f1(int pos, uint8_t d)
{
  int idx = 0;
  int i;
  int y = 0;

  idx = d * font1_0_cols * font1_0_size;
  for (i = 0; i != 12; ++i)
  {
    vfd_prepare_send_bytes((i + pos) * 8 + y, font1_0_size);
    vfd_send_byte(font1_0(idx++));
    vfd_send_byte(font1_0(idx++));
    vfd_send_byte(font1_0(idx++));
    delay(10);
  }
}

static inline void vfd_print_freq(uint64_t freq)
{
  int pos = 100;
    
  vfd_print_f1(pos, (freq / 10000) % 10);
  
  pos = pos + font1_0_cols + 2;
  vfd_print_f1(pos, (freq / 1000) % 10);
  pos = pos + font1_0_cols + 2;
  vfd_print_f1(pos, (freq / 100) % 10);
  pos = pos + font1_0_cols + 2;
  vfd_print_f1(pos, (freq / 10) % 10);
  pos = pos + font1_0_cols + 2;
  vfd_print_f1(pos, freq % 10);
}

void setup()
{
  encoder_setup();
  vfd_setup();
}

void loop()
{
  static uint8_t freq = 0;
  static uint8_t n_seed = 0;

  if (n_seed == 255)
    return;

  ++n_seed;
    
  {
    /* Fill with zeros */
    int x, n;

    x = 8 * 64;
    n = 64 * 8;
    vfd_prepare_send_bytes(x, n);

    for (x = 0; x != n; ++x)
    {
      if (x == 0)
      {
        vfd_send_byte(freq++);
      }
      else if (x < 8)
        vfd_send_byte(0x00);
      else
        vfd_send_byte(random(255));
    }

    delay(10);
  }
}
