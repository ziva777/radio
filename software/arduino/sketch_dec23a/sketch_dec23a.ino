#include <Bounce2.h>
#include <SI4735.h>
#include <util/atomic.h>
#include <GyverEncoder.h>
#include <TimerOne.h>

#include "defines.h"
#include "untitled.h"
#include "vfd.h"
#include "filter.h"
#include "fonts.h"

SI4735 rx;
Filter filter_pilot;
Filter filter_rssi;
Filter filter_snr;
Encoder enc2(ENC2_A_ARDUINO_PIN,
             ENC2_B_ARDUINO_PIN,
             ENC2_BTN_ARDUINO_PIN);
Bounce2::Button button = Bounce2::Button();

static uint8_t enc_step = 100; /* kHz */
static volatile uint32_t freq = 87000; /* kHz */

void
enc_read(void)
{
  static uint32_t enc_read_threshold = 0;

  if (enc_read_threshold++ % 2 != 0)
    return;

  if (ENC_B_PINS & (1 << ENC_B_PIN)) {
    if (freq < 108000)
      freq += enc_step;
  } else {
    if (freq > 87000)
      freq -= enc_step;
  }
}

inline void
enc_setup(void)
{
  sbi(ENC_A_PORT, ENC_A_PIN);
  cbi(ENC_A_DDR, ENC_A_PIN);
  attachInterrupt(0, enc_read, FALLING);

  sbi(ENC_B_PORT, ENC_B_PIN);
  cbi(ENC_B_DDR, ENC_B_PIN);

  sbi(ENC_BTN_PORT, ENC_BTN_PIN);
  cbi(ENC_BTN_DDR, ENC_BTN_PIN);

//  pinMode(LED_BUILTIN, OUTPUT);
}

static uint32_t enc2_cnt;

void
enc2_update(void)
{
  enc2.tick();

  if (enc2.isTurn()) {
    if (enc2.isRight())
      ++enc2_cnt;
    if (enc2.isLeft())
      --enc2_cnt;
  }
}

inline void
buttons_setup(void)
{
  pinMode(ENC_BTN_ARDUINO_PIN, INPUT_PULLUP);
  pinMode(BT1_BTN_ARDUINO_PIN, INPUT_PULLUP);
  pinMode(BT2_BTN_ARDUINO_PIN, INPUT_PULLUP);

  pinMode(BT2_MUTE_ARDUINO_PIN, INPUT_PULLUP);
//  pinMode(BT3_DIM_ARDUINO_PIN, INPUT_PULLUP);

  pinMode(AMP_MUTE_ARDUINO_PIN, OUTPUT);
  digitalWrite(AMP_MUTE_ARDUINO_PIN, LOW);
  
  pinMode(AMP_MODE_ARDUINO_PIN, OUTPUT);
  digitalWrite(AMP_MODE_ARDUINO_PIN, LOW);
  
  enc2.setType(TYPE2);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(enc2_update);

  button.attach(BT3_DIM_ARDUINO_PIN, INPUT_PULLUP);
  button.interval(5);
  button.setPressedState(LOW); 
}

inline void radio_setup(void) {
#define RESET_PIN 8
#define FM_FUNCTION 0
  digitalWrite(RESET_PIN, HIGH);
  
  int16_t si4735Addr = rx.getDeviceI2CAddress(RESET_PIN);
  if ( si4735Addr == 0 ) {
    while (1);
  }
  
  delay(500);
  rx.setup(RESET_PIN, FM_FUNCTION);
  rx.setFM(8700, 10800, 8700, 10);
//  rx.setAutomaticGainControl(0, 26);
  rx.setAutomaticGainControl(0, 0);
  rx.setVolume(63);
}

void setup() {
  buttons_setup();
  vfd_setup();
  enc_setup();
  
  radio_setup();
  pinMode(LED_BUILTIN, OUTPUT);
  filter_pilot = FilterCreate(0.5, 0.5, 1);
  filter_rssi = FilterCreate(0.5, 10, 0.0001);
  filter_snr = FilterCreate(0.5, 10, 0.0001);
}

void loop() {
  static uint32_t f1, f2 = 0;
  uint64_t *scr = (uint64_t *) screen_buf;
  uint32_t f;
  int32_t delta;
  static uint8_t snr_prev;
  static uint8_t rssi_prev;
  static bool pilot_prev;
  uint8_t snr = 0;
  uint8_t rssi = 0;
  bool pilot = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    f1 = freq;
  }

  if (f1 == f2) {    
    rx.getCurrentReceivedSignalQuality();
    snr = rx.getCurrentSNR();
    rssi = rx.getCurrentRSSI();
    pilot = rx.getCurrentPilot();

    rssi = FilterUpdate(&filter_rssi, rssi);
    snr = FilterUpdate(&filter_snr, snr);
    // pilot = FilterUpdate(&filter_pilot, pilot);

//    if (snr == snr_prev && rssi == rssi_prev && pilot == pilot_prev)
//      return;

    snr_prev = snr;
    rssi_prev = rssi;
    pilot_prev = pilot;
  }

