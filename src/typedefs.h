/**
 * Standard data type shorthand defined as with avr-libc.
 * long long is unsupported since we probably shouldn't be using it anyway.
 *
 * @file typedefs.h
 * @author Jon Sowman, University of Southampton <j.sowman@soton.ac.uk>
 * @copyright Jon Sowman 2014, All Rights Reserved
 */

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <msp430.h>

/**
 * This is shorthand from avr-libc
 * @param x Shift 1 left by x bits
 */
#define _BV(x) (1<<x)

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#endif /* __TYPEDEFS_H__ */
