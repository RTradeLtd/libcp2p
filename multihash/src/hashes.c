/***
 * Some helpers in identifying hashes
 */

#include <stdlib.h>

#include "mh/hashes.h"

#include "mh/errors.h"
#include "mh/generic.h"


static const struct hash_info {
	int hash;
	const char *name;
	int length;
} hash_infos[] = {
	{ MH_H_SHA1, "sha1", 20},
	{ MH_H_SHA2_256, "sha2-256", 32 },
	{ MH_H_SHA2_512, "sha2-512", 64 },
	{ MH_H_SHA3_512, "sha3-512", 64 },
	{ MH_H_SHA3_384, "sha3-384", 45 },
	{ MH_H_SHA3_256, "sha3-256", 32 },
	{ MH_H_SHA3_224, "sha3-224", 28 },
	{ MH_H_SHAKE_128, "shake-128", 16 },
	{ MH_H_SHAKE_256, "shake-256", 32 },
	{ MH_H_BLAKE2B, "blake2b", 64 },
	{ MH_H_BLAKE2S, "blake2s", 32 }
};

mh_assert_static(sizeof(hash_infos) / sizeof(hash_infos[0]) == MH_H_COUNT);

/**
 * Given the id, return a struct that shows the id, name, and default length
 * @param hash the id, such as MH_H_SHA1
 * @returns a hash_info struct that has an int, const char* and int
 */
static const struct hash_info *find_hash(int hash) {
	// naive search, could be replaced with binary
	unsigned int i = 0;
	for (; i < MH_H_COUNT; i++) {
		if (hash_infos[i].hash == hash)
			return &hash_infos[i];
	}

	return NULL;
}

/**
 * Given the id, return the hash name
 * @param hash the id (such as MH_H_SHA1)
 * @returns the name as text, such as "sha1"
 */
const char *mh_hash_name(int hash) {
	const struct hash_info *info = find_hash(hash);
	return (info != NULL) ? info->name : NULL;
}

/**
 * Given the id, return the default length
 * @param hash the id
 * @returns the default length of that hash
 */
int mh_hash_default_length(int hash) {
	const struct hash_info *info = find_hash(hash);
	return (info != NULL) ? info->length : MH_E_UNKNOWN_CODE;
}
