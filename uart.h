/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#ifndef __UART_H__
#define __UART_H__

#include <msp430.h>
#include "typedefs.h"

#ifndef F_CPU
    #error "F_CPU not defined"
#endif

void uart_init(void);
static void _uart_tx(char* string);
void uart_debug(char* string);

#endif /* __UART_H__ */
