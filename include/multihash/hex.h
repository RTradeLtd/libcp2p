#ifndef HEX_H
#define HEX_H

// from https://github.com/multiformats/c-multihash/pull/6/files?file-filters%5B%5D=.h&file-filters%5B%5D=.txt&file-filters%5B%5D=No+extension&file-filters%5B%5D=dotfile

void hex_to_bin(unsigned const char* hex, unsigned char* dest);

void bin_to_hex(unsigned const char* bin, unsigned char* dest);

void dec_to_hex(const unsigned long long* dec, unsigned char* dest);
#endif