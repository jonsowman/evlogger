/**
 * Handles configuration and initialisation of the CMA3000 onboard
 * accelerometer.
 *
 * This module is based on the TI example code (see the notice in the source
 * code) buit is heavily modified to use a finite state machine (FSM) type
 * approach to getting data from the accelerometer such that very little CPU
 * time is required (since the logger is typically busy with other things).
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
 * @file       accel.c
 *
 * @author TI, Modified by Jon Sowman
 * @addtogroup Accelerometer
 * @{
 */

#include <inttypes.h>
#include "msp430.h"
#include "accel.h"
#include "system.h"
#include "uart.h"

static int8_t accelData;
static int8_t RevID;

int8_t Cma3000_xAccel;
int8_t Cma3000_yAccel;
int8_t Cma3000_zAccel;

// Maintain a pointer to the SampleBuffer
static volatile SampleBuffer *sb;

/**
 * Contain the current accelerometer state. This is volatile since it is
 * modified in the interrupt service routine.
 */
static volatile accel_state_t accel_state;

/**
 * Configures the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 * @param samplebuffer The SampleBuffer in which to place accelerometer data
 * samples.
 */
void Cma3000_init(volatile SampleBuffer *samplebuffer)
{
    uint8_t i;
    sb = samplebuffer;
    
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
    for(i=0; i < ACCEL_CHANNELS; i++)
        sb->accel[i] = 0;

    // Fire an interrupt when we get a new char
    UCA0IE |= UCRXIE;
}

/**
 * Disables the CMA3000-D01 3-Axis Ultra Low Power Accelerometer
 */

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

/**
 * Reads data from the accelerometer
 */

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

/**
 * Reads data from the accelerometer
 * @param  Address  Address of register
 * @return Register contents
 */

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
 * Commence reading of data into the SampleBuffer
 */
void Cma3000_readAccelFSM(void)
{
    accel_state = STATE_ACCEL_NONE;

    // Assert CS
    ACCEL_OUT &= ~ACCEL_CS;

    // Transmit the first byte (the FSM will handle from here on)
    UCA0TXBUF = DOUTX << 2;

    // Move into the first state
    accel_state = STATE_ACCEL_XREQ;
}

/**
 * Get the current state of the accelerometer
 * \returns The current state as an accel_state_t.
 */
accel_state_t Cma3000_getState(void)
{
    return accel_state;
}

/**
 * Writes data to the accelerometer
 * @param  Address  Address of register
 * @param  accelData     Data to be written to the accelerometer
 * @return  Received data
 */
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
 * Interrupt whenever we get a new byte from the accelerometer. We should
 * update the state machine and process the byte accordingly.
 */
interrupt(USCI_A0_VECTOR) USCI_A0_ISR(void)
{
    switch(UCA0IV)
    {
        case USCI_UCRXIFG:
            // Process depending on current accel state
            switch(accel_state)
            {
                case STATE_ACCEL_NONE:
                    // What are we doing here?
                    break;
                case STATE_ACCEL_XREQ:
                    // Transmit a dummy to get data x
                    UCA0TXBUF = 0;
                    accel_state = STATE_ACCEL_XRDY;
                    break;
                case STATE_ACCEL_XRDY:
                    // We've got data x, store it and move to y
                    sb->accel[0] = UCA0RXBUF;
                    UCA0TXBUF = DOUTY << 2;
                    accel_state = STATE_ACCEL_YREQ;
                    break;
                case STATE_ACCEL_YREQ:
                    // Transmit a dummy to get Y
                    UCA0TXBUF = 0;
                    accel_state = STATE_ACCEL_YRDY;
                    break;
                case STATE_ACCEL_YRDY:
                    // We've got y, store it and move to z
                    sb->accel[1] = UCA0RXBUF;
                    UCA0TXBUF = DOUTZ << 2;
                    accel_state = STATE_ACCEL_ZREQ;
                    break;
                case STATE_ACCEL_ZREQ:
                    // Transmit a dummy to get z
                    UCA0TXBUF = 0;
                    accel_state = STATE_ACCEL_ZRDY;
                    break;
                case STATE_ACCEL_ZRDY:
                    // We've got z, store it and finish
                    sb->accel[2] = UCA0RXBUF;
                    accel_state = STATE_ACCEL_DONE;
                    // Deselect acceleration sensor
                    ACCEL_OUT |= ACCEL_CS;
                    break;
                default: break;
            }
            break;
        default:
            break;
    }
}

/**
 * @}
 */
