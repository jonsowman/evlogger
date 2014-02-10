/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "system.h"

volatile uint32_t ticks;

// We can register a function to be called at each of 10ms, 100ms, and 1s
void (*fn_10ms)(void);
void (*fn_100ms)(void);
void (*fn_1s)(void);

/**
 * Use timer A0 to set up a system clock ticking at 1ms intervals.
 * We will register functions to be called at 1ms, 10ms and 100ms intervals.
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
