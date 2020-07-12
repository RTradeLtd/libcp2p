#ifndef MH_HULTIHASH_H
#define MH_HULTIHASH_H

#include <stddef.h>

/**
 * Functions for working with a multihash.
 * A multihash is a hash with a prefix "code" that
 * helps determine what type of hash this is.
 * code name
 * 0x00 identity
 * 0x11 sha1
 * 0x12 sha2-256
 * 0x13 sha2-512
 * 0x14 sha3-512
 * 0x15 sha3-384
 * 0x16 sha3-256
 * 0x17 sha3-224
 * 0x18 shake-128
 * 0x19 shake-256
 * 0x40 blake2b
 * 0x41 blake2s
 * # 0x0400-0x040f reserved for application specific functions
 * # 0x14 formerly had the name "sha3", now deprecated
 */

/**
 * returns hash code or error (which is < 0)
 * @param mh the multihash
 * @param len the length of the multihash
 * @returns errors ( < 0 ) or the multihash
 */
int mh_multihash_hash(const unsigned char *multihash, size_t len);

/***
 * returns the length of the multihash's data section
 * @param mh the multihash
 * @param len the length of the multihash
 * @returns the length of the data section, or an error if < 0
 */
int mh_multihash_length(const unsigned char *multihash, size_t len);

/**
 * gives access to raw digest inside multihash buffer
 * @param multihash the multihash
 * @param len the length
 * @param digest the results
 * @returns error if less than zero, otherwise 0
 */
int mh_multihash_digest(const unsigned char *multihash, size_t len,
	 unsigned char **digest, size_t *digest_len);

/**
 * determine the size of the multihash given the data size
 * @param code currently not used
 * @param hash_len the data size
 * @returns hash_len + 2 (until the code parameter (varint) is added
 */
int mh_new_length(int code, size_t digest_len);

/***
 * create a multihash based on some data
 * @param buffer where to put the multihash
 * @param code the code
 * @param digest the data within the multihash
 * @returns error (if < 0) or 0
 */
int mh_new(unsigned char* buffer, int code, const unsigned char *digest,
	size_t digest_len);

#endif /* end of include guard */
