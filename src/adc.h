/**
 * @file adc.h
 * @author Jon Sowman, University of Southampton <j.sowman@soton.ac.uk>
 * @copyright Jon Sowman 2014, All Rights Reserved
 * @addtogroup ADC
 * @{
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <msp430.h>
#include <msp430f5529.h>
#include "typedefs.h"
#include "logger.h"

void adc_init(volatile SampleBuffer *sb);
void adc_convert(void);

#endif /* __ADC_H__ */

/**
 * @}
 */
