/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include "adc.h"
#include "delay.h"

/**
 * Set up the ADC clock and configure resolution, then enable the ADC
 * unit.
 */
void adc_init(void)
{
    // Be sure that conversions are disabled
    ADC12CTL0 &= ~ADC12ENC;

    // Enable ADC12 and set sample time
    ADC12CTL0 |= ADC12ON | ADC12SHT0_12 | ADC12SHT1_12;

    // Now divide by 5 (0b101) to get 4MHz and clock from SMCLK
    // Use the sampling timer (SHP)
    ADC12CTL1 |= ADC12DIV_4 | ADC12SSEL_3 | ADC12SHP;

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
    while(!(ADC12IFG & ADC12IFG0));
    return ADC12MEM0;
}
