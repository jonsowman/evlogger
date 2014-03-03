/**
 * System header.
 *
 * @file system.h
 * @author Jon Sowman, University of Southampton <js39g13@soton.ac.uk>
 * @copyright Jon Sowman 2014, All Rights Reserved
 * @addtogroup System
 * @{
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <legacymsp430.h>
#include "typedefs.h"

/**
 * Define a type to hold a system clock time
 */
typedef uint32_t clock_time_t;

void clock_init(void);
void sys_clock_init(void);
clock_time_t clock_time(void);
void _delay_ms(uint32_t delay);

#endif /* __SYSTEM_H__ */

/**
 * @}
 */
