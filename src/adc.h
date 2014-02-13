/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <msp430.h>
#include <msp430f5529.h>
#include "typedefs.h"
#include "logger.h"

void adc_init(SampleBuffer *sb);
void adc_convert(void);

#endif /* __ADC_H__ */
