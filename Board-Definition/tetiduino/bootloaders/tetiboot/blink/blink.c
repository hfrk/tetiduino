#include <avr/io.h>

int main() {
    int foo = 0;
    TCCR1B = _BV(CS12) | _BV(CS10);
    DDRB |= _BV(PINB1) | _BV(PINB2);
    do {
        TCNT1 = -(F_CPU/(1024*4));
        //TCNT1 = 0;
        TIFR1 = _BV(TOV1);
        while(!(TIFR1 & _BV(TOV1)));
        PINB |= foo ? _BV(PINB1) : _BV(PINB2);
        foo = !foo;
    } while (1);
    return 0;
}
