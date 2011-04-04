/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <Wire.h>
#include "M24LC256.h"

// M24LC256 base address on I2C bus

const uint8_t I2C_M24LC256 = B1010000;

M24LC256::M24LC256(uint8_t _address)
{
    i2c_address = _address | I2C_M24LC256;
}

// Call AFTER "Wire.begin()"
void M24LC256::begin(void)
{
    // Actually, we have nothing to do
}

// Note that we use an internal "page size" of 16.  The chip has a page size of 64 bytes,
// and the wire library can only send 30 at a time.  So the largest even factor of 64 that's
// less than 30 is 16, so we write & read even chunks of 16.
//
// The public read/write functions can read/write any amount of data, to any location.
// Pages are automaticaly handled so we never do a page write over a page boundary.

// A chunk is the largest even factor of a page size that the Wire library can write,
// 16 bytes

// internal page size scales to the size of the Wire buffer.  Increase these values in Wire.h and utility/twi.h
// to get much faster speed.  The chip has a 5msec write cycle no matter how many bytes you push it, up to
// 64.

#if ( BUFFER_LENGTH < 34 ) || ( TWI_BUFFER_LENGTH < 34 )
const int page_size = 0x10;
#elif ( BUFFER_LENGTH < 66 ) || ( TWI_BUFFER_LENGTH < 66 )
const int page_size = 0x20;
#else
const int page_size = 0x40;
#endif

uint16_t M24LC256::WriteChunk(uint16_t location, uint16_t length, uint8_t* data)
{
    uint16_t bytes_written = min(length,page_size);

    //printf("Writing %u to %u from %p\n",bytes_written,location,data);

    Wire.beginTransmission(i2c_address);  // START condition
    Wire.send((uint8_t)(location >> 8));
    Wire.send((uint8_t)(location & 0xff));
    Wire.send(data,bytes_written);
    Wire.endTransmission();
    delay(5);  // 5msec write cycle time (Datasheet, pg.4

    return bytes_written;
}

void M24LC256::write(uint16_t location, void* buf, uint16_t len)
{
    uint16_t remaining = len;
    uint8_t* next_in = reinterpret_cast<uint8_t*>(buf);
    uint16_t next_out = location;

    // First, deal with page boundaries.  If location is NOT on an even 16 bytes,
    // we'll want to write out only enough to get to the end of this page.

    if ( location & (page_size-1) )
    {
        uint16_t chunk_length = min(remaining, page_size - ( location & (page_size-1) ));

        uint16_t written = WriteChunk( next_out, chunk_length, next_in );
        remaining -= written;
        next_in += written;
        next_out += written;
    }

    while ( remaining )
    {
        uint16_t written = WriteChunk( next_out, remaining, next_in );
        remaining -= written;
        next_in += written;
        next_out += written;
    }
}

uint16_t M24LC256::ReadChunk(uint16_t location, uint16_t length, uint8_t* data)
{
    uint16_t bytes_received = 0;
    uint16_t bytes_requested = min(length,16);

    Wire.beginTransmission(i2c_address);  // START condition
    Wire.send((uint8_t)(location >> 8));
    Wire.send((uint8_t)(location & 0xff));

    Wire.endTransmission();

    Wire.requestFrom(i2c_address,bytes_requested);
    uint16_t remaining = bytes_requested;
    uint8_t* next = data;

    while (Wire.available() && remaining--)
    {
        *next++ = Wire.receive();
        ++bytes_received;
    }

    //*next = 0; // null terminate fur debug printing purposes
    //printf("Read %u from %u to %p %s\n",bytes_received,location,data, (char*)data);

    delay(1);
    return bytes_received;
}

uint16_t M24LC256::read(uint16_t location, void* buf, uint16_t len)
{
    uint16_t bytes_received = 0;
    uint16_t remaining = len;
    uint8_t* next_memory = reinterpret_cast<uint8_t*>(buf);
    uint16_t next_chip = location;

    // First, deal with page boundaries.  If location is NOT on an even 16 bytes,
    // we'll want to write out only enough to get to the end of this page.

    if ( location & 0xf )
    {
        uint16_t chunk_length = min( remaining, 0x10 - ( location & 0xf ) );

        uint16_t chunk_received = ReadChunk(next_chip,chunk_length,next_memory);
        remaining -= chunk_received;
        next_memory += chunk_received;
        next_chip += chunk_received;
        bytes_received += chunk_received;
    }

    while ( remaining )
    {
        uint16_t chunk_received = ReadChunk(next_chip,remaining,next_memory);
        remaining -= chunk_received;
        next_memory += chunk_received;
        next_chip += chunk_received;
        bytes_received += chunk_received;
        if ( ! chunk_received ) // if we don't get any data, fall out so we don't hang here forever.
            break;
    }

    return bytes_received;
}

// vim:ci:sw=4 sts=4 ft=cpp
