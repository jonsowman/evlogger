/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "delay.h"

/**
 * Delay for the provided number of milliseconds
 * \param delay The number of ms to delay.
 */ 
void _delay_ms(uint32_t delay)
{
    uint32_t i;
    for(i=0; i < delay; i++)
    {
        __delay_cycles(20000);
    }
}

