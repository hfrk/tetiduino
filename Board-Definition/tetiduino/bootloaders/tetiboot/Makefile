# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
#
# Instructions
#
# To make bootloader .hex file:
# make atmega328
# etc...
#
# To burn bootloader .hex file:
# make atmega328_isp

# Program name
PROGRAM    = tetiboot

# Build environments
TOOLROOT = ../../tools
GCCROOT  = $(TOOLROOT)/avr/bin/
AVRDUDE_CONF = $(TOOLROOT)/avr/etc/avrdude.conf

# Parameter avrdude
ISPTOOL	  = arduino
ISPPORT	  ?= /dev/ttyUSB0
ISPBAUD   = 19200

MCU_TARGET = atmega328p
LDSECTIONS = -Wl,--section-start=.text=0x7c00

# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x2f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.

ISPFUSES = $(GCCROOT)avrdude -C $(AVRDUDE_CONF) -c $(ISPTOOL) \
           -p $(MCU_TARGET) -P $(ISPPORT) -b $(ISPBAUD) \
           -e -U lock:w:0xff:m -U efuse:w:0x$(EFUSE):m \
           -U hfuse:w:0x$(HFUSE):m -U lfuse:w:0x$(LFUSE):m
ISPFLASH = $(GCCROOT)avrdude -C $(AVRDUDE_CONF) -c $(ISPTOOL) \
           -p $(MCU_TARGET) -P $(ISPPORT) -b $(ISPBAUD) \
           -U flash:w:$(PROGRAM).hex -U lock:w:0xcf:m


CC         = $(GCCROOT)avr-gcc
OBJ        = $(PROGRAM).o
OPTIMIZE   = -Os
CFLAGS     = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) -DBAUD_RATE=115200
LDFLAGS    = $(LDSECTIONS) -Wl,--relax -Wl,--gc-sections -nostartfiles

OBJCOPY    = $(GCCROOT)avr-objcopy
OBJDUMP    = $(GCCROOT)avr-objdump
SIZE       = $(GCCROOT)avr-size

atmega328: MCU_TARGET = atmega328p
atmega328: AVR_FREQ = 20000000L
#atmega328: AVR_FREQ = 16000000L
atmega328: LDSECTIONS  = -Wl,--section-start=.text=0x7e00 -Wl,--section-start=.version=0x7ffe
atmega328: $(PROGRAM).elf
atmega328: $(PROGRAM).hex
atmega328: $(PROGRAM).lst
atmega328: $(PROGRAM).bin

atmega328_isp: atmega328
atmega328_isp: MCU_TARGET = atmega328p
# 512 byte boot, SPIEN
atmega328_isp: HFUSE = DE
# Low power xtal (16MHz) 16KCK/14CK+65ms
#atmega328_isp: LFUSE = FF
# Full swing xtal (20MHz) 16KCK/14CK+65ms
atmega328_isp: LFUSE = F7
# 2.7V brownout
atmega328_isp: EFUSE = FD
atmega328_isp: isp

# Build instructions

all: atmega328_isp
	$(MAKE) all ISPPORT=${ISPPORT} -C blink

isp: atmega328
	$(ISPFUSES)
	$(ISPFLASH)

%.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	#$(SIZE) $@ -A | grep -Po '(?<=\.text)\s+\K([0-9]+)'
	$(SIZE) $@

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex
