#include <string.h>
#include "minunit.h"
#include "mh/multihash.h"

#include "mh/hashes.h"

#include "examples.h"

char error_buf[256];

static char *test_multihash_hash_decoding_works(void) {
	mu_assert("sha1 multihash is detected as sha1",
		mh_multihash_hash(sha1_example, sizeof(sha1_example))
		== MH_H_SHA1);
	mu_assert("sha2 multihash is detected as sha2",
		mh_multihash_hash(sha2_example, sizeof(sha2_example))
		== MH_H_SHA2_256);
	mu_assert("sha3 multihash is detected as sha3",
		mh_multihash_hash(sha3_example, sizeof(sha3_example))
		== MH_H_SHA3_512);
	return NULL;
}

static char *test_multihash_length_decoding_works(void) {
	mu_assert("sha1 multihash has correct length",
		mh_multihash_length(sha1_example, sizeof(sha1_example))
		== mh_hash_default_length(MH_H_SHA1));
	mu_assert("sha2 multihash has correct length",
		mh_multihash_length(sha2_example, sizeof(sha2_example))
		== mh_hash_default_length(MH_H_SHA2_256));
	mu_assert("sha3-512 multihash has correct length",
		mh_multihash_length(sha3_example, sizeof(sha3_example))
		== mh_hash_default_length(MH_H_SHA3_512));
	return NULL;
}

static char *mu_all_tests(void) {
	mu_run_test(test_multihash_hash_decoding_works);
	mu_run_test(test_multihash_length_decoding_works);
	return NULL;
}

