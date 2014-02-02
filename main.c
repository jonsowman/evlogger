/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include <msp430.h>
#include <msp430f5529.h>
#include <stdint.h>
#include <stdio.h>

#include "HAL_Dogs102x6.h"
#include "typedefs.h"
#include "delay.h"
#include "uart.h"
#include "adc.h"
#include "clock.h"

#include "ff.h"
#include "diskio.h"

#define _BV(x) (1<<x)

void sys_clock_init(void);
void led_toggle(void);

int main( void )
{
    uint16_t adc_read;
    char s[30];
    FATFS FatFs;
    FRESULT fr;
    FIL fil;
    UINT bw;

    // Stop the wdt
    WDTCTL = WDTPW | WDTHOLD;

    // Set up the system clock and any required peripherals
    sys_clock_init();
    clock_init();
    uart_init();
    sd_init();
    adc_init();
    Dogs102x6_init();
    Dogs102x6_backlightInit();

    // Enable LED on P1.0 and turn it off
    P1DIR |= _BV(0);
    P1OUT &= ~_BV(0);

    // Select the potentiometer and enable the ADC on that channel
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);
    P6SEL |= _BV(5);
    adc_select(0x05);

    // Wait for peripherals to boot
    _delay_ms(100);
    
    // Test that minicom/term is behaving
    uart_debug("Hello world");

    // Flash the LED at 1 second
    register_function_1s(&led_toggle);

    // Call the periodic fatfs timer functionality
    register_function_10ms(&disk_timerproc);

    // Test the LCD
    Dogs102x6_setBacklight(8);
    Dogs102x6_setContrast(8);
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, "Hello World", DOGS102x6_DRAW_NORMAL);
    
    while(1);

    // Mount the FAT filesystem
    _delay_ms(100);
    fr = f_mount(&FatFs, "", 1);
    while( fr != FR_OK )
    {
        sprintf(s, "Mount failed: %d", fr);
        uart_debug(s);
        _delay_ms(1000);
        fr = f_mount(&FatFs, "", 1);
    }

    fr = f_open(&fil, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if( fr == FR_OK )
    {
        f_write(&fil, "Some test text\r\n", 17, &bw);
        f_close(&fil);
        if(bw == 17)
            uart_debug("Wrote full data");
        else
            uart_debug("Wrote partial data");
    }
    else
    {
        sprintf(s, "Couldn't open file: %d", fr);
        uart_debug(s);
    }

    while(1)
    {
        adc_read = adc_convert();
        sprintf(s, "%u", adc_read);
        uart_debug(s);
        _delay_ms(1000);
    }
    return 0;
}

/**
 * Set up the system clock to use the external crystal as the stabilisation
 * source for the FLL and have MCLK at 20MHz.
 */
void sys_clock_init( void )
{
    uint16_t i;

    // Port select XT2
    P5SEL |= (1 << 2) | (1 << 3);

    // Enable XT2 (4MHz xtal attached to XT2) and disable XT1 (LF & HF)
    UCSCTL6 &= ~XT2OFF;
    UCSCTL6 |= XT1OFF;
    _BIS_SR(OSCOFF); // Disable LFXT1

    // Wait for XT2 to stabilise
    do {
        UCSCTL7 &= ~XT2OFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & XT2OFFG );

    // Set FLL reference to be XT2 divided by 2 (FLLREFDIV=2)
    UCSCTL3 = SELREF__XT2CLK | FLLREFDIV__2;

    // Set the FLL loop divider to 4 (D=4) and the multiplier to 5 (N=4)
    // DCOCLK = D * (N+1) * (FLLREFCLK / FLLREFDIV)
    UCSCTL2 = 0x0004;
    UCSCTL2 |= FLLD__4; // compensate for N=0 disallowed

    // Set the DCO to range 4 (1.3 - 28.2MHz, target 20MHz)
    UCSCTL1 = DCORSEL_4;

    // Wait until the DCO has stabilised
    do {
        UCSCTL7 &= ~DCOFFG;
        for( i = 0xFFF; i > 0; i--);
    } while( UCSCTL7 & DCOFFG );

    // At this point, DCOCLK is a 20MHz stabilised reference
    // So set MCLK and SMCLK to use this
    UCSCTL4 = SELS_3 | SELM_3;
}

/**
 * Toggle the LED on P1.0, requires already set as output
 */
void led_toggle(void)
{
    P1OUT ^= _BV(0);
}
