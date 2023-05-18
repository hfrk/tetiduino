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


# program name should not be changed...
PROGRAM    = tetiboot

# enter the parameters for the avrdude isp tool
ISPTOOL	   = arduino
ISPPORT	   ?= /dev/ttyUSB0
ISPSPEED   = -b 19200

MCU_TARGET = atmega328p
LDSECTIONS  = -Wl,--section-start=.text=0x7c00 -Wl,--section-start=.version=0x7ffe

# Build environments

TOOLROOT = ../../tools
GCCROOT = $(TOOLROOT)/avr/bin/
AVRDUDE_CONF = -C$(TOOLROOT)/avr/etc/avrdude.conf

# End of build environment code.


# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x2f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.

ISPFUSES    = $(GCCROOT)avrdude $(AVRDUDE_CONF) -c $(ISPTOOL) \
              -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
              -e -U lock:w:0x3f:m -U efuse:w:0x$(EFUSE):m \
              -U hfuse:w:0x$(HFUSE):m -U lfuse:w:0x$(LFUSE):m
ISPFLASH    = $(GCCROOT)avrdude $(AVRDUDE_CONF) -c $(ISPTOOL) \
              -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
              -U flash:w:$(PROGRAM)_$(TARGET).hex -U lock:w:0x0f:m

OBJ        = $(PROGRAM).o
OPTIMIZE = -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls

CC         = $(GCCROOT)avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) $(DEFS)
override LDFLAGS       = $(LDSECTIONS) -Wl,--relax -Wl,--gc-sections -nostartfiles -nostdlib

OBJCOPY        = $(GCCROOT)avr-objcopy
OBJDUMP        = $(GCCROOT)avr-objdump

SIZE           = $(GCCROOT)avr-size

atmega328: TARGET = atmega328
atmega328: MCU_TARGET = atmega328p
atmega328: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
atmega328: AVR_FREQ = 20000000L
#atmega328: AVR_FREQ = 16000000L
atmega328: LDSECTIONS  = -Wl,--section-start=.text=0x7e00 -Wl,--section-start=.version=0x7ffe
atmega328: $(PROGRAM)_atmega328.hex
atmega328: $(PROGRAM)_atmega328.lst
atmega328: $(PROGRAM)_atmega328.bin

atmega328_isp: atmega328
atmega328_isp: TARGET = atmega328
atmega328_isp: MCU_TARGET = atmega328p
# 512 byte boot, SPIEN
atmega328_isp: HFUSE = DE
#atmega328_isp: set_hfuse
# Low power xtal (16MHz) 16KCK/14CK+65ms
#atmega328_isp: LFUSE = FF
# Full swing xtal (20MHz) 16KCK/14CK+65ms
atmega328_isp: LFUSE = F7
# 2.7V brownout
atmega328_isp: EFUSE = FD
atmega328_isp: isp

# Fuse bytes, 0 means programmed/enabled
# Fuse high byte:
# 7: reset disable
# 6: debugwire enable
# 5: SPI enable
# 4: Watchdog always on
# 3: preserve EEPROM
# 2: boot size 1
# 1: boot size 0
# 0: select reset vector

# Fuse low byte:

set_hfuse:
	$(eval HFUSE = $(shell \
		if [ ${BYTES} -gt 512 ]; \
		    then echo DC; \
            else echo DE; \
        fi;) \
	)

test:
	${eval VALUE := $(shell echo "5000")}
	if [ ${VALUE} -gt 512 ]; \
		then echo foo; \
		else echo bar; \
	fi;

all: atmega328_isp
	$(MAKE) all ISPPORT=${ISPPORT} -C blink

#
# Generic build instructions
#
#

isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

%.size: %.elf
	$(SIZE) $<
	$(eval BYTES:=$(shell $(SIZE) $< -A \
		| grep -Po '(?<=\.text)\s+\K([0-9]+)'))
	echo $(BYTES) > $@

%.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf %.size
	$(OBJCOPY) -j .text -j .data -j .version \
	--set-section-flags .version=alloc,load \
	-O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .version \
	--set-section-flags .version=alloc,load \
	-O binary $< $@

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex *.size
