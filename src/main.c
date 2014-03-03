/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 *
 * @file main.c
 * @author Jon Sowman
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
