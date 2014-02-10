#
# Makefile for msp430 (based on mspgcc example)
#
# Jon Sowman 2013
# <jon@jonsowman.com>
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = evlogger_bin
MCU        = msp430f5529
PROGRAMMER = rf2500
# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = main.c uart.c adc.c system.c ff.c HAL_Dogs102x6.c HAL_SDCard.c mmc.c logger.c
# Include are located in the Include directory
INCLUDES = -isystem /usr/msp430/include/

#######################################################################################
CFLAGS   = -mmcu=$(MCU) -DF_CPU=20000000 -g -Os -Wall -Wunused $(INCLUDES)   
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
MSPDEBUG = mspdebug
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
########################################################################################
# the file which will include dependencies
DEPEND = $(SOURCES:.c=.d)
# all the object files
OBJECTS = $(SOURCES:.c=.o)
all: $(TARGET).elf $(TARGET).hex $(TARGET).txt 
$(TARGET).elf: $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo
%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
%.txt: %.hex
	$(MAKETXT) -O $@ -TITXT $< -I
	unix2dos $(TARGET).txt
#  The above line is required for the DOS based TI BSL tool to be able to read the txt file generated from linux/unix systems.
%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<
# rule for making assembler source listing, to see the code
%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@
# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif
# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -M ${CFLAGS} $< >$@
.SILENT:
.PHONY:	clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET).*
	-$(RM) $(SOURCES.c=.lst)
	-$(RM) $(DEPEND)

flash:  all
	$(MSPDEBUG) $(PROGRAMMER) --force-reset "prog $(TARGET).elf"
