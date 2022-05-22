#include "defines.h"
#include "font.h"

#define cbi(reg, bit)   ((reg) &= ~(1 << (bit)))
#define sbi(reg, bit)   ((reg) |= (1 << (bit)))

#define LOW_BYTE(b)     ((b) & 0xFF)
#define HIGH_BYTE(b)    ((b) >> 8)

static uint16_t freq = 54321;
static uint8_t freq_proc = 0;
static uint8_t buf[1024];
static uint8_t encoder_pos;
static uint8_t encoder_last_pos;

static inline void encoder_setup(void)
{
  sbi(ENC_A_PORT, ENC_A_PIN);
  cbi(ENC_A_DDR, ENC_A_PIN);

  sbi(ENC_B_PORT, ENC_B_PIN);
  cbi(ENC_B_DDR, ENC_B_PIN);

  sbi(ENC_BTN_PORT, ENC_BTN_PIN);
  cbi(ENC_BTN_DDR, ENC_BTN_PIN);

  encoder_last_pos = digitalRead(ENC_A_PIN);
}

static inline void encoder_read(void)
{
#define FREQ_PROC_INC 100
  encoder_pos = digitalRead(A0);
  if (encoder_pos != encoder_last_pos)
  {
    if (encoder_pos != digitalRead(A1))
    {
      freq_proc += FREQ_PROC_INC;
      if (freq_proc == 100)
      {
        freq_proc = 0;
        freq += 1;
      }
    }
    else
    {
      if (freq_proc == 0)
      {
        freq_proc = 100 - FREQ_PROC_INC;
        freq -= 1;
      }
      else
        freq_proc -= FREQ_PROC_INC;
    }
    
    encoder_last_pos = encoder_pos;
  }
}

static inline void vfd_send_byte(uint8_t b)
{ 
  cbi(WR_PORT, WR_PIN);
  D0_PORT = b;
  sbi(WR_PORT, WR_PIN);
  _delay_us(42);
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

static inline void vfd_setup(void)
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

void setup()
{
  encoder_setup();
  vfd_setup();
}

static inline void sync(void)
{
#define HALF_SCREEN (128 * 8)
  int i;
  uint8_t *ptr;

  ptr = buf;
  vfd_prepare_send_bytes(0, HALF_SCREEN);

  for (i = 0; i != HALF_SCREEN; ++i)
  {
    encoder_read();
    vfd_send_byte(*ptr++);
  }
}

static inline void print_buf(uint16_t v, uint8_t proc)
{
  uint8_t         d;
  uint16_t        off;
  const uint8_t  *p;
  uint16_t        i,
                  j,
                  k;

  i = 0;
  off = font1_0_cols * font1_0_size;

  for (k = 10000; k != 100; k /= 10)
  {
    d = (v / k) % 10;
    p = font_1_digits_data + off * d;
    
    for (j = 0; j < font1_0_size * font1_0_cols; j+= font1_0_size, i += 8)
    {
      /* memcpy(buf+i, p+j, font1_0_size); */
      *(buf + i + 0) = (pgm_read_byte(&p[j + 0]));
      *(buf + i + 1) = (pgm_read_byte(&p[j + 1]));
      *(buf + i + 2) = (pgm_read_byte(&p[j + 2]));
    }
    
    i += 8 * 2;
  }

  i += 8 * 0;
  /**(buf + i + 0) = 0x00;*/
  /**(buf + i + 1) = 0x00;*/
  *(buf + i + 2) = 0x60;
  /**(buf + i + 3) = 0x00;*/
  i += 8 * 1;
  /**(buf + i + 0) = 0x00;*/
  /**(buf + i + 1) = 0x00;*/
  *(buf + i + 2) = 0x60;
  /**(buf + i + 3) = 0x00;*/
  i += 8 * 3;

  for (k = 100; k != 0; k /= 10)
  {
    d = (v / k) % 10;
    p = font_1_digits_data + off * d;
    
    for (j = 0; j < font1_0_size * font1_0_cols; j+= font1_0_size, i += 8)
    {
      /* memcpy(buf+i, p+j, font1_0_size); */
      *(buf + i + 0) = (pgm_read_byte(&p[j + 0]));
      *(buf + i + 1) = (pgm_read_byte(&p[j + 1]));
      *(buf + i + 2) = (pgm_read_byte(&p[j + 2]));
    }
    
    i += 8 * 2;
  }

  off = font2_0_cols * font2_0_size;
  
  for (k = 10; k != 0; k /= 10)
  {
    d = (proc / k) % 10;
    p = font_2_digits_data + off * d;

    for (j = 0; j < font2_0_size * font2_0_cols; j+= font2_0_size, i += 8)
    {
      /* memcpy(buf+i, p+j, font1_0_size); */
      *(buf + i + 0) = (pgm_read_byte(&p[j + 0]));
      *(buf + i + 1) = (pgm_read_byte(&p[j + 1]));
      *(buf + i + 2) = (pgm_read_byte(&p[j + 2]));
    }
    
    i += 8 * 1;
  }
}

void loop()
{
  print_buf(freq, freq_proc);
  sync();
}
