#include <string.h>
#include "minunit.h"

#include "mh/errors.h"
#include "mh/hashes.h"

char error_buf[256];

static char *test_all_hashes_have_lengths(void) {
	int i = 0;
	int length = 0;
	for (; i < MH_H_COUNT; i++) {
		length = mh_hash_default_length(mh_all_hashes[i]);
		sprintf(error_buf, "mh_hash_default_length: hash %d"
				" returned invalid (%d) default length",
				i, length);
		mu_assert(error_buf, length > 0);
	}
	return NULL;
}

static char *test_error_when_out_of_bands(void) {
	mu_assert("out of bands index should give error",
			mh_hash_default_length(1 << 20) == MH_E_UNKNOWN_CODE);
	return NULL;
}

static char *test_lengths_are_correct_for_known_codes(void) {
#define hlen mh_hash_default_length
	mu_assert("sha1 has valid length", hlen(MH_H_SHA1) == 20);
	mu_assert("sha3-512 has valid length", hlen(MH_H_SHA3_256) == 32);
	mu_assert("shake-128 has valid length", hlen(MH_H_SHAKE_128) == 16);

#undef hlen
	return NULL;
}



static char *mu_all_tests(void) {
	mu_run_test(test_all_hashes_have_lengths);
	mu_run_test(test_error_when_out_of_bands);
	mu_run_test(test_lengths_are_correct_for_known_codes);
	return NULL;
}

