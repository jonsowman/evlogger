/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "logger.h"
#include "adc.h"

/**
 * Set up the hardware for logging functionality
 */
void logger_init(void)
{
    // Enable LED on P1.0 and turn it off
    P1DIR |= _BV(0);
    P1OUT &= ~_BV(0);

    // Select the potentiometer and enable the ADC on that channel
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);
    P6SEL |= _BV(5);
    adc_select(0x05);
}
