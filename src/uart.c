/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 *
 * @file uart.c
 * @author Jon Sowman
 * @addtogroup UART
 * @{
 */

#include <string.h>
#include "uart.h"

/**
 * Set up the UCSI for UART operation at 9600 baud. The baud rate
 * generator is currently hard coded for 25MHz SMCLK.
 * @todo Fix this to calculate BRR values from F_CPU.
 */
void uart_init(void)
{
    P4SEL |= (1 << 4) | (1 << 5);

    // Make sure the USCI is in reset state
    UCA1CTL1 |= UCSWRST;

    // Clock the USCI from SMCLK
    UCA1CTL1 |= UCSSEL_2;

    // Baud rate should be 9600 at 25MHz (see datasheet for these)
    // UCOS16 = 0
    UCA1BR0 = 2603 & 0xFF;
    UCA1BR1 = 2603 >> 8;
    UCA1MCTL = UCBRS_2;

    // Finally, release the USCI reset logic to enable the peripheral
    UCA1CTL1 &= ~UCSWRST;

    // Enable interrupts from the UCSI
    UCA1IE |= UCRXIE;
}

/**
 * Takes a char pointer, transmit data over the UART until we find a 
 * null character.
 * @param string A char pointer to the string to transmit
 * @todo This should time out instead of busy-wait forever.
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
 * Send a CRLF terminated string to the debug output (to avoid storing
 * the terminators in RAM all the time).
 * @param string A char pointer to the string to transmit.
 */
void uart_debug(char* string)
{
    if(strlen(string) >= UART_BUF_LEN)
        uart_debug("[WARN] UART BUF_OVF");
    _uart_tx(string);
    _uart_tx("\r\n");
}

/**
 * @}
 */
