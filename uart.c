/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "uart.h"

/**
 * Set up the UCSI for UART operation at 115200 baud. The baud rate
 * generator is currently hard coded for 20MHz SMCLK.
 * TODO: Fix this to calculate BRR values from F_CPU.
 */
void uart_init(void)
{
    P3SEL |= (1 << 3) | (1 << 4);

    // Make sure the USCI is in reset state
    UCA1CTL1 |= UCSWRST;

    // Clock the USCI from SMCLK
    UCA1CTL1 |= UCSSEL_2;

    // Baud rate should be 115200 at 20MHz, register values are:
    // UCOS16 = 1
    // UCBRx = 10; UCBRSx = 0; UCBRFx = 14
    UCA1BR0 = 10;
    UCA1BR1 = 0;
    UCA1MCTL = UCOS16 | (0x0E << 4);

    // Finally, release the USCI reset logic to enable the peripheral
    UCA1CTL1 &= ~UCSWRST;
}

/**
 * Takes a char pointer, transmit until we find a null character
 * TODO: This should time out instead of busy-wait forever.
 * \param string A char pointer to the string to transmit
 */
void uart_tx(char* string)
{
    while(*string)
    {
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *string++;
    }
}
