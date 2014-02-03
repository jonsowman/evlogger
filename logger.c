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

    // Enable the buttons
    // S1 is on P1.7, S2 is on P2.2
    S1_PORT_OUT |= S1_PIN;
    S1_PORT_REN |= S1_PIN;
    // Trigger on falling edge, clear int flags, enable interupts
    S1_PORT_IES &= ~S1_PIN;
    S1_PORT_IFG &= ~S1_PIN;
    S1_PORT_IE |= S1_PIN;
}

/**
 * Interrupt vector for button 1
 */
interrupt(PORT1_VECTOR) PORT1_ISR(void)
{
    if(P1IV & P1IV_P1IFG7)
        P1OUT ^= _BV(0);
}
