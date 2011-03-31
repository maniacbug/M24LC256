/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <WProgram.h>

/**
 * Driver for Microchip 24LC256 EEPROM
 *
 * Please refer to:
 *
 * @li <a href="http://maniacbug.github.com/M24LC256/classM24LC256.html">Detailed Documentation</a>
 * @li <a href="https://github.com/maniacbug/M24LC256">Source Code</a>
 * @li <a href="http://ww1.microchip.com/downloads/en/DeviceDoc/21203Q.pdf">Chip Datasheet</a>
 *
 * Create one instance of this class for each chip in your circuit.  This allows you
 * to have up to 8 of these active at any time.
 *
 * This class is
 * optimized to take advantage of the page size of the chip.  The chip has a 5msec
 * write cycle now matter how many bytes you push it, up to its page size (64 bytes
 * for 24LC256.  To get higher throughput, increase the size of the Wire buffer by
 * increasing BUFFER_LENGTH in Wire.h and TWI_BUFFER_LENGTH in utility/twi.h.  Increase
 * them to 34 for 32-byte writes, and to 66 for 64-byte writes.  Be warned that these
 * buffers suck up your memory, so be careful!
 */

class M24LC256
{
protected:
    /**
     * The 7-bit address of the particular device we're driving on the I2C bus.
     */
    uint8_t i2c_address;

    /**
     * @name Internals
     *
     * Protected methods that address the chip directly.
     */
    /**@{*/
    /**
     * Writes a small chunk of data directly to the chip.
     *
     * Only 16 bytes maximum are written at a time.  Less bytes are written
     * if @p location is not on an even 16-byte boundary.
     *
     * @param location The chip address where to write the data
     * @param length The number of bytes available to be written
     * @param data The in-memory location to find the data
     *
     * @returns The number of bytes written to the chip
     */
    uint16_t WriteChunk(uint16_t location, uint16_t length, uint8_t* data);

    /**
     * Reads a small chunk of data directly from the chip.
     *
     * Only 16 bytes maximum are read at a time.  Less bytes are read
     * if @p location is not on an even 16-byte boundary.
     *
     * @param location The chip address where to read the data
     * @param length The maximum number of bytes desired to be read
     * @param data The in-memory location of where to place the data
     *
     * @returns The number of bytes read from the chip
     */
    uint16_t ReadChunk(uint16_t location, uint16_t length, uint8_t* data);

    /**@}*/
public:
    /**
     * Constructor
     *
     * @param _address the low 3 bits of the address for the chip being
     * addressed.  This should correspond to the A0-A2 pins.
     */
    M24LC256(uint8_t _address);

    /**
     * Setup & prepare
     *
     * Call this function from within the setup() function of your
     * sketch, AFTER Wire.begin().
     *
     * Note that this function currently does nothing, but is included for forward
     * compatability.  Someday something may be needed here.
     */
    void begin(void);

    /**
     * Write data to any location on the chip
     *
     * This function is not constrained by the size or location of data.  Write what
     * you want, where you want.
     *
     * @warning This will not write beyond the top of the address space of the chip.
     * (0x8000).  If you attempt to write beyond the upper limit, only the amount which
     * fits within the memory limit is written, the rest is ignored.
     *
     * @param location The chip address where to write the data
     * @param buf The in-memory location of where to look for the data
     * @param len The number of bytes to be written
     */
    void write(uint16_t location, void* buf, uint16_t len);

    /**
     * Read data from any location on the chip
     *
     * This function is not constrained by the size or location of data.  Write what
     * you want, where you want.
     *
     * @warning This will not read beyond the top of the address space of the chip.
     * (0x8000).  If you attempt to read beyond the upper limit, only the amount found
     * before the memory limit is read. Check the return value for actual number of
     * bytes read.
     *
     * @param location The chip address where to read the data
     * @param buf The in-memory location of where to place the data
     * @param len The maximum number of bytes desired to be read
     *
     * @returns The number of bytes read from the chip
     */

    uint16_t read(uint16_t location, void* buf, uint16_t len);
};

/** @mainpage Driver library for 24LC256 EEPROM
 *
 * See the M24LC256 class for details on how to drive this chip.
 */

// vim:ci:sw=4 sts=4 ft=cpp
