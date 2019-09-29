###############################################################################
# Makefile for the project AVR
###############################################################################

## General Flags
PROJECT = firmware
MCU = atmega88pa
TARGET = firmware

CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON) \
		 -Iport -I. \
		 -Ifreemodbus/modbus/rtu -Ifreemodbus/modbus/ascii -Ifreemodbus/modbus/include 
CFLAGS += -Wall -gstabs -DF_CPU=1000000UL  -Os -Wall -Wstrict-prototypes

CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gstabs

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Objects that must be built in order to link
#OBJECTS = excoils.o
OBJECTS = firmware.o
MBPORTOBJECTS = port/portserial.o \
			port/portevent.o \
			port/porttimer.o \
			port/mbcrc.o
MBOBJECTS = freemodbus/modbus/mb.o \
			freemodbus/modbus/rtu/mbrtu.o \
			freemodbus/modbus/ascii/mbascii.o \
			freemodbus/modbus/functions/mbfunccoils.o \
			freemodbus/modbus/functions/mbfuncdiag.o \
			freemodbus/modbus/functions/mbfuncholding.o \
			freemodbus/modbus/functions/mbfuncinput.o \
			freemodbus/modbus/functions/mbfuncother.o \
			freemodbus/modbus/functions/mbfuncdisc.o \
			freemodbus/modbus/functions/mbutils.o 


## Build
all: $(TARGET).elf $(TARGET).cof $(TARGET).hex $(TARGET).eep 

## Compile
firmware.o: firmware.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET).elf: $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(MBPORTOBJECTS) $(MBOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET).elf

%.hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.cof: $(TARGET).elf
	touch demo.cof
#	$(OBJCOPY) --debugging \
#		-O elf $< $@

%.eep: $(TARGET).elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	$(OBJCOPY) -h -S $< > $@

flash:
#	$(AVRDUDE) -p m8 -c avr109 -b 9600 -P /dev/ttyUSB0 -U flash:w:$(TARGET).hex
	$(AVRDUDE) -p m88p -c ponyser -i 400 -p /dev/ttyUSB0 -U flash:w:$(TARGET).hex

#size: ${TARGET}
#	@echo
#	@sh avr-mem.sh ${TARGET} ${MCU}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS) 
	-rm -rf $(TARGET).elf $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).map
	-rm -rf dep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