//  rx.setVolume(0);

  if (f2 != 0) {
    delta = f1;
    delta -= f2;
    delta /= 100;

#if 1
   
      
  if (delta != 0) {
    // Up
    rx.setFrequency(f1 / 10);
  } else {
    // Down
//    rx.setFrequency(f2 / 10);
  }

//    if (delta / 10 > 0) {
//      rx.setFrequencyStep(100);
//      for (int32_t i = delta / 10; i != 0; --i) {
//        delta -= 10;
//        rx.frequencyUp();
//      }
//    } else if (delta / 10 < 0) {
//      rx.setFrequencyStep(100);
//      for (int32_t i = delta / 10; i != 0; ++i) {
//        delta += 10;
//        rx.frequencyDown();
//      }
//    }
//
//    rx.setFrequencyStep(10);
//
//    if (delta > 0)
//      for (int32_t i = delta; i != 0; --i)
//        rx.frequencyUp();
//    else
//      for (int32_t i = delta; i != 0; ++i)
//        rx.frequencyDown();

//    rx.setAutomaticGainControl(1, 26);
#endif
  }

  f2 = f1;
  f = f1 - 87000;
  f /= 100;

  vfd_load(untitled_raw);

  {
    font_write(&font_hw_sign, 0,  8,  f2 / 10);
  }

#define SCALE_BEGIN 22
  for (int i = 5; i != 8; ++i) {
    screen_buf[8 * (SCALE_BEGIN + f - 1) + i] = 0x00;
    screen_buf[8 * (SCALE_BEGIN + f    ) + i] |= 0xFF;
    screen_buf[8 * (SCALE_BEGIN + f + 1) + i] = 0x00;
  }

  {
    // frame counter
    static uint64_t fr_cnt;
    scr[255] = ++fr_cnt;  
  }

  {
    int x, off;

    if (f1 >= 100000) {
      x = 122 - 11 * 3;
      off = 11 * 3 + 3;
    } else {
      x = 122 - 11 * 2;
      off = 11 * 2 + 3;
    }
    
      font_write(&font_dot16_w, x, 22, f1 / 1000);
      font_write(&font_dot16_w, x + off, 22, (f1/100) % 10);
  }

  
  //font_write(&FONT_5x5,     0,  0, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  //font_write(&font_hw_sign, 0,  8, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  //font_write(&font_hw_sign, 0,  8,  snr);
  font_write(&font_hw_sign, 0,  16, rssi);
  //font_write(&font_dot16,   16, 24, f1 / 1000);
  //font_write(&font_dot11,   16 + 9 * 3, 28, (f1/100) % 10);
  font_write(&FONT_5x5, 10 * 6, 0, "BROADCAST 87-108 MHZ");

  if (snr >= 4)
    font_write(&FONT_5x5, 126 + 60, 32 - 8 * 2, pilot ? "STEREO" : "MONO");

  char bar[64] = {0};
  static uint8_t bar_counter = 0;
  const char border[] = "011111111111111111111111111111111111111111111111111111111111110";

  for (int i = 0; i < snr / 1; ++i)
    bar[i] = '0';

  font_write(&font_hbar, 126 + 44, 32 - 8 * 1, border);
  font_write(&font_hbar, 126 + 44, 32 - 8 * 1, bar);
  
  for (int i = 0; i < rssi / 1; ++i)
    bar[i] = '0';

  const char spectr[] = "0123456789:;<=>?";

  font_write(&font_hbar, 126 + 44, 32 - 8 * 0, border);
  font_write(&font_hbar, 126 + 44, 32 - 8 * 0, bar);
  //font_write(&font_hbar, 0, 0, "00");

  {
    static char bar2[64] = {0};
    
    if (++bar_counter % 8 == 0) {
      for (int i = 0; i < 16; ++i)
        bar2[i] = random('0' + 1, '0' + 16);
    }
  
    bar2[16] = '\0';
    font_write(&font_sbar, 22, 22, bar2);
  }

  {
    int res;
    static int res1, res2;
    
    res = digitalRead(BT1_BTN_ARDUINO_PIN);
    if (res == HIGH)
      font_write(&font_hw_sign, 0,  0, "H");
    else
      font_write(&font_hw_sign, 0,  0, "L");

   res = digitalRead(BT2_MUTE_ARDUINO_PIN);
    if (res == HIGH)
      font_write(&font_hw_sign, 8,  0, "H");
    else
      font_write(&font_hw_sign, 8,  0, "L");

   digitalWrite(AMP_MUTE_ARDUINO_PIN, res);

    res = digitalRead(BT2_BTN_ARDUINO_PIN);
    if (res == HIGH)
      font_write(&font_hw_sign, 16,  0, "H");
    else
      font_write(&font_hw_sign, 16,  0, "L");

    digitalWrite(AMP_MODE_ARDUINO_PIN, res);

    res = digitalRead(ENC_BTN_ARDUINO_PIN);
    if (res == HIGH)
      font_write(&font_hw_sign, 24,  0, "H");
    else
      font_write(&font_hw_sign, 24,  0, "L");

    button.update();
    if (button.pressed())
      res = HIGH;
    else
      res = LOW;
    
    /*if (res == HIGH)
      font_write(&font_hw_sign, 32,  0, "H");
    else
      font_write(&font_hw_sign, 32,  0, "L");*/
    font_write(&font_hw_sign, 32,  0, dim);

    if (res == HIGH)
      vfd_dim();
  }

  {
    uint32_t cnt;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      cnt = enc2_cnt;
    }

    font_write(&font_hw_sign, 38,  0, cnt);
  }
  
  vfd_sync();
//  rx.setVolume(63);
}
