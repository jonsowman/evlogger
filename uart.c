/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "uart.h"

/**
 * Set up the UCSI for UART operation at 9600 baud. The baud rate
 * generator is currently hard coded for 20MHz SMCLK.
 * TODO: Fix this to calculate BRR values from F_CPU.
 */
void uart_init(void)
{
    P4SEL |= (1 << 4) | (1 << 5);

    // Make sure the USCI is in reset state
    UCA1CTL1 |= UCSWRST;

    // Clock the USCI from SMCLK
    UCA1CTL1 |= UCSSEL_2;

    // Baud rate should be 9600 at 20MHz (see datasheet for these)
    // UCOS16 = 0
    UCA1BR0 = 2083 & 0xFF;
    UCA1BR1 = 2083 >> 8;
    UCA1MCTL = UCBRS_2;

    // Finally, release the USCI reset logic to enable the peripheral
    UCA1CTL1 &= ~UCSWRST;

    // Enable interrupts from the UCSI
    UCA1IE |= UCRXIE;
}

/**
 * Takes a char pointer, transmit until we find a null character
 * TODO: This should time out instead of busy-wait forever.
 * \param string A char pointer to the string to transmit
 */
static void _uart_tx(char* string)
{
    while(*string)
    {
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *string++;
    }
}

/**
 * Send a \r\n terminated string to the debug output (to avoid storing
 * the terminators in RAM all the time).
 * \param string A char pointer to the string to transmit.
 */
void uart_debug(char* string)
{
    _uart_tx(string);
    _uart_tx("\r\n");
}

/**
 * Initialise UCB1 for the SD card SPI unit.
 */
void sd_init(void)
{
    // Put UCSI state machine in reset
    UCB1CTL1 |= UCSWRST;

    // Set SD CS line to output, and SPI lines to alt. digital fn
    P3DIR |= _BV(7);
    P4SEL |= _BV(1) | _BV(2) | _BV(3);

    // MSB first, be a master
    UCB1CTL0 |= UCMSB | UCMST;

    // Clock from SMCLK
    UCB1CTL1 |= UCSSEL__SMCLK;

    // Release the USCI reset logic
    UCB1CTL1 &= ~UCSWRST;
}
