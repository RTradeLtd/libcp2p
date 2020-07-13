#include <multihash/dec.h>

/*!
 * @author bonedaddy
 * @note used to silence `ISO C forbids specifying range of elements to
 * initialize `
 * @note used to silence `initialized field overwritten`
 */
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Woverride-init"

static const long value_table[] = {[0 ... 255] = -1, ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ['A'] = 10, 11, 12, 13, 14, 15, ['a'] = 10, 11, 12, 13, 14, 15};

unsigned long long bin_to_dec(unsigned const char *bin) {
    unsigned long long dec = 0;
    int count = 0;
    while (bin[count] != '\0') {
        if (bin[count] == '1')
            dec = dec * 2 + 1;
        else if (bin[count] == '0')
            dec *= 2;
        count++;
    }
    return dec;
}

long hex_to_dec(unsigned const char *hex) {
    long dec = 0;
    while (*hex && dec >= 0) {
        dec = (dec << 4) | value_table[*hex++];
    }
    return dec;
}