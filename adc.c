/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "adc.h"

/**
 * Set up the ADC clock and configure resolution, then enable the ADC
 * unit.
 */
void adc_init(void)
{
    // We clock from SMCLK which is 20MHz
    // Predivide the clock by 4 to get 5MHz
    ADC12CTL2 |= ADC12PDIV;

    // Now divide by 5 (0b101) to get 1MHz and clock from SMCLK
    // Use the sampling timer (SHP)
    ADC12CTL1 |= ADC12DIV2 | ADC12DIV0 | ADC12SSEL_3 | ADC12SHP;

    // Set 12 bit resolution
    ADC12CTL2 |= ADC12RES_3;

    // Finally, enable ADC12 and set sample time
    ADC12CTL0 |= ADC12ON | ADC12SHT02 | ADC12SHT12;

    // Enable conversions
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
    // Set the range to Vcc & GND by clearing bits 4-6 in the MCTLx reg
    // Also clear the 'EOS' bit 7
    // Set the channel that this conversion memory register is concerned with
    ADC12MCTL0 = (channel & 0x0F);
}

/**
 * Run a single conversion on the currently selected channel (or
 * group of channels for sequential conversion mode).
 */
uint16_t adc_convert(void)
{
    // Start conversion
    ADC12CTL0 |= ADC12SC;
    while(!(ADC12IFG & ADC12IFG0));
    return ADC12MEM0;
}
