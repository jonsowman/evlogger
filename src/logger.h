/**
 * EV Datalogger Project
 *
 * Jon Sowman 2014 <js39g13@soton.ac.uk>
 * University of Southampton
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

// Ring buffer length for the SD card, should be a multiple of 2 of the
// sector size (512 bytes)
#define SD_RINGBUF_LEN 2048

// Don't change the below line
#define SD_RINGBUF_MASK (SD_RINGBUF_LEN - 1)

typedef struct RingBuffer
{
    char* buffer;
    uint16_t head, tail, len, mask;
    uint8_t overflow;
} RingBuffer;

void logger_init(void);
void sd_setup(RingBuffer *sdbuf);
FRESULT sd_write(RingBuffer *rb, char *writebuf, FIL *fil, uint16_t n);
uint8_t ringbuf_write(RingBuffer* buf, char* data, uint16_t n);
uint8_t ringbuf_read(RingBuffer *buf, char* read_buffer, uint16_t n);
void update_lcd(RingBuffer *buf);
void logger_enable(void);
void logger_disable(void);

#endif /* __LOGGER_H__ */
