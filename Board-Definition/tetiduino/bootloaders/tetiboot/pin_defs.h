#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
/* Onboard LED is connected to pin PB5 in Arduino NG, Diecimila, and Duemilanove */ 
#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN     PINB
#define LED         PINB1
#endif

