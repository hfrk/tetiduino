#include <inttypes.h>
#include <avr/io.h>

/* Watchdog settings */
#define WD_OFF    (0)
#define WD_16MS   (_BV(WDE))
#define WD_32MS   (_BV(WDP0) | _BV(WDE))
#define WD_64MS   (_BV(WDP1) | _BV(WDE))
#define WD_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WD_250MS  (_BV(WDP2) | _BV(WDE))
#define WD_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WD_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WD_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WD_4S     (_BV(WDP3) | _BV(WDE))
#define WD_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))

static inline void setWatchdog(uint8_t timeout) {
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = timeout;
}

static inline void resetWatchdog() {
    __asm__ __volatile__ ("wdr\n");
}
