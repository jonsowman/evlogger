/**
 * \mainpage Electric Vehicle Datalogger
 *
 * \section intro Introduction
 * This project uses an MSP430 (specifically, the MSP430F5529) to log data from
 * various channels to be used on an electric vehicle. This project is designed
 * for use on the MSP-EXP430F5529LP experimentation board which is available
 * from Texas Instruments.
 * 
 * \section spec Specification
 * There are six analogue channels broken out on the development board, plus a
 * user potentiometer. Additionally, there is a CMA3000 3-axis accelerometer
 * which is capable of running at up to 400Hz. The analogue channels (including
 * the pot) are sampled and logged at 1kHz. The accelerometer is also logged at
 * this frequency though analysis of the output will confirm that its value
 * only changes at 400Hz.
 *
 * \section software Software Architecture
 * The software documented here is written specifically for the project, but
 * libraries and hardware abstraction layer utilities are used from various
 * sources which are documentented in the code. Licenses for these parts of the
 * code are respected and as such, copyright notices will be found in some
 * source files where they were used as the basis for the code.
 *
 * The main functionality for the datalogger is in the Logger module. The
 * overall architecture overview is that we set up a timer to interrupt at the
 * log frequency, which collects available data and puts it into a buffer ready
 * to be transferred to the SD card. It then invalidates the data and triggers
 * another conversion run such that on the next interrupt, the new data will be
 * ready.
 *
 * A software controlled RingBuffer is used to store data before it is
 * transferred to the SD card, and a full ring buffer implementation can be
 * found in the Logger module. This is generic and can be used in other
 * projects. The size of this buffer is controlled by SD_RINGBUF_LEN, and
 * should be as large as possible for best performance but should never be
 * smaller than the sector size (usually 512 bytes for FAT16).
 *
 * The peripherals are controlled by separate modules, see ADC, Accelerometer,
 * UART particularly. Documentation for how these are configured can be found
 * in the relevant source files; here it suffices to note that CPU time is
 * minimised by use of DMA in the case of the ADC and an interrupt controlled
 * finite state machine (FSM) in the case of the accelerometer. The UART is
 * principally for debugging purposes and is not set up for speed (it currently
 * busy-waits during transmits) and as such, should not be used in production
 * runs of the firmware builds.
 *
 * The onboard peripherals, perticularly the CPU core clock and system wall
 * clock timer are controlled by the System module, relevant documentation is
 * contained within.
 *
 * \section build Building the Firmware
 * A Makefile is provided to build the firmware using the GNU mspgcc toolchain.
 * Builds of this toolchain are available for Windows, OS X and Linux though on
 * Windows you will need to set an IDE up as appropriate since the Makefile
 * will not work (unless you use cygwin). To build the firmware using the
 * Makefile please change to the source directory and run $ make. Flashing the
 * device is done using mspdebug over the Spy-by-Wire interface, and can be
 * enacted by running $ make flash.  Cleaning the build directory (deleting all
 * object files, dependency list files and binaries) can be done using $ make
 * clean.
 *
 * \section author Authorship
 * Jon Sowman, University of Southampton <js39g13@soton.ac.uk>. Please get in
 * touch with any questions or comments.
 *
 * @file main.c
 * @author Jon Sowman, University of Southampton <js39g13@soton.ac.uk>
 * @copyright Jon Sowman 2014, All Rights Reserved
 * @addtogroup main
 * @{
 */

#include <msp430.h>
#include <msp430f5529.h>
#include <stdint.h>
#include <stdio.h>

#include "HAL_Dogs102x6.h"
#include "typedefs.h"
#include "uart.h"
#include "adc.h"
#include "system.h"
#include "logger.h"

#include "HAL_SDCard.h"
#include "ff.h"

/**
 * Kill the watchdog timer to prevent it firing, then set up the system clock
 * followed by onboard peripherals. Wait for all systems to be ready before
 * starting the datalogger.
 */
int main(void)
{
    // Stop the wdt
    WDTCTL = WDTPW | WDTHOLD;

    // Set up the system clock and any required peripherals
    sys_clock_init();
    clock_init();
    uart_init();
    Dogs102x6_init();
    Dogs102x6_backlightInit();

    // Wait for peripherals to boot
    _delay_ms(100);
 
    // Test that minicom/term is behaving
    uart_debug("Hello world");

    // Test the LCD
    Dogs102x6_setBacklight(1);
    Dogs102x6_setContrast(6);
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "=== EV LOGGER ===", DOGS102x6_DRAW_INVERT);

    // Wait for periphs to boot and start logging
    logger_init();
     
    // We should never get to this point
    while(1);

    return 0;
}

/**
 * @}
 */
