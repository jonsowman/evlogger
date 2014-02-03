/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "logger.h"
#include "adc.h"

volatile uint8_t logger_running;

/**
 * Set up the hardware for logging functionality
 */
void logger_init(void)
{
    // Enable LEDs and turn them off (P1.0, P8.1, P8.2)
    P1DIR |= _BV(0);
    P1OUT &= ~_BV(0);
    P8DIR |= _BV(1);
    P8OUT &= ~_BV(1);
    P8DIR |= _BV(2);
    P8OUT &= ~_BV(2);

    // Select the potentiometer and enable the ADC on that channel
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);
    P6SEL |= _BV(5);
    adc_select(0x05);

    // Enable the buttons
    // S1 is on P1.7, S2 is on P2.2
    S1_PORT_OUT |= S1_PIN;
    S1_PORT_REN |= S1_PIN;
    S2_PORT_OUT |= S2_PIN;
    S2_PORT_REN |= S2_PIN;
    // Trigger on falling edge, clear int flags, enable interupts
    S1_PORT_IES &= ~S1_PIN;
    S1_PORT_IFG &= ~S1_PIN;
    S1_PORT_IE |= S1_PIN;
    S2_PORT_IES &= ~S2_PIN;
    S2_PORT_IFG &= ~S2_PIN;
    S2_PORT_IE |= S2_PIN;

    // Set up 16 bit timer TIMER1 to interrupt at the log frequency
    TA1CCR0 = 19999;

    // Clock from SMCLK with no divider, use "up" mode, use interrupts
    TA1CTL |= TASSEL_2 | TACLR;

    // Enable interrupts on CCR0
    TA1CCTL0 |= CCIE;

    logger_enable();

    // Enable interrupts (if they're not already)
    eint();
}

/**
 * Enable TA1 to begin logging by setting mode control to "up" mode,
 * counter counts to TAxCCR0.
 */
void logger_enable(void)
{
    // Clear bits 4 and 5
    TA1CTL &= ~MC_3;
    TA1CTL |= MC_1;
    P1OUT |= _BV(0);
    logger_running = 1;
}

/**
 * Disable TA1 to halt logging by setting mode control to STOP.
 */
void logger_disable(void)
{
    // Clear bits 4 and 5
    TA1CTL &= ~MC_3;
    P1OUT &= ~_BV(0);
    logger_running = 0;
}

/**
 * Interrupt for TA1, here we should log one block of data to the SD card.
 * In future we may buffer data and log it in blocks, or transfer it to the
 * SD card via DMA.
 */
interrupt(TIMER1_A0_VECTOR) TIMER1_A0_ISR(void)
{
    P8OUT ^= _BV(1);
}

/**
 * Interrupt vector for button S1
 */
interrupt(PORT1_VECTOR) PORT1_ISR(void)
{
    if(P1IV & P1IV_P1IFG7)
    {
        if(logger_running)
            logger_disable();
        else
            logger_enable();
    }

}

/**
 * Interrupt vector for button S2
 */
interrupt(PORT2_VECTOR) PORT2_ISR(void)
{
    if(P2IV & P2IV_P2IFG2)
        ; // Do something here
}
