/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
 * 
 * @file logger.h
 * @author Jon Sowman
 * @addtogroup logger
 * @{
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <legacymsp430.h>
#include "typedefs.h"
#include "ff.h"

#define S1_PORT_OUT P1OUT
#define S1_PORT_REN P1REN
#define S1_PORT_IES P1IES
#define S1_PORT_IE P1IE
#define S1_PORT_IFG P1IFG
#define S1_PIN _BV(7)

#define S2_PORT_OUT P2OUT
#define S2_PORT_REN P2REN
#define S2_PORT_IES P2IES
#define S2_PORT_IE P2IE
#define S2_PORT_IFG P2IFG
#define S2_PIN _BV(2)

/**
 * Ring buffer length for the SD card, should be a multiple of 2 of the
 * sector size (512 bytes)
 */
#define SD_RINGBUF_LEN 2048

/**
 * The ring buffer mask value. This is automatically calculated.
 */
#define SD_RINGBUF_MASK (SD_RINGBUF_LEN - 1)

/**
 * @struct RingBuffer
 * @brief A structure that emulates a ring buffer
 * @var RingBuffer::buffer
 * A pointer to the start of the character buffer to be used by this ring 
 * buffer.
 * @var RingBuffer::head
 * A pointer to the head of the ring buffer (the next free byte available
 * for writing).
 * @var RingBuffer::tail
 * A pointer to the tail of the ring buffer (the next unread byte)
 * @var RingBuffer::len
 * The length of the ring buffer
 * @var RingBuffer::mask
 * This value is a ring buffer intrinsic and is automatically generated.
 * @var RingBuffer::overflow
 * A flag that will be set non-zero if a buffer overflow occurs (the head
 * tries to "overtake" the tail.
 */
typedef struct RingBuffer
{
    char* buffer;
    uint16_t head, tail, len, mask;
    uint8_t overflow;
} RingBuffer;

/**
 * The number of ADC channels that we will sample from. It is vital that this
 * is correctly set such that the DMA system will work properly.
 */
#define ADC_CHANNELS 7

/**
 * The number of accelerometer channels (typically 3) to be sampled from. This
 * must be set correctly such that the DMA transfer system will work correctly.
 */
#define ACCEL_CHANNELS 3

/**
 * @struct SampleBuffer
 * @brief A structure to contain one 'set' of samples from the vehicle.
 * @var SampleBuffer::adc
 * Storage for the ADC channels
 * @var SampleBuffer::accel
 * Storage for the accelerometer channels
 */
typedef struct SampleBuffer
{
    volatile uint16_t adc[ADC_CHANNELS];
    volatile uint16_t accel[ACCEL_CHANNELS];
} SampleBuffer;

void logger_init(void);
void sd_setup(RingBuffer *sdbuf);
FRESULT sd_write(RingBuffer *rb, char *writebuf, FIL *fil, uint16_t n);
uint8_t ringbuf_write(RingBuffer* buf, char* data, uint16_t n);
uint8_t ringbuf_read(RingBuffer *buf, char* read_buffer, uint16_t n);
void update_lcd(RingBuffer *buf);
void logger_enable(void);
void logger_disable(void);

#endif /* __LOGGER_H__ */

/**
 * @}
 */
