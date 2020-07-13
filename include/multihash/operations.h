
#ifndef OPERATIONS_H
#define OPERATIONS_H

// from
// https://github.com/multiformats/c-multihash/pull/6/files?file-filters%5B%5D=.h&file-filters%5B%5D=.txt&file-filters%5B%5D=No+extension&file-filters%5B%5D=dotfile

void xor_bin(const unsigned char *bin1, const unsigned char *bin2,
             unsigned char *dest);

#endif /* OPERATIONS_H */