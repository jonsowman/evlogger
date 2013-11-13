/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <msp430.h>
#include <msp430f5529.h>
#include "typedefs.h"

void adc_init(void);
uint16_t adc_convert(void);
void adc_select(const uint8_t channel);

#endif /* __ADC_H__ */
