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

#define SPI_SIMO        BIT1
#define SPI_SOMI        BIT2
#define SPI_CLK         BIT3
#define SD_CS           BIT7

#define SPI_SEL         P4SEL
#define SPI_DIR         P4DIR
#define SPI_OUT         P4OUT
#define SPI_REN         P4REN
#define SD_CS_SEL       P3SEL
#define SD_CS_OUT       P3OUT
#define SD_CS_DIR       P3DIR

// Length of the char buffer for debugging over UART
#define UART_BUF_LEN 50

void uart_init(void);
void uart_debug(char* string);

void sd_init(void);

#endif /* __UART_H__ */
