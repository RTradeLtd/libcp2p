/***
 * handles varint
 */
#ifndef _VARINT_H_
#define _VARINT_H_
#include <stdlib.h>

/**
 * Encode an unsigned long long into a varint
 * @param n the number to encode
 * @param buf where to put the results
 * @param len the length of buf
 * @param bytes the length written
 * @returns a pointer to the buf
 */
unsigned char* varint_encode(const unsigned long long n, unsigned char* buf, int len, size_t* bytes);

/***
 * decode a varint
 * @param buf the buffer that contains the varint
 * @param len the length of the buffer
 * @param bytes number of bytes processed
 * @returns the value decoded
 */
unsigned long long varint_decode(const unsigned char* buf, int len, size_t* bytes);

/**
 *
 */
int varint_encoding_length(unsigned long long n);

#endif
