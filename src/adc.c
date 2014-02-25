/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include <inttypes.h>
#include "adc.h"
#include "system.h"

/**
 * Set up the ADC clock and configure resolution, then enable the ADC
 * unit.
 */
void adc_init(SampleBuffer *sb)
{
    uint8_t i;

    // Clear the ADC sample buffer
    for(i = 0; i < ADC_CHANNELS; i++)
        sb->adc[i] = 0;

    // Be sure that conversions are disabled
    ADC12CTL0 &= ~ADC12ENC;

    // Need 4us conversion time for 12 bit resolution and 10kR source
    // resistance. This is 20 cycles at 5MHz ADC12CLK, round up to 32
    // Enable ADC12 and use multiple-sample-conversion mode
    ADC12CTL0 |= ADC12ON | ADC12MSC | ADC12SHT0_3 | ADC12SHT1_3;

    // If using external reference or AVCC then we can have 5MHz maximum
    // ADC12CLK, so divide by 5 to get 5MHz from 25MHz SMCLK
    // Use the sampling timer (SHP) and sequential conversion mode
    ADC12CTL1 |= ADC12DIV_4 | ADC12SSEL_3 | ADC12SHP | ADC12CONSEQ_1;

    // A6, A7, A12, A13, A14, A15 are broken out, set these as sources for
    // ADC12MEM0-5 with AVCC as +ve and AVSS as -ve
    ADC12MCTL0 = ADC12INCH_6;
    ADC12MCTL1 = ADC12INCH_7;
    ADC12MCTL2 = ADC12INCH_12;
    ADC12MCTL3 = ADC12INCH_13;
    ADC12MCTL4 = ADC12INCH_14;
    ADC12MCTL5 = ADC12INCH_15;
    // Set end of sequence (EOS) for final channel
    ADC12MCTL6 = ADC12INCH_5 | ADC12EOS;

    // Enable conversions last, can't modify ADC12ON whilst ADC12ENC=1
    ADC12CTL0 |= ADC12ENC;

    // Now set up DMA to transfer ADC readings to the ADC buffer
    // Set DMA channel 0 trigger to ADC12IFGx
    DMACTL0 |= DMA0TSEL_24;

    // Don't let DMA interrupt read-modify-write CPU operations
    DMACTL4 |= DMARMWDIS;

    // Select block transfer, increment both source and dest addresses
    DMA0CTL |= DMADT_1 | DMADSTINCR_3 | DMASRCINCR_3;

    // Set source address to first ADC conversion memory, destination to ADC
    // buffer, transfer 6 words (6 channels)
    DMA0SA = (uintptr_t)&ADC12MEM0;
    DMA0DA = (uintptr_t)&(sb->adc);
    DMA0SZ = ADC_CHANNELS;
}

/**
 * Enable DMA on channel 0 which will move data to the sample buffer
 * after the ADC conversion run has completed.
 */
void adc_convert(void)
{
    // Enable DMA on Channel 0
    DMA0CTL |= DMAEN;

    // Start conversion and wait until the DMA transfer completes
    ADC12CTL0 |= ADC12SC;
}
