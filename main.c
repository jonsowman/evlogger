/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include <msp430.h>
#include <msp430f5529.h>
#include <stdint.h>
#include <stdio.h>

#include "typedefs.h"
#include "uart.h"

#define _BV(x) (1<<x)

void sys_clock_init(void);

int main( void )
{
    // Stop the wdt
    WDTCTL = WDTPW | WDTHOLD;

    sys_clock_init();

    P1DIR |= _BV(0);
    P1DIR |= _BV(1);
    
    while(1)
    {
        // Check and show any oscillator faults
        if(UCSCTL7 & XT1LFOFFG)
        {
            SFRIFG1 &= ~OFIFG;
            P1OUT |= _BV(1);
        } else {
            P1OUT &= ~_BV(1);
        }

        P1OUT ^= _BV(0);
        __delay_cycles(160000);
        __delay_cycles(160000);
    }
    return 0;
}

/**
 * Set up the system clock to use the external crystal as the stabilisation
 * source for the FLL.
 */
void sys_clock_init( void )
{
    uint16_t i;

    // Port select XT2
    P5SEL |= (1 << 2) | (1 << 3);

    // Enable XT2 (4MHz xtal attached to XT2) and disable XT1 (LF & HF)
    UCSCTL6 &= ~XT2OFF;
    UCSCTL6 |= XT1OFF;
    _BIS_SR(OSCOFF); // Disable LFXT1

    // Wait for XT2 to stabilise
    do {
        UCSCTL7 &= ~XT2OFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & XT2OFFG );

    // Set FLL reference to be XT2 divided by 4
    UCSCTL3 = SELREF__XT2CLK | FLLREFDIV__4;

    // Set the FLL loop divider to 16 and the multiplier to 1
    // DCOCLK = D * (N+1) * (FLLREFCLK / FLLREFDIV)
    UCSCTL2 = 0x0000;
    UCSCTL2 |= FLLD__8; // compensate for N=0 disallowed

    // Set the DCO to range 4 (1.3 - 28.2MHz, target 16MHz)
    UCSCTL1 = DCORSEL_4;

    // Wait until the DCO has stabilised
    do {
        UCSCTL7 &= ~DCOFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & DCOFFG );

    // At this point, DCOCLK is a 16MHz stabilised reference
    // So set MCLK to use this
    UCSCTL4 = SELS_3 | SELM_3;
}
