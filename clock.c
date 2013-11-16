/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "clock.h"

#define _BV(x) (1<<x)

volatile uint32_t ticks;

/**
 * Use timer A0 to set up a system clock ticking at 1ms intervals.
 * We will register functions to be called at 1ms, 10ms and 100ms intervals.
 */
void clock_init(void)
{
    // Reset the local tick counter
    ticks = 0;

    // Count to 19999 (20000 actual counts)
    TA0CCR0 = 19999;

    // Clock from SMCLK with no divider, use "up" mode, use interrupts
    TA0CTL |= TASSEL_2 | MC_1 | TACLR;

    // CCR0 interrupt enable
    TA0CCTL0 |= CCIE;

    // Enable global interrupts (macro from legacymsp430.h)
    eint();

    return;
}

/**
 * Interrupt service routine for the system ticks counter.
 * the interrupt() macro is from legacymsp430.h
 */
interrupt(TIMER0_A0_VECTOR) TIMER0_A0_ISR(void)
{
    ticks++;
    if(ticks % 1000UL == 0) P1OUT ^= _BV(0);
}
