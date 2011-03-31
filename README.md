# Driver for Microchip 24LC256 EEPROM

See documentation at http://maniacbug.github.com/M24LC256/classM24LC256.html

See <a href="http://ww1.microchip.com/downloads/en/DeviceDoc/21203Q.pdf">datasheet</a>

Create one instance of this class for each chip in your circuit.  This allows you
to have up to 8 of these active at any time.

This class is
optimized to take advantage of the page size of the chip.  The chip has a 5msec
write cycle now matter how many bytes you push it, up to its page size (64 bytes
for 24LC256.  To get higher throughput, increase the size of the Wire buffer by
increasing BUFFER_LENGTH in Wire.h and TWI_BUFFER_LENGTH in utility/twi.h.  Increase
them to 34 for 32-byte writes, and to 66 for 64-byte writes.  Be warned that these
buffers suck up your memory, so be careful!
