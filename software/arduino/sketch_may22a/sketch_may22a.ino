#include <SI4735.h>
#include <Bounce2.h>

#include "defines.h"
#include "font.h"

#define cbi(reg, bit)   ((reg) &= ~(1 << (bit)))
#define sbi(reg, bit)   ((reg) |= (1 << (bit)))

#define LOW_BYTE(b)     ((b) & 0xFF)
#define HIGH_BYTE(b)    ((b) >> 8)

SI4735 rx;
Bounce debouncer; 

static uint32_t freq = 100000;
static uint32_t freq_copy = 0;
static uint8_t freq_proc = 0;
static uint8_t buf[2048];

static inline void encoder_setup(void)
{
  sbi(ENC_A_PORT, ENC_A_PIN);
  cbi(ENC_A_DDR, ENC_A_PIN);
  attachInterrupt(0, encoder_read, FALLING);

  sbi(ENC_B_PORT, ENC_B_PIN);
  cbi(ENC_B_DDR, ENC_B_PIN);

  sbi(ENC_BTN_PORT, ENC_BTN_PIN);
  cbi(ENC_BTN_DDR, ENC_BTN_PIN);
}

static uint8_t encoder_read_threshold = 0;
static uint8_t encoder_step = 100;

static void encoder_read(void)
{
    if (encoder_read_threshold++ % 2 == 0)
    {
      if (ENC_B_PINS & (1 << ENC_B_PIN))
        freq += encoder_step;
      else
        freq -= encoder_step;
    }
}

static void button_read(void)
{
  debouncer.update();
  int value = debouncer.read();
  if ( value == LOW ) {
    encoder_step = 10;
  } else {
    encoder_step = 100;
  }
}

static inline void vfd_send_byte(uint8_t b)
{
  while (!(RDY_PINS & (1 << RDY_PIN)));
  
  cbi(WR_PORT, WR_PIN);
  D0_PORT = b;
  sbi(WR_PORT, WR_PIN);
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
    n = 256 * 8;
    vfd_prepare_send_bytes(x, n);

    for (x = 0; x != n; ++x)
      vfd_send_byte(0x00);
      
    delay(10);
  }
}

static inline void si4735_setup(void)
{
#define RESET_PIN 8
#define AM_FUNCTION 1
#define FM_FUNCTION 0
  //Serial.begin(9600);
  //while(!Serial);

  digitalWrite(RESET_PIN, HIGH);
  int16_t si4735Addr = rx.getDeviceI2CAddress(RESET_PIN);
  if ( si4735Addr == 0 ) {
    //Serial.println("Si473X not found!");
    //Serial.flush();
    while (1);
  } else {
    //Serial.print("The SI473X / SI474X I2C address is 0x");
    //Serial.println(si4735Addr, HEX);
  }

  delay(500);
  //rx.setup(RESET_PIN, FM_FUNCTION);
  //rx.setup(RESET_PIN, AM_FUNCTION);
  // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 100kHz)
  rx.setFM(6400, 12400, 9920, 10);
  freq = rx.getFrequency();
  freq *= 10;
  freq_copy = freq;
  rx.setVolume(63);
  rx.setFrequencyStep(1);
  //rx.setAutomaticGainControl(1, 0);
//rx.setAM(150, 8000, 7095, 10);
//      rx.setAvcAmMaxGain(48); // Sets the maximum gain for automatic volume control on AM mode.
//      rx.setSeekAmLimits(520, 1750);
//      rx.setVolume(63);
//      rx.setSeekAmSpacing(10); // spacing 10kHz
}

void setup()
{
  encoder_setup();
  vfd_setup();
  si4735_setup();
  digitalWrite(9, HIGH); // radio on/off input
  debouncer.attach(ENC_BTN_ARDUINO_PIN);
  debouncer.interval(5);
}

static inline void sync(void)
{
#define HALF_SCREEN (256 * 8)
  int i;
  uint8_t *ptr;

  ptr = buf;
  vfd_prepare_send_bytes(0, HALF_SCREEN);

  for (i = 0; i != HALF_SCREEN; ++i)
  {
    vfd_send_byte(*ptr++);
  }
}

static inline void print_buf(uint32_t v, uint8_t proc)
{
  uint8_t         d;
  uint16_t        off;
  const uint8_t  *p;
  uint32_t        i,
                  j,
                  k;

  i = 0;
  off = font1_0_cols * font1_0_size;

  i = 8 * (80);
  off = font1_0_cols * font1_0_size;

  for (k = 100000; k != 100; k /= 10)
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
  if (freq_copy != freq)
  {
    rx.setFrequency(freq / 10);
    freq_copy = freq;
  }
  print_buf(freq, freq_proc);
  sync();
  button_read();
}
