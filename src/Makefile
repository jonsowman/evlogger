# Makefile for msp430 (based on mspgcc example)
#
# Jon Sowman 2014
# <jon@jonsowman.com>
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# 'make flash' builds everything (if not already built) and flashes to target
#
# You need to set TARGET, MCU & PROGRAMMER for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt and $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = evlogger_bin
MCU        = msp430f5529
PROGRAMMER = rf2500

# Include and build directories
INCDIR = ../inc/HAL
OBJDIR = build

# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = $(wildcard *.c) $(wildcard ${INCDIR}/*.c) 
# Include are located in the Include directory
INCLUDES = -isystem /usr/msp430/include/

#######################################################################################
CFLAGS   = -mmcu=$(MCU) -I${INCDIR} -DF_CPU=25000000 -g -Os -Wall -Wunused $(INCLUDES)   
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=${OBJDIR}/$(TARGET).map
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

# look in the build directory for pre-reqs
vpath %.o ${OBJDIR}
vpath %.c ${INCDIR}
vpath %.h ${INCDIR}

# all the object files
OBJECTS = $(SOURCES:.c=.o)

all: ${OBJDIR}/$(TARGET).elf ${OBJDIR}/${TARGET}.hex ${OBJDIR}/${TARGET}.txt

${OBJDIR}/$(TARGET).elf: $(addprefix ${OBJDIR}/, ${notdir $(DEPEND)}) $(addprefix ${OBJDIR}/, ${notdir $(OBJECTS)})
	echo "Linking $@"
	$(CC) $(addprefix ${OBJDIR}/, ${notdir $(OBJECTS)}) $(LDFLAGS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) ${OBJDIR}/$(TARGET).elf
	echo

${OBJDIR}/${TARGET}.hex: ${OBJDIR}/${TARGET}.elf
	$(OBJCOPY) -O ihex $< $@

${OBJDIR}/${TARGET}.txt: ${OBJDIR}/${TARGET}.hex
	$(MAKETXT) -O $@ -TITXT $< -I
	unix2dos ${OBJDIR}/$(TARGET).txt
#  The above line is required for the DOS based TI BSL tool to be able to read the txt file generated from linux/unix systems.

${OBJDIR}/%.o: %.c
	echo "Compiling $< to $@"
	$(CC) -c $(CFLAGS) -o $@ $<

# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(addprefix ${OBJDIR}/, ${notdir $(DEPEND)})
endif

# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
${OBJDIR}/%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -MT '$(basename $@).o' -MM ${CFLAGS} $< > $@

.SILENT:
.PHONY:	clean
clean:
	-$(RM) ${OBJDIR}/*

flash:  all
	$(MSPDEBUG) $(PROGRAMMER) --force-reset "prog ${OBJDIR}/$(TARGET).elf"
