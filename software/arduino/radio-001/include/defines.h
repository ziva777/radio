#ifndef _DEFINES_H_
#define _DEFINES_H_

#define cbi(reg, bit)   ((reg) &= ~(1 << (bit)))
#define sbi(reg, bit)   ((reg) |= (1 << (bit)))

#define RDY_PORT    PORTC
#define RDY_DDR     DDRC
#define RDY_PIN     PIN6
#define RDY_PINS    PINC

#define WR_PORT     PORTC
#define WR_DDR      DDRC
#define WR_PIN      PIN7

#define D7_PORT     PORTA

#define D7_DDR      DDRA
#define D7_PIN      PIN7

#define D6_PORT     PORTA
#define D6_DDR      DDRA
#define D6_PIN      PIN6

#define D5_PORT     PORTA
#define D5_DDR      DDRA
#define D5_PIN      PIN5

#define D4_PORT     PORTA
#define D4_DDR      DDRA
#define D4_PIN      PIN4

#define D3_PORT     PORTA
#define D3_DDR      DDRA
#define D3_PIN      PIN3

#define D2_PORT     PORTA
#define D2_DDR      DDRA
#define D2_PIN      PIN2

#define D1_PORT     PORTA
#define D1_DDR      DDRA
#define D1_PIN      PIN1

#define D0_PORT     PORTA
#define D0_DDR      DDRA
#define D0_PIN      PIN0

#define ENC_A_PORT  PORTE
#define ENC_A_DDR   DDRE
#define ENC_A_PIN   PIN4

#define ENC_B_PORT  PORTB
#define ENC_B_DDR   DDRB
#define ENC_B_PIN   PIN5
#define ENC_B_PINS  PINB

#define ENC_BTN_PORT PORTB
#define ENC_BTN_DDR  DDRB
#define ENC_BTN_PIN  PIN6
#define ENC_BTN_ARDUINO_PIN  12

#define ENC2_A_PORT  PORTB
#define ENC2_A_DDR   DDRB
#define ENC2_A_PIN   PIN0
#define ENC2_A_ARDUINO_PIN  53

#define ENC2_B_PORT  PORTB
#define ENC2_B_DDR   DDRB
#define ENC2_B_PIN   PIN1
#define ENC2_B_PINS  PINB
#define ENC2_B_ARDUINO_PIN  52

#define ENC2_BTN_PORT PORTB
#define ENC2_BTN_DDR  DDRB
#define ENC2_BTN_PIN  PIN2
#define ENC2_BTN_ARDUINO_PIN  51

#define BTN1_PORT PORTH
#define BTN1_DDR  DDRH
#define BTN1_PIN  PIN3
#define BTN1_ARDUINO_PIN  6

#define BTN2_PORT PORTH
#define BTN2_DDR  DDRH
#define BTN2_PIN  PIN4
#define BTN2_ARDUINO_PIN  7

#define AMP_MUTE_ARDUINO_PIN  10
#define AMP_MODE_ARDUINO_PIN  9

#define BTN_MUTE_ARDUINO_PIN  5
#define BTN_DIM_ARDUINO_PIN  4

#define AUX_ARDUINO_PIN 3

#endif // _DEFINES_H_