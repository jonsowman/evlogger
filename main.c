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
#include "adc.h"

#define _BV(x) (1<<x)

void sys_clock_init(void);

int main( void )
{
    uint16_t adc_read;
    char s[20];

    // Stop the wdt
    WDTCTL = WDTPW | WDTHOLD;

    // Set up the system clock and any required peripherals
    sys_clock_init();
    uart_init();
    adc_init();

    P1DIR |= _BV(0);
    P1DIR |= _BV(1);

    // Turn P8.0 on as the power supply for the pot
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);

    // Read from ADC channel 5 (A5)
    adc_select(5);

    while(1)
    {
        adc_read = adc_convert();
        sprintf(s, "%u", adc_read);
        uart_debug(s);
        P1OUT ^= _BV(0);
        __delay_cycles(160000);
        __delay_cycles(160000);
        __delay_cycles(160000);
        __delay_cycles(160000);
    }
    return 0;
}

/**
 * Set up the system clock to use the external crystal as the stabilisation
 * source for the FLL and have MCLK at 20MHz.
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

    // Set FLL reference to be XT2 divided by 2 (FLLREFDIV=2)
    UCSCTL3 = SELREF__XT2CLK | FLLREFDIV__2;

    // Set the FLL loop divider to 4 (D=4) and the multiplier to 5 (N=4)
    // DCOCLK = D * (N+1) * (FLLREFCLK / FLLREFDIV)
    UCSCTL2 = 0x0004;
    UCSCTL2 |= FLLD__4; // compensate for N=0 disallowed

    // Set the DCO to range 4 (1.3 - 28.2MHz, target 20MHz)
    UCSCTL1 = DCORSEL_4;

    // Wait until the DCO has stabilised
    do {
        UCSCTL7 &= ~DCOFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & DCOFFG );

    // At this point, DCOCLK is a 20MHz stabilised reference
    // So set MCLK and SMCLK to use this
    UCSCTL4 = SELS_3 | SELM_3;
}
