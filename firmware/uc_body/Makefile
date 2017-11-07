#
# Makefile for AVR
#
# @date 01.09.2013
# @author Denise Ratasich
# 

#-------------------------------------------------------------------------
# project specific things
# change these definitions for new projects
#-------------------------------------------------------------------------

# fill in a project name (name of the hexfile generated)
PROJNAME = firmware

# target architecture
MCU	= atmega2560

BINFORMAT = ihex

# programming parameters
PORT = /dev/ttyACM3
MC = ATmega2560
DEVICE = stk500v2
BAUD = 115200

# Flags
CFLAGS  = -mmcu=$(MCU) -Wall -O0 -I"include"
LDFLAGS	= -mmcu=$(MCU)
OCFLAGS	= -O $(BINFORMAT)
PRFLAGS = -c $(DEVICE) -b $(BAUD) -p $(MC) -P $(PORT) -e -v -v
#EEPFLAGS= --set-section-flags=.eeprom="alloc,load" --change-section-address .eeprom-0x810000

# specify source files
SRC		:= $(wildcard src/*.c)
OBJS		:= $(SRC:.c=.o)

#-------------------------------------------------------------------------
# targets
#-------------------------------------------------------------------------

all: bin/$(PROJNAME).hex bin/$(PROJNAME).elf

# example: avr-objcopy -O ihex demo.elf demo.hex
bin/%.hex: bin/%.elf
	avr-objcopy $(OCFLAGS) $< $@

bin/%.elf: $(OBJS)
	mkdir -p bin
	avr-gcc $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $<


.PHONY: flash
# avrdude -c stk500v2 -p ATmega16 -P /dev/ttyUSB0 -e -U flash:w:demo.hex
flash: bin/$(PROJNAME).hex
	avrdude $(PRFLAGS) -U flash:w:bin/$(PROJNAME).hex


.PHONY: clean
clean:
	rm -f src/*~
	rm -f src/*.[do]
	rm -f include/*~
	rm -f -r bin
