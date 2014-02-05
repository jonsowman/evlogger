/**
 * EV Datalogger Project
 *
 * Jon Sowman 2013 <js39g13@soton.ac.uk>
 * University of Southampton
 */

#include <string.h>

#include "HAL_Dogs102x6.h"
#include "logger.h"
#include "adc.h"
#include "ff.h"
#include "uart.h"
#include "delay.h"
#include "clock.h"

volatile uint8_t logger_running;
char s[UART_BUF_LEN];
char sdbuf[SD_BUF_LEN];

// A FAT filesystem appears!
FATFS FatFs;
FIL fil;

/**
 * Set up the hardware for logging functionality
 */
void logger_init(void)
{
    // Enable LEDs and turn them off (P1.0, P8.1, P8.2)
    P1DIR |= _BV(0);
    P1OUT &= ~_BV(0);
    P8DIR |= _BV(1);
    P8OUT &= ~_BV(1);
    P8DIR |= _BV(2);
    P8OUT &= ~_BV(2);

    // Select the potentiometer and enable the ADC on that channel
    P8DIR |= _BV(0);
    P8OUT |= _BV(0);
    P6SEL |= _BV(5);
    adc_select(0x05);

    // Enable the buttons
    // S1 is on P1.7, S2 is on P2.2
    S1_PORT_OUT |= S1_PIN;
    S1_PORT_REN |= S1_PIN;
    S2_PORT_OUT |= S2_PIN;
    S2_PORT_REN |= S2_PIN;
    // Trigger on falling edge, clear int flags, enable interupts
    S1_PORT_IES &= ~S1_PIN;
    S1_PORT_IFG &= ~S1_PIN;
    S1_PORT_IE |= S1_PIN;
    S2_PORT_IES &= ~S2_PIN;
    S2_PORT_IFG &= ~S2_PIN;
    S2_PORT_IE |= S2_PIN;

    // Set up 16 bit timer TIMER1 to interrupt at the log frequency
    TA1CCR0 = 19999;

    // Clock from SMCLK with /8 divider, use "up" mode, use interrupts
    TA1CTL |= TASSEL_2 | TACLR | ID_3;

    // Enable interrupts on CCR0
    TA1CCTL0 |= CCIE;

    // Update the LCD regularly
    register_function_1s(&update_lcd);

    // Enable interrupts (if they're not already)
    eint();

    // Call the SD setup routine
    sd_setup();

    // The logger should start in its OFF state
    logger_running = 0;
}

/**
 * Update the LCD with the current status of the logger service. This should
 * not be called too regularly on the MSP-EXP430 board due to the SD card and
 * LCD panel being on the same SPI bus and will cause slowdown of SD
 * transactions.
 */
void update_lcd(void)
{
    uint16_t adc_read;

    P1OUT ^= _BV(0);

    adc_read = adc_convert();
    sprintf(s, "ADC: %u", adc_read);
    Dogs102x6_clearRow(2);
    Dogs102x6_stringDraw(2, 0, s, DOGS102x6_DRAW_NORMAL);
    _delay_ms(100);
}

/**
 * Set up the SD card for logging to it
 */
void sd_setup(void)
{   
    FRESULT fr;
    UINT bw;
    DWORD fsz;
    char filebuf[15];
    char stw[20] = "sometextsometext";

    fr = f_mount(0, &FatFs);
    while( fr != FR_OK )
    {
        sprintf(s, "Mount fail: %d", fr);
        uart_debug(s);
        _delay_ms(100);
        fr = f_mount(0, &FatFs);
    }

    // Attempt to open a file
    fr = f_open(&fil, "hello.txt", FA_READ | FA_WRITE);
    while( fr != FR_OK )
    {
        _delay_ms(500);
        sprintf(s, "Open fail: %d", fr);
        uart_debug(s);
        fr = f_open(&fil, "hello.txt", FA_READ | FA_WRITE);
    }

    // Determine the size of the file
    fsz = f_size(&fil);
    sprintf(s, "file is %d bytes", (int)fsz);
    uart_debug(s);

    // Try and read from the file
    fr = f_read(&fil, filebuf, 12, &bw);
    sprintf(s, "Read %d bytes, result %d", bw, fr);
    uart_debug(s);

    // Try and write something new, move to beginning of file
    fr = f_lseek(&fil, 16);
    fr = f_lseek(&fil, 0);

    // Write something else
    fr = f_write(&fil, stw, 2, &bw);
    sprintf(s, "Wrote %d bytes, result %d", bw, fr);
    uart_debug(s);

    fr = f_sync(&fil);
    sprintf(s, "synced, result %d", fr);
    uart_debug(s);

    // Close the file
    fr = f_close(&fil);
    sprintf(s, "closed, result %d", fr);
    uart_debug(s);

    uart_debug("Done");
}

/**
 * Enable TA1 to begin logging by setting mode control to "up" mode,
 * counter counts to TAxCCR0.
 */
void logger_enable(void)
{
    // Stop any timer activity
    TA1CTL &= ~MC_3;

    logger_running = 1;

    // Start the timer
    TA1CTL |= MC_1;
}

/**
 * Disable TA1 to halt logging by setting mode control to STOP.
 */
void logger_disable(void)
{
    // Clear bits 4 and 5
    TA1CTL &= ~MC_3;
    logger_running = 0;
}

/**
 * Interrupt for TA1, here we should log one block of data to the SD card.
 * In future we may buffer data and log it in blocks, or transfer it to the
 * SD card via DMA.
 */
interrupt(TIMER1_A0_VECTOR) TIMER1_A0_ISR(void)
{
    ;
}

/**
 * Interrupt vector for button S1
 */
interrupt(PORT1_VECTOR) PORT1_ISR(void)
{
    if(P1IV & P1IV_P1IFG7)
    {
        if(logger_running)
            logger_disable();
        else
            logger_enable();
    }

}

/**
 * Interrupt vector for button S2
 */
interrupt(PORT2_VECTOR) PORT2_ISR(void)
{
    if(P2IV & P2IV_P2IFG2)
    {
        P8OUT ^= _BV(1);
    }
}
