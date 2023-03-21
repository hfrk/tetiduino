TOOLROOT = ../../../tools/
GCCROOT = $(TOOLROOT)avr/bin/

CC = $(GCCROOT)avr-gcc
OBJDUMP = $(GCCROOT)avr-objdump
OBJCOPY = $(GCCROOT)avr-objcopy
AVRDUDE = $(GCCROOT)avrdude
AVRDUDE_CONF = -C$(TOOLROOT)/avr/etc/avrdude.conf

CFLAGS = -mmcu=atmega328p -DF_CPU=16000000

build: blink.c
	$(CC) $(CFLAGS) $^ -c -o blink.o

blink.elf: blink.o
	$(CC) $(CFLAGS) -o $@ $^

blink.lst: blink.elf
	$(OBJDUMP) -h -S $< > $@

blink.hex: blink.elf
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O ihex $< $@

upload: blink.elf
	$(AVRDUDE) $(AVRDUDE_CONF) -c arduino -p atmega328p \
        -P /dev/ttyUSB0 -b 19200 -D -Uflash:w:blink.elf

upload2: blink.hex
	$(AVRDUDE) $(AVRDUDE_CONF) -c arduino -p atmega328p \
        -P /dev/ttyUSB0 -b 19200 -D -Uflash:w:blink.hex:i

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex *.size
