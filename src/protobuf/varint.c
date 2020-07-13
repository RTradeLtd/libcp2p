#include "varint.h"
#include <assert.h>

// to fix overflow in conversion from ‘int’ to ‘char’ changes value from ‘128’
// to ‘-128’ use unsigned char
static const unsigned char MSB = 0x80;
static const char MSBALL = ~0x7F;

static const unsigned long long N1 = 128; // 2 ^ 7
static const unsigned long long N2 = 16384;
static const unsigned long long N3 = 2097152;
static const unsigned long long N4 = 268435456;
static const unsigned long long N5 = 34359738368;
static const unsigned long long N6 = 4398046511104;
static const unsigned long long N7 = 562949953421312;
static const unsigned long long N8 = 72057594037927936;
static const unsigned long long N9 = 9223372036854775808U;

int varint_encoding_length(unsigned long long n) {
    return (n < N1 ? 1 : n < N2 ? 2 : n < N3 ? 3 : n < N4 ? 4 : n < N5 ? 5 : n < N6 ? 6 : n < N7 ? 7 : n < N8 ? 8 : n < N9 ? 9 : 10);
}

/**
 * Encode an unsigned long long into a varint
 * @param n the number to encode
 * @param buf where to put the results
 * @param len the length of buf
 * @param bytes the length written
 * @returns a pointer to the buf
 * @todo
 *   0 determine why `len` isnt being used
 */
#pragma GCC diagnostic ignored "-Wunused-parameter" // when compiling with NDEBUG, len is unused and
                                                    // gives a warning
unsigned char *varint_encode(const unsigned long long n, unsigned char *buf, int len, size_t *bytes) {
    unsigned char *ptr = buf;
    unsigned long long copy = n;

    while (copy & MSBALL) {
        *(ptr++) = (copy & 0xFF) | MSB;
        copy = copy >> 7;
        assert((ptr - buf) < len);
    }
    *ptr = copy;
    if (bytes != NULL)
        *bytes = ptr - buf + 1;

    return buf;
}

/***
 * decode a varint
 * @param buf the buffer that contains the varint
 * @param len the length of the buffer
 * @param bytes number of bytes processed
 * @returns the value decoded
 */
unsigned long long varint_decode(const unsigned char *buf, int len, size_t *bytes) {
    unsigned long long result = 0;
    if (bytes != NULL)
        *bytes = 0;
    int bits = 0;
    const unsigned char *ptr = buf;
    unsigned long long ll;
    while (*ptr & MSB) {
        ll = *ptr;
        result += ((ll & 0x7F) << bits);
        ptr++;
        bits += 7;
        assert((ptr - buf) < len);
    }
    ll = *ptr;
    result += ((ll & 0x7F) << bits);

    if (bytes != NULL)
        *bytes = ptr - buf + 1;

    return result;
}
