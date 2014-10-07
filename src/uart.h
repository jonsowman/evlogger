/**
 * UART header.
 *
 * @file uart.h
 * @author Jon Sowman, University of Southampton <j.sowman@soton.ac.uk>
 * @copyright Jon Sowman 2014, All Rights Reserved
 * @addtogroup UART
 * @{
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

/**
 * Length of the temporary buffer we will use for containing strings to be
 * written out over the UART. This should be set to at least the maximum length
 * of any string to be transmitted, doing otherwise will cause a buffer
 * overflow and undefined behaviour, even system crashes.
 */
#define UART_BUF_LEN 50

void uart_init(void);
void uart_debug(char* string);

#endif /* __UART_H__ */

/**
 * @}
 */
