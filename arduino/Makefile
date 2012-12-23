# Kerry Wong's Arduino 1.0 Makefile
# http://www.kerrywong.com
#
# $Id$

TARGET = friendlyhal
# Change this to match your arduino installation directory
INSTALL_DIR = $(HOME)/arduino
PORT = /dev/ttyUSB0
UPLOAD_RATE = 57600
AVRDUDE_PROGRAMMER = stk500v1
MCU = atmega328p
F_CPU = 16000000

VERSION=1.0
ARDUINO = $(INSTALL_DIR)/hardware/arduino/cores/arduino
VARIANTS = $(INSTALL_DIR)/hardware/arduino/variants/standard
ARDUINO_LIB = $(INSTALL_DIR)/libraries
AVR_TOOLS_PATH = /usr/bin
AVRDUDE_PATH = $(INSTALL_DIR)/hardware/tools

#Note that if your program has dependencies other than those
#already listed below, you will need to add them accordingly.
C_MODULES =  \
$(ARDUINO)/wiring_pulse.c \
$(ARDUINO)/wiring_analog.c \
$(ARDUINO)/wiring.c \
$(ARDUINO)/wiring_digital.c \
$(ARDUINO)/WInterrupts.c \
$(ARDUINO)/wiring_shift.c \

CXX_MODULES = \
$(ARDUINO)/Tone.cpp \
$(ARDUINO)/WMath.cpp \
$(ARDUINO)/Print.cpp \
$(ARDUINO)/HardwareSerial.cpp \
$(ARDUINO)/CDC.cpp \
$(ARDUINO)/HID.cpp \
$(ARDUINO)/IPAddress.cpp \
$(ARDUINO)/new.cpp \
$(ARDUINO)/Stream.cpp \
$(ARDUINO)/USBCore.cpp \
$(ARDUINO)/WMath.cpp \
$(ARDUINO)/WString.cpp \
$(ARDUINO)/main.cpp \
$(ARDUINO_LIB)/EEPROM/EEPROM.cpp \
TempHumSensor/TempHumSensor.cpp \
PIRSensor/PIRSensor.cpp

CXX_APP = $(ARDUINO)/main.cpp
MODULES = $(C_MODULES) $(CXX_MODULES)
SRC = $(C_MODULES)
CXXSRC = $(CXX_MODULES) $(CXX_APP)
FORMAT = ihex

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG = stabs
DEBUG =

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)L -DARDUINO=$(VERSION)
CXXDEFS = -DF_CPU=$(F_CPU)L -DARDUINO=$(VERSION) -D__AVR_ATmega328P__

# Place -I options here
CINCS = -I$(ARDUINO)  -I$(VARIANTS) -I$(ARDUINO_LIB)
CXXINCS = -I$(ARDUINO) -I$(VARIANTS) -I$(ARDUINO_LIB) -ITempHumSensor/ -IPIRSensor/

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
#CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
#CWARN = -Wall -Wstrict-prototypes
#CWARN = -Wall   # show all warnings
CWARN = -w #suppress all warnings
####CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CTUNING = -ffunction-sections -fdata-sections
CXXTUNING = -fno-exceptions -ffunction-sections -fdata-sections
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) -O$(OPT) $(CWARN) $(CTUNING) $(CDEFS) $(CINCS) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEBUG) -O$(OPT) $(CWARN) $(CXXTUNING) $(CDEFS) $(CINCS)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
LDFLAGS = -O$(OPT) -lm -Wl,--gc-sections


# Programming support using avrdude. Settings and variables.
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:applet/main.hex

AVRDUDE_FLAGS = -V -F -C $(INSTALL_DIR)/hardware/tools/avrdude.conf \
-p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
-b $(UPLOAD_RATE) -D

# Program settings
CC = $(AVR_TOOLS_PATH)/avr-gcc
CXX = $(AVR_TOOLS_PATH)/avr-g++
LD = $(AVR_TOOLS_PATH)/avr-gcc
OBJCOPY = $(AVR_TOOLS_PATH)/avr-objcopy
OBJDUMP = $(AVR_TOOLS_PATH)/avr-objdump
AR  = $(AVR_TOOLS_PATH)/avr-ar
SIZE = $(AVR_TOOLS_PATH)/avr-size
NM = $(AVR_TOOLS_PATH)/avr-nm
AVRDUDE = $(AVRDUDE_PATH)/avrdude
REMOVE = rm -f
MV = mv -f

OBJ_DEPS=$(ARDUINO)/HardwareSerial.o $(ARDUINO)/Print.o TempHumSensor/TempHumSensor.o PIRSensor/PIRSensor.o  $(ARDUINO)/wiring_digital.o

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)
OBJ_MODULES = $(C_MODULES:.c=.o) $(CXX_MODULES:.cpp=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = $(CFLAGS) -mmcu=$(MCU)
ALL_CXXFLAGS = $(CXXFLAGS) -mmcu=$(MCU)
ALL_ASFLAGS = -x assembler-with-cpp $(ASFLAGS) -mmcu=$(MCU)
ALL_LDFLAGS = $(LDFLAGS) -mmcu=$(MCU)

# Default target.
all: applet_files build sizeafter

build: elf hex

applet/main.o: $(ARDUINO)/main.cpp
	test -d applet || mkdir applet
	$(CXX) -c $(ALL_CXXFLAGS) $(ARDUINO)/main.cpp  -o applet/main.o

applet/friendlyhal.o: friendlyhal.cpp
	$(CXX) -c $(ALL_C/XXFLAGS) friendlyhal.cpp -o applet/friendlyhal.o -DUBRRH $(CXXINCS) $(CXXDEFS)

elf: applet/main.elf
hex: applet/main.hex
eep: applet/main.eep
lss: applet/main.lss
sym: applet/main.sym

# Program the device.
upload: applet/main.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) applet/main.hex
ELFSIZE = $(SIZE)  applet/main.elf
sizebefore:
	@if [ -f applet/main.elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:
	@if [ -f applet/main.elf ]; then echo; echo $(MSG_SIZE_AFTER); $(HEXSIZE); echo; fi


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: applet/main.elf
	$(COFFCONVERT) -O coff-avr applet/main.elf main.cof


extcoff: main.elf
	$(COFFCONVERT) -O coff-ext-avr applet/main.elf main.cof


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	$(OBJCOPY) -O $(FORMAT) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--no-change-warnings \
	--change-section-lma .eeprom=0 $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

# Link: create ELF output file from library.

applet/main.elf: applet/main.o applet/core.a applet/friendlyhal.o $(OBJ_DEPS)
	$(LD) $(ALL_LDFLAGS) -o $@ applet/main.o applet/core.a applet/friendlyhal.o $(OBJ_DEPS)

applet/core.a: $(OBJ_MODULES)
	@for i in $(OBJ_MODULES); do echo $(AR) rcs applet/core.a $$i; $(AR) rcs applet/core.a $$i; done


# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Automatic dependencies
%.d: %.c
	$(CC) -M $(ALL_CFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@

%.d: %.cpp
	$(CXX) -M $(ALL_CXXFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@


# Target: clean project.
clean:
	$(REMOVE) applet/* \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)
	rmdir applet

.PHONY:	all build elf hex eep lss sym program coff extcoff clean applet_files sizebefore sizeafter

#include $(SRC:.c=.d)
#include $(CXXSRC:.cpp=.d)
