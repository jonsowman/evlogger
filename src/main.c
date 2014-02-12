/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
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

char s[25];

int main(void)
{
    uint16_t i;

    // Stop the wdt
    WDTCTL = WDTPW | WDTHOLD;

    // Set up the system clock and any required peripherals
    sys_clock_init();
    clock_init();
    uart_init();
    adc_init();
    Dogs102x6_init();
    Dogs102x6_backlightInit();

    // Enable LED on P1.0 and turn it off
    P1DIR |= _BV(0);
    P1OUT &= ~_BV(0);

    // Select the potentiometer and enable the ADC on that channel
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);
    P6SEL |= _BV(5);
    adc_select(0x05);

    // Wait for peripherals to boot
    _delay_ms(100);
 
    // Test that minicom/term is behaving
    uart_debug("Hello world");

    // Test the LCD
    Dogs102x6_setBacklight(1);
    Dogs102x6_setContrast(6);
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "=== EV LOGGER ===", DOGS102x6_DRAW_INVERT);

    while(1)
    {
        i = adc_convert();
        sprintf(s, "adc=%u", i);
        uart_debug(s);
        _delay_ms(100);
    }
   
    // Wait for periphs to boot and start logging
    logger_init();
     
    while(1);

    return 0;
}

