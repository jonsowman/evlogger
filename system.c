/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "system.h"

volatile uint32_t ticks;

/**
 * Use timer A0 to set up a system clock ticking at 1ms intervals.
 */
void clock_init(void)
{
    // Reset the local tick counter and set function ptrs to null
    ticks = 0;
    fn_10ms = fn_100ms = fn_1s = NULL;

    // Count to 19999 (20000 actual counts)
    TA0CCR0 = 19999;

    // Clock from SMCLK with no divider, use "up" mode, use interrupts
    TA0CTL |= TASSEL_2 | MC_1 | TACLR;

    // CCR0 interrupt enable
    TA0CCTL0 |= CCIE;

    // Enable global interrupts (macro from legacymsp430.h) and return
    eint();
    return;
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

/**
 * Return the current system time
 */
uint32_t clock_time(void)
{
    return ticks;
}

/**
 * Interrupt service routine for the system ticks counter.
 * the interrupt() macro is from legacymsp430.h
 */
interrupt(TIMER0_A0_VECTOR) TIMER0_A0_ISR(void)
{
    ticks++;
}
