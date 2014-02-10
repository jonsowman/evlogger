/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "HAL_PMM.h"
#include "system.h"

volatile uint32_t ticks;

/**
 * Use timer A0 to set up a system clock ticking at 1ms intervals.
 */
void clock_init(void)
{
    // Reset the local tick counter and set function ptrs to null
    ticks = 0;

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
 * source for the FLL and have MCLK at 25MHz.
 */
void sys_clock_init( void )
{
    uint16_t i;

    SetVCore(3);

    // Port select XT2
    P5SEL |= (1 << 2) | (1 << 3);

    __bis_status_register(SCG0);

    // Enable XT2 (4MHz xtal attached to XT2) and disable XT1 (LF & HF)
    UCSCTL6 &= ~XT2OFF;
    UCSCTL6 |= XT1OFF;
    _BIS_SR(OSCOFF); // Disable LFXT1

    // Wait for XT2 to stabilise
    do {
        UCSCTL7 &= ~XT2OFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & XT2OFFG );

    // Set DCO to lowest tap
    UCSCTL0 = 0x0000;

    // Set FLL reference to be XT2 divided by 2 (FLLREFDIV=2)
    UCSCTL3 = SELREF__XT2CLK | FLLREFDIV__4;

    // Set the FLL loop divider to 4 (D=4) and the multiplier to 5 (N=4)
    // DCOCLK = D * (N+1) * (FLLREFCLK / FLLREFDIV)
    // See footnote, p.61, F5529 specific datasheet
    UCSCTL2 &= ~(0x03FF);
    UCSCTL2 = 24;
    UCSCTL2 |= FLLD__1; // compensate for N=0 disallowed

    // Set the DCO to range 6 (10.7 - 39.0MHz, target 25MHz)
    // NB: f_dco_max(n, 0) < f_target < f_dco_min(n, 31)
    UCSCTL1 = DCORSEL_6;

    __bic_status_register(SCG0);
    // Wait until the DCO has stabilised
    do {
        UCSCTL7 &= ~DCOFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & DCOFFG );

    // At this point, DCOCLK is a 25MHz stabilised reference
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
 * Delay for the provided number of milliseconds
 * \param delay The number of ms to delay.
 */ 
void _delay_ms(uint32_t delay)
{
    uint32_t i;
    for(i=0; i < delay; i++)
    {
        __delay_cycles(25000);
    }
}

/**
 * Interrupt service routine for the system ticks counter.
 * the interrupt() macro is from legacymsp430.h
 */
interrupt(TIMER0_A0_VECTOR) TIMER0_A0_ISR(void)
{
    ticks++;
}
