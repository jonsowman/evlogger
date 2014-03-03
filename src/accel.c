/*******************************************************************************
 *
 *  HAL_Cma3000.c - Code for using the CMA3000-D01 3-Axis Ultra Low Power
 *                  Accelerometer
 *
 *  Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @file       accel.c
 * @addtogroup Accelerometer
 * @{
 ******************************************************************************/
#include <inttypes.h>
#include "msp430.h"
#include "accel.h"
#include "system.h"
#include "uart.h"

int8_t accelData;
int8_t RevID;
int8_t Cma3000_xAccel;
int8_t Cma3000_yAccel;
int8_t Cma3000_zAccel;

// Stores x-Offset
int8_t Cma3000_xAccel_offset;

// Stores y-Offset
int8_t Cma3000_yAccel_offset;

// Stores z-Offset
int8_t Cma3000_zAccel_offset;

// Maintain a pointer to the SampleBuffer
volatile SampleBuffer *sb;

// Temporary place to store incoming accel data
uint8_t rxbuf[7];

// These are the accel commands to be written via DMA (excluding 1st byte)
uint8_t cmdbuf[] = {0, DOUTY << 2, 0, DOUTZ << 2, 0, 0, 0};

//FIXME
char s[20];

/**
 * @brief  Configures the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 * @param  none
 * @return none
 */
void Cma3000_init(volatile SampleBuffer *samplebuffer)
{
    uint8_t i;

    do
    {
        // Set P3.6 to output direction high
        ACCEL_OUT |= ACCEL_PWR;
        ACCEL_DIR |= ACCEL_PWR;

        // P3.3,4 option select
        ACCEL_SEL |= ACCEL_SIMO + ACCEL_SOMI;

        // P2.7 option select
        ACCEL_SCK_SEL |= ACCEL_SCK;

        ACCEL_INT_DIR &= ~ACCEL_INT;

        // Generate interrupt on Lo to Hi edge
        ACCEL_INT_IES &= ~ACCEL_INT;

        // Clear interrupt flag
        ACCEL_INT_IFG &= ~ACCEL_INT;

        // Unselect acceleration sensor
        ACCEL_OUT |= ACCEL_CS;
        ACCEL_DIR |= ACCEL_CS;

        // **Put state machine in reset**
        UCA0CTL1 |= UCSWRST;
        // 3-pin, 8-bit SPI master Clock polarity high, MSB
        UCA0CTL0 = UCMST + UCSYNC + UCCKPH + UCMSB;
        // Use SMCLK, keep RESET
        UCA0CTL1 = UCSWRST + UCSSEL_2;
        // /0x30
        UCA0BR0 = 0x30;
        // 0
        UCA0BR1 = 0;
        // No modulation
        UCA0MCTL = 0;
        // **Initialize USCI state machine**
        UCA0CTL1 &= ~UCSWRST;

        // Read REVID register
        RevID = Cma3000_readRegister(REVID);
        __delay_cycles(50 * TICKSPERUS);

        // Activate measurement mode: 2g/400Hz
        accelData = Cma3000_writeRegister(CTRL, G_RANGE_2 | I2C_DIS | MODE_400);

        // Settling time per DS = 10ms
        __delay_cycles(1000 * TICKSPERUS);

        // INT pin interrupt disabled
        ACCEL_INT_IE  &= ~ACCEL_INT;

        // Repeat till interrupt Flag is set to show sensor is working
    } while (!(ACCEL_INT_IN & ACCEL_INT));

    // Clear the sample buffer accelerometer data
    sb = samplebuffer;
    for(i=0; i < ACCEL_CHANNELS; i++)
        sb->accel[i] = 0;

    // Clear the accel temp buffer
    for(i=0; i<7; i++)
        rxbuf[i] = 0;

    // Set DMA1 to write to the accel, DMA2 to read from it
    DMACTL0 |= DMA1TSEL_17;
    DMACTL1 |= DMA2TSEL_16;

    // Enable round robin and don't let DMA interrupt RMW cycles
    DMACTL4 |= ROUNDROBIN | DMARMWDIS;

    // Set up burst block transfer. Increment source for DMA1, dst for DMA2
    // Source and dest are both bytes
    DMA1CTL |= DMADT_2 | DMASRCINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL;
    DMA2CTL |= DMADT_2 | DMADSTINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL;

    // DMA1 - transfer from command buffer to SPI TX
    // DMA2 - transfer from SPI RX to receive buffer
    // A transfer is 2 bytes each way
    DMA1SA = (uintptr_t)cmdbuf;
    DMA1DA = (uintptr_t)&UCA0TXBUF;
    DMA1SZ = 7;
    DMA2SA = (uintptr_t)&UCA0RXBUF;
    DMA2DA = (uintptr_t)rxbuf;
    DMA2SZ = 7;

    // Fire an interrupt when receive completes
    DMA2CTL |= DMAIE;
}

/***************************************************************************//**
 * @brief  Disables the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 * @param  none
 * @return none
 ******************************************************************************/

void Cma3000_disable(void)
{
    // Set P3.6 to output direction low
    ACCEL_OUT &= ~ACCEL_PWR;

    // Disable P3.3,4 option select
    ACCEL_SEL &= ~(ACCEL_SIMO + ACCEL_SOMI);

    // Disable P2.7 option select
    ACCEL_SCK_SEL &= ~ACCEL_SCK;

    // Set CSn to low
    ACCEL_OUT &= ~ACCEL_CS;

    // INT pin interrupt disabled
    ACCEL_INT_IE  &= ~ACCEL_INT;

    // **Put state machine in reset**
    UCA0CTL1 |= UCSWRST;
}

