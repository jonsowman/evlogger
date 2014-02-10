/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdio.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <legacymsp430.h>
#include "typedefs.h"

void clock_init(void);
void sys_clock_init(void);
uint32_t clock_time(void);
void _delay_ms(uint32_t delay);

#endif /* __CLOCK_H__ */
