#include "mh/multihash.h"

#include "mh/hashes.h"
#include "mh/errors.h"
#include "mh/generic.h"

#include <string.h>
#include <stdlib.h>

#define VARINT_MASK (1 << 7)

/**
 * checks the length of a multihash for validity
 * @param len the length of the multihash
 * @returns errors or MH_E_NO_ERROR(0)
 */
static int check_len(size_t len) {
	if (len < 1)
		return MH_E_TOO_SHORT;
	else if (len >= 128)
		return MH_E_TOO_LONG;

	return MH_E_NO_ERROR;
}

/**
 * do some general checks on the multihash for validity
 * @param mh the multihash
 * @param len the length of the multihash
 * @returns errors or MH_E_NO_ERROR(0)
 */
static int check_multihash(const unsigned char mh[], size_t len) {
	int err;

	if (len < 3)
		return MH_E_TOO_SHORT;

	if (mh[0] & VARINT_MASK) {
		// This value is a varint, but there are currently no supported
		// values that require more than a single byte to represent.
		return MH_E_VARINT_NOT_SUPPORTED;
	} else if (mh[1] & VARINT_MASK) {
		return MH_E_VARINT_NOT_SUPPORTED;
	}

	err = check_len(mh[1]);

	return err;
}


/**
 * returns hash code or error (which is < 0)
 * @param mh the multihash
 * @param len the length of the multihash
 * @returns errors ( < 0 ) or the multihash
 */
int mh_multihash_hash(const unsigned char *mh, size_t len) {
	int err = check_multihash(mh, len);
	if (err)
		return err;

	return (int) mh[0];
}


/***
 * returns the length of the multihash's data section
 * @param mh the multihash
 * @param len the length of the multihash
 * @returns the length of the data section, or an error if < 0
 */
int mh_multihash_length(const unsigned char *mh, size_t len) {
	int err = check_multihash(mh, len);
	if (err)
		return err;

	return (int) mh[1];
}

/**
 * gives access to raw digest inside multihash buffer
 * @param multihash the multihash
 * @param len the length
 * @param digest the results
 * @returns error if less than zero, otherwise 0
 */
int mh_multihash_digest(const unsigned char *multihash, size_t len, unsigned char **digest,
		size_t *digest_len) {
	int err = check_multihash(multihash, len);
	if (err)
		return err;

	(*digest_len) = (size_t) mh_multihash_length(multihash, len);
	(*digest) = (unsigned char*)multihash + 2; // Always true without varint

	return 0;
}

/**
 * determine the size of the multihash given the data size
 * @param code currently not used
 * @param hash_len the data size
 * @returns hash_len + 2 (until the code parameter (varint) is added
 */
int mh_new_length(int code, size_t hash_len) {
	// right now there is no varint support
	// so length required is 2 + hash_len
	UNUSED(code);
	return 2 + hash_len;
}

/***
 * create a multihash based on some data
 * @param buffer where to put the multihash
 * @param code the code
 * @param digest the data within the multihash
 * @returns error (if < 0) or 0
 */
int mh_new(unsigned char* buffer, int code, const unsigned char *digest,
	size_t digest_len) {
	if (code & VARINT_MASK)
		return MH_E_VARINT_NOT_SUPPORTED;
	if (digest_len > 127)
		return MH_E_DIGSET_TOO_LONG;

	buffer[0] = (unsigned char) ((unsigned int) code) & 255;
	buffer[1] = (unsigned char) digest_len;
	memcpy(buffer + 2, digest, digest_len);

	return 0;
}

