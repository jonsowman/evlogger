/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 *
 * Standard data type shorthand defined as with avr-libc.
 * long long is unsupported since we probably shouldn't be using it anyway.
 *
 */

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <msp430.h>

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

#endif /* __TYPEDEFS_H__ */
