#include <multihash/base64.h>
#include <multihash/dec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char b64[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                              'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                              's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

int mod(size_t octets) {
    if (!(octets % 5))
        return 0;
    else if (!(octets % 2))
        return 2;
    else
        return 1;
}

unsigned char *bin_to_b64(unsigned const char *bin) {
    size_t length = strlen((const char *)bin);

    if (length % 8) {
        printf("%s\n", "Please provide whole bytes!");
        return NULL;
    }

    unsigned char *in = malloc(length + 24);
    memcpy(in, bin, length);

    size_t octets = (length - (length % 8)) / 8;
    if (octets % 3 || !octets) {
        size_t to_go = ((8 - (length % 8)) + mod(octets) * 8);
        for (int i = 0; i < (int)to_go; i++) {
            in[length + (to_go - i) - 1] = '0';
        }
        length += to_go;
    }

    unsigned char *out = malloc(4 * ((length * 2) / 3));

    size_t chunks = length / 6;
    for (int i = 0; i < (int)chunks; i++) {
        unsigned char *chunk = malloc(6);
        for (int j = 0; j < 6; j++) {
            chunk[j] = in[(i * 6) + j];
        }
        if ((i == (int)chunks - 1 || i == (int)chunks - 2) && bin_to_dec(chunk) == 0) {
            out[i] = '=';
        } else {
            out[i] = b64[bin_to_dec(chunk)];
        }
    }

    return out;
}