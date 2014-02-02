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

#define SDCS_PORT P3OUT
#define SDCS_PIN _BV(7)

// Length of the char buffer for debugging over UART
#define UART_BUF_LEN 40

void uart_init(void);
void uart_debug(char* string);

void sd_init(void);

#endif /* __UART_H__ */
