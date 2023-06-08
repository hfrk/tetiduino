#define SPMCSR_ADDR         0x37

static inline void spm_fill_buffer(uint16_t, uint16_t);
static inline void spm_erase(uint16_t);
static inline void spm_write(uint16_t);
static inline void spm_rww_reenable();
static inline void read_page(uint16_t, uint8_t);

void spm_fill_buffer(uint16_t address, uint16_t data) {
    // To fill a page buffer:
    // - Set up the address in the Z pointer
    // - Set up the data in R1:R0
    // - Write 0x01 to SPMCSR
    // - Do SPM
    static const uint8_t fill = 0x01;
    __asm__ __volatile__ (
        "movw r0, %[data]\n\t"
        "out %[spmcsr], %[spmtype]\n\t"
        "spm\n\t"
        "clr r1\n\t"
        : 
        :           "z" (address),
          [data]    "r" (data),
          [spmcsr]  "i" (SPMCSR_ADDR),
          [spmtype] "r" (fill)
        : "r0"
    );
}

void spm_erase(uint16_t address) {
    // To erase a page:
    // - Set up the address in the Z pointer
    // - Write 0x03 to SPMCSR
    // - Do SPM
    static const uint8_t erase = 0x03;
    __asm__ __volatile__ (
        "out %[spmcsr], %[spmtype]\n\t"
        "spm\n\t"
        : 
        :           "z" (address),
          [spmcsr]  "i" (SPMCSR_ADDR),
          [spmtype] "r" (erase)
    );
}

void spm_write(uint16_t address) {
    // To write a page:
    // - Set up the address in the Z pointer
    // - Write 0x05 to SPMCSR
    // - Do SPM
    static const uint8_t write = 0x05;
    __asm__ __volatile__ (
        "out %[spmcsr], %[spmtype]\n\t"
        "spm\n\t"
        : 
        :           "z" (address),
          [spmcsr]  "i" (SPMCSR_ADDR),
          [spmtype] "r" (write)
    );
}

void spm_rww_reenable() {
    // To re-enable RWW section:
    // - Ensure previous SPM instruction is done
    // - Write 0x11 to SPMCSR
    // - Do SPM
    static const uint8_t rww_reenable = 0x11;
    __asm__ __volatile__ (
        "out %[spmcsr], %[spmtype]\n\t"
        "spm\n\t"
        :
        : [spmcsr]  "i" (SPMCSR_ADDR),
          [spmtype] "r" (rww_reenable)
    );
}

void read_page(uint16_t address, uint8_t length) {
    // Using LPM instruction:
    // - Load address to the Z pointer
    // - Specify destination address (default: r0)
    // - Do LPM
    char ch;

    __asm__ __volatile__ ("" : : "z" (address));
    while (length--) {
        __asm__ __volatile__ (
            "lpm %[ch], Z+\n\t"
            : [ch] "=r" (ch)
        );
        putch(ch);
    }
}
