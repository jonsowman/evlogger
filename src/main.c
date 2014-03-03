/**
 * \mainpage Electric Vehicle Datalogger
 *
 * \section Introduction
 * This project uses an MSP430 (specifically, the MSP430F5529) to log data from
 * various channels to be used on an electric vehicle. This project is designed
 * for use on the MSP-EXP430F5529LP experimentation board which is available
 * from Texas Instruments.
 * 
 * \section Specification
 * There are six analogue channels broken out on the development board, plus a
 * user potentiometer. Additionally, there is a CMA3000 3-axis accelerometer
 * which is capable of running at up to 400Hz. The analogue channels (including
 * the pot) are sampled and logged at 1kHz. The accelerometer is also logged at
 * this frequency though analysis of the output will confirm that its value
 * only changes at 400Hz.
 *
 * \section Software Architecture
 * The software documented here is written specifically for the project, but
 * libraries and hardware abstraction layer utilities are used from various
 * sources which are documentented in the code. Licenses for these parts of the
 * code are respected and as such, copyright notices will be found in some
 * source files where they were used as the basis for the code.
 *
 * \section Authorship
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
