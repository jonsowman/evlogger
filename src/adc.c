/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "adc.h"
#include "system.h"

/**
 * Set up the ADC clock and configure resolution, then enable the ADC
 * unit.
 */
void adc_init(void)
{
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
    // Set end of sequence (EOS) for final channel
    ADC12MCTL5 = ADC12INCH_15 | ADC12EOS;

    // Enable conversions last, can't modify ADC12ON whilst ADC12ENC=1
    ADC12CTL0 |= ADC12ENC;
}

/**
 * Select which of the channels (0-15) should be:
 * - The channel to read from (single conversion mode)
 * - The start channel (sequential conversion mode)
 * \param channel The channel number
 */
void adc_select(const uint8_t channel)
{
    // Need to disable conversions before we can change ADC12MCTLx
    ADC12CTL0 &= ~ADC12ENC;

    // Set the range to Vcc & GND by clearing bits 4-6 in the MCTLx reg
    // Also clear the 'EOS' bit 7
    // Set the channel that this conversion memory register is concerned with
    ADC12MCTL0 = (channel & 0x0F);

    // Re-enable conversions
    ADC12CTL0 |= ADC12ENC;
}

/**
 * Run a single conversion on the currently selected channel (or
 * group of channels for sequential conversion mode).
 * \return The result of the ADC conversion.
 */
uint16_t adc_convert(void)
{
    // Start conversion
    ADC12CTL0 |= ADC12SC;
    while(!(ADC12IFG & ADC12IFG5));
    return ADC12MEM5;
}
