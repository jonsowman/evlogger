/*******************************************************************************
 *
 *  HAL_Cma3000.h
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
 *  @file: accel.h
 *  @addtogroup Accelerometer
 *  @{
 *
 ******************************************************************************/

#ifndef HAL_CMA3000_H
#define HAL_CMA3000_H

#include <stdint.h>
#include "logger.h"

#define DOUTX       0x06
#define DOUTY       0x07
#define DOUTZ       0x08

// CONSTANTS
#define TICKSPERUS              (F_CPU/ 1000000)

// PORT DEFINITIONS
#define ACCEL_INT_IN            P2IN
#define ACCEL_INT_OUT           P2OUT
#define ACCEL_INT_DIR           P2DIR
#define ACCEL_SCK_SEL           P2SEL
#define ACCEL_INT_IE            P2IE
#define ACCEL_INT_IES           P2IES
#define ACCEL_INT_IFG           P2IFG
#define ACCEL_INT_VECTOR        PORT2_VECTOR
#define ACCEL_OUT               P3OUT
#define ACCEL_DIR               P3DIR
#define ACCEL_SEL               P3SEL

// PIN DEFINITIONS
#define ACCEL_INT               BIT5
#define ACCEL_CS                BIT5
#define ACCEL_SIMO              BIT3
#define ACCEL_SOMI              BIT4
#define ACCEL_SCK               BIT7
#define ACCEL_PWR               BIT6

// ACCELEROMETER REGISTER DEFINITIONS
#define REVID                   0x01
#define CTRL                    0x02
#define MODE_400                0x04        // Measurement mode 400 Hz ODR
#define DOUTX                   0x06
#define DOUTY                   0x07
#define DOUTZ                   0x08
#define G_RANGE_2               0x80        // 2g range
#define I2C_DIS                 0x10        // I2C disabled

extern int8_t Cma3000_xAccel;
extern int8_t Cma3000_yAccel;
extern int8_t Cma3000_zAccel;

/**
 * Enumerate possible states for the accelerometer finite state machine (FSM)
 */
typedef enum accel_state_t
{
    /// We have no or invalid data in the SampleBuffer
    STATE_ACCEL_NONE,
    /// We have requested the data value for X
    STATE_ACCEL_XREQ,
    /// A data value for X is ready
    STATE_ACCEL_XRDY,
    /// We have requested the data value for Y
    STATE_ACCEL_YREQ,
    /// A data value for Y is ready
    STATE_ACCEL_YRDY,
    /// We have requested the data value for Z
    STATE_ACCEL_ZREQ,
    /// A data value for Z is ready
    STATE_ACCEL_ZRDY,
    /// We have completed, there is a full set of valid data in th
    /// sample buffer
    STATE_ACCEL_DONE
} accel_state_t;

extern void Cma3000_init(volatile SampleBuffer *sb);
extern void Cma3000_disable(void);
extern void Cma3000_readAccel(void);
extern void Cma3000_setAccel_offset(int8_t xAccel_offset, int8_t yAccel_offset, int8_t zAccel_offset);
extern void Cma3000_readAccel_offset(void);
extern int8_t Cma3000_readRegister(uint8_t Address);
void Cma3000_readAccelFSM(void);
accel_state_t Cma3000_getState(void);
extern int8_t Cma3000_writeRegister(uint8_t Address, int8_t Data);

#endif /* HAL_MENU_H */

/**
 * @}
 */