/***************************************************************************//**
 * @brief  Reads data from the accelerometer
 * @param  None
 * @return None
 ******************************************************************************/

void Cma3000_readAccel(void)
{
    // Read DOUTX register
    Cma3000_xAccel = Cma3000_readRegister(DOUTX);
    __delay_cycles(50 * TICKSPERUS);

    // Read DOUTY register
    Cma3000_yAccel = Cma3000_readRegister(DOUTY);
    __delay_cycles(50 * TICKSPERUS);

    // Read DOUTZ register
    Cma3000_zAccel = Cma3000_readRegister(DOUTZ);
}

/***************************************************************************//**
 * @brief  Sets accelerometer offset.
 * @param  xAccel_offset  x-axis offset
 * @param  yAccel_offset  y-axis offset
 * @param  zAccel_offset  z-axis offset
 * @return None
 ******************************************************************************/

void Cma3000_setAccel_offset(int8_t xAccel_offset,
                             int8_t yAccel_offset,
                             int8_t zAccel_offset)
{
    // Store x-Offset
    Cma3000_xAccel_offset = xAccel_offset;

    // Store y-Offset
    Cma3000_yAccel_offset = yAccel_offset;

    // Store z-Offset
    Cma3000_zAccel_offset = zAccel_offset;
}

/***************************************************************************//**
 * @brief  Reads data from the accelerometer with removed offset
 * @param  None
 * @return None
 ******************************************************************************/

void Cma3000_readAccel_offset(void)
{
    // Read current accelerometer value
    Cma3000_readAccel();

    // remove offset
    Cma3000_xAccel -= Cma3000_xAccel_offset;

    // remove offset
    Cma3000_yAccel -= Cma3000_yAccel_offset;

    // remove offset
    Cma3000_zAccel -= Cma3000_zAccel_offset;
}

/***************************************************************************//**
 *
 * @brief  Reads data from the accelerometer
 * @param  Address  Address of register
 * @return Register contents
 ******************************************************************************/

int8_t Cma3000_readRegister(uint8_t Address)
{
    uint8_t Result;

    // Address to be shifted left by 2 and RW bit to be reset
    Address <<= 2;

    // Select acceleration sensor
    ACCEL_OUT &= ~ACCEL_CS;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write address to TX buffer
    UCA0TXBUF = Address;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write dummy data to TX buffer
    UCA0TXBUF = 0;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer
    Result = UCA0RXBUF;

    // Wait until USCI_A0 state machine is no longer busy
    while (UCA0STAT & UCBUSY) ;

    // Deselect acceleration sensor
    ACCEL_OUT |= ACCEL_CS;

    // Return new data from RX buffer
    return Result;
}

/**
 * Read all three accelerometer channels to the temporary buffer via DMA.
 * @param none
 * @return none
 */
void Cma3000_readAccelDMA(void)
{
    // Select (deselection happens in the DMA2IFG ISR)
    ACCEL_OUT &= ~ACCEL_CS;

    // Transmit the first byte manually which will then trigger DMA
    UCA0TXBUF = DOUTX << 2;

    // Enable the transfers over the two channels
    DMA1CTL |= DMAEN;
    DMA2CTL |= DMAEN;
}

/***************************************************************************//**
 * @brief  Writes data to the accelerometer
 * @param  Address  Address of register
 * @param  accelData     Data to be written to the accelerometer
 * @return  Received data
 ******************************************************************************/

int8_t Cma3000_writeRegister(uint8_t Address, int8_t accelData)
{
    uint8_t Result;

    // Address to be shifted left by 2
    Address <<= 2;

    // RW bit to be set
    Address |= 2;

    // Select acceleration sensor
    ACCEL_OUT &= ~ACCEL_CS;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write address to TX buffer
    UCA0TXBUF = Address;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer just to clear interrupt flag
    Result = UCA0RXBUF;

    // Wait until ready to write
    while (!(UCA0IFG & UCTXIFG)) ;

    // Write data to TX buffer
    UCA0TXBUF = accelData;

    // Wait until new data was written into RX buffer
    while (!(UCA0IFG & UCRXIFG)) ;

    // Read RX buffer
    Result = UCA0RXBUF;

    // Wait until USCI_A0 state machine is no longer busy
    while (UCA0STAT & UCBUSY) ;

    // Deselect acceleration sensor
    ACCEL_OUT |= ACCEL_CS;

    return Result;
}

/**
 * Interrupt when the DMA controller has finished receiving new data
 * from the accelerometer. We should process this data.
 *
 * @param none
 * @return none
 */
interrupt(DMA_VECTOR) DMA_ISR(void)
{
    P8OUT &= ~_BV(1);
    switch(DMAIV)
    {
        case DMAIV_DMA2IFG:
            // Deassert CS now that the transfer is complete
            ACCEL_OUT |= ACCEL_CS;

            // Move data into the sample buffer
            sb->accel[0] = (uint16_t)rxbuf[2];
            sb->accel[1] = (uint16_t)rxbuf[4];
            sb->accel[2] = (uint16_t)rxbuf[6];
            
            sprintf(s, "%u", sb->accel[0]);
            uart_debug(s);

            break;
        default:
            break;
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
