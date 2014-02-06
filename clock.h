/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
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
uint32_t clock_time(void);
void register_function_10ms(void (*function_10ms)(void));
void register_function_100ms(void (*function_100ms)(void));
void register_function_1s(void (*function_1s)(void));

#endif /* __CLOCK_H__ */
