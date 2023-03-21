#include <inttypes.h> // fixed length integer types
#include <avr/io.h> // _BV and register definitions
#include "watchdog.h"

int main(void) __attribute__((naked)) __attribute__((section (".init9")));
void jumpToApp() __attribute__((naked));
void flashLED(uint8_t, uint8_t);

int main() {
    uint8_t ch;
    
    asm volatile ("clr __zero_reg__");

    ch = MCUSR;
    MCUSR = 0;

    setWatchdog(WD_1S);

    // Set up Timer 1 for timeout counter
    TCCR1B = _BV(CS12) | _BV(CS10); // div 1024

    if (ch & _BV(EXTRF)) {
        // external reset
        //setWatchdog(WD_16MS);
        while(1) {
            flashLED(6, PINB2);
        }
    }
    else {
        // other reset source
        flashLED(2, PINB1);
        jumpToApp();
    }
}

void flashLED(uint8_t count, uint8_t pin) {
  /* Set LED pin as output */
  DDRB |= _BV(pin);

  do {
    TCNT1 = -(F_CPU/(1024*16));
    TIFR1 = _BV(TOV1);
    while(!(TIFR1 & _BV(TOV1)));
    PINB |= _BV(pin);
  } while (--count);
}

void jumpToApp() {
    // Jump to app start address (0x0000)
    // this is done by setting the Z register (R31:R30) to 0
    // and do indirect jump (IJMP)
    setWatchdog(WD_OFF);
    __asm__ __volatile__ (
        "clr r30\n"
        "clr r31\n"
        "ijmp\n"
    );
}
