/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <legacymsp430.h>
#include "typedefs.h"

#define S1_PORT_OUT P1OUT
#define S1_PORT_REN P1REN
#define S1_PORT_IES P1IES
#define S1_PORT_IE P1IE
#define S1_PORT_IFG P1IFG
#define S1_PIN _BV(7)

#define S2_PORT_OUT P2OUT
#define S2_PORT_REN P2REN
#define S2_PORT_IES P2IES
#define S2_PORT_IE P2IE
#define S2_PORT_IFG P2IFG
#define S2_PIN _BV(2)

void logger_init(void);
void sd_setup(void);
void logger_enable(void);
void logger_disable(void);

#endif /* __LOGGER_H__ */
