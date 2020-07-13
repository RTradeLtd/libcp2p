#ifndef DEC_H
#define DEC_H

// from
// https://github.com/multiformats/c-multihash/pull/6/files?file-filters%5B%5D=.h&file-filters%5B%5D=.txt&file-filters%5B%5D=No+extension&file-filters%5B%5D=dotfile

#include <stddef.h>

unsigned long long bin_to_dec(unsigned const char *bin);

long hex_to_dec(unsigned const char *hex);

#endif