#include <string.h>
#include "minunit.h"

#include "mh/errors.h"
#include "mh/hashes.h"

char error_buf[256];

static char *test_all_hashes_have_names(void) {
	int i = 0;
	for (; i < MH_H_COUNT; i++) {
		sprintf(error_buf, "hash code %d does not have name mapping",
				i);
		mu_assert(error_buf, mh_hash_name(mh_all_hashes[i]) != NULL);
	}
	return NULL;
}

static char *test_name_is_null_when_out_of_bands(void) {
	mu_assert("hash code out of range does not have name",
			mh_hash_name(0) == NULL);
	return NULL;
}

static char *mu_all_tests(void) {
	mu_run_test(test_name_is_null_when_out_of_bands);
	mu_run_test(test_all_hashes_have_names);
	return NULL;
}

