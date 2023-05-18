#include <inttypes.h>   // fixed width integer types
#include <avr/io.h>     // _BV and register definitions
#include "watchdog.h"
#include "stk500.h"

int main(void) __attribute__((naked)) __attribute__((section (".init9")));
void jumpToApp() __attribute__((naked));
void flashLED(uint8_t, uint8_t);
void putch(char);
char getch();
void waitForEOP();
void ignoreCommand(uint8_t);

#include "spmlpm.h"

//uint8_t buff[128];
#define buff ((uint8_t*)(0x100))

int main() {
    uint8_t ch;
    register uint16_t address;
    register uint8_t bytelength;
    
    asm volatile ("clr __zero_reg__");
    ch = MCUSR;
    MCUSR = 0;

    setWatchdog(WD_1S);
    // Set up Timer 1 for timeout counter
    TCCR1B = _BV(CS12) | _BV(CS10); // div 1024

    if (ch & _BV(EXTRF)) {
        // external reset, start bootloader
        flashLED(2, PINB2);

        // set up UART comm at 115200 baud
        UCSR0A = _BV(U2X0); //Double speed mode USART0
        UCSR0B = _BV(RXEN0) | _BV(TXEN0);
        UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
        UBRR0L = (uint8_t)( (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1);

        for (;;) {
            /* get character from UART */
            ch = getch();

            if(ch == STK_GET_PARAMETER) {
                getch();
                waitForEOP();
                putch(0x03);
            }
            else if(ch == STK_READ_SIGN) {
                waitForEOP();
                putch(SIGNATURE_0);
                putch(SIGNATURE_1);
                putch(SIGNATURE_2);
            }
            else if(ch == STK_SET_DEVICE) {
                ignoreCommand(20);
                waitForEOP();
            }
            else if(ch == STK_SET_DEVICE_EXT) {
                ignoreCommand(5);
                waitForEOP();
            }
            else if(ch == STK_UNIVERSAL) {
                ignoreCommand(4);
                waitForEOP();
            }
            else if(ch == STK_LOAD_ADDRESS) {
                uint8_t addr_low = getch();
                uint8_t addr_high = getch();
                address = (addr_high << 8) | (addr_low);
                address *= 2; // to byte address
                waitForEOP();
            }
            else if(ch == STK_PROG_PAGE) {
                uint8_t* sram;
                uint8_t bytelength, wordlength;
                uint16_t word;
                uint16_t addr = address;

                getch();
                bytelength = getch();
                wordlength = bytelength / 2;
                getch();

                sram = buff;
                do {
                    *sram = getch();
                    sram++;
                } while (--bytelength);

                waitForEOP();

                sram = buff;
                do {
                    word = (*sram);
                    sram++;
                    word |= (*sram) << 8;
                    sram++;
                    spm_fill_buffer(addr, word);
                    addr += 2;
                } while (--wordlength);
                spm_erase(address);
                while(SPMCSR & _BV(SPMEN));
                spm_write(address);
                while(SPMCSR & _BV(SPMEN));
                spm_rww_reenable();
                // //buff = (uint8_t*) 0x100; // RAM start
                // for(uint8_t i = 0; i < 128; i++) {
                //     buff[i] = getch();
                //     //*buff++ = getch();
                // }
            }
            else if(ch == STK_READ_PAGE) {
                getch();
                bytelength = getch();
                getch();
                waitForEOP();

                read_page(address, bytelength);
                // //buff = (uint8_t*) 0x100; // RAM start
                // for(uint8_t i = 0; i < 128; i++) {
                //     putch(buff[i]);
                //     //putch(*buff++);
                // } 
            }
            else if (ch == STK_LEAVE_PROGMODE) {
                waitForEOP();
            }
            else {
                waitForEOP();
            }

            putch(STK_OK);
        }

        // setWatchdog(WD_16MS);
        // while(1) {
        //     flashLED(6, PINB2);
        // }
    }
    else {
        // other reset source, jump directly to app
        flashLED(2, PINB3);
        jumpToApp();
    }
}

void flashLED(uint8_t count, uint8_t pin) {
    /* Set LED pin as output */
    DDRB |= _BV(pin);

    do {
        TCNT1 = -(F_CPU/(1024*16));
        TIFR1 = _BV(TOV1);
        // wait until timer overflow
        while(!(TIFR1 & _BV(TOV1)));
        // toggle pin
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

void putch(char ch) {
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch; 
}

char getch() {
    char ch;
    while (!(UCSR0A & _BV(RXC0)));
    if (!(UCSR0A & _BV(FE0))) {
        // Frame not error
        resetWatchdog();
    }
    ch = UDR0;
    return ch;
}

void waitForEOP() {
    if(getch() == CRC_EOP) {
        putch(STK_INSYNC);
    }
    else {
        putch(STK_NOSYNC);
    }
}

void ignoreCommand(uint8_t length) {
    while (length--) {
        getch();
    };
}
