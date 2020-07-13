# NOTE

The following tests need to be added because upstream didnt have `examples.h` commited

#include "minunit.h"
#include "multihash/multihash.h"
#include <string.h>
/*!

* @note need to figure out how we can add these tests
* i checked upstream and couldn't find `examples.h`

#include "multihash/hashes.h"

// #include "examples.h"

char error_buf[256];

static char *test_multihash_hash_decoding_works(void) {
    mu_assert("sha1 multihash is detected as sha1",
             ;
    mu_assert("sha2 multihash is detected as sha2",
              mh_multihash_hash(sha2_example, sizeof(sha2_example)) ==
                  MH_H_SHA2_256);
    mu_assert("sha3 multihash is detected as sha3",
              mh_multihash_hash(sha3_example, sizeof(sha3_example)) ==
                  MH_H_SHA3_512);
    return NULL;
}

static char *test_multihash_length_decoding_works(void) {
    mu_assert("sha1 multihash has correct length",
              mh_multihash_length(sha1_example, sizeof(sha1_example)) ==
                  mh_hash_default_length(MH_H_SHA1));
    mu_assert("sha2 multihash has correct length",
              mh_multihash_length(sha2_example, sizeof(sha2_example)) ==
                  mh_hash_default_length(MH_H_SHA2_256));
    mu_assert("sha3-512 multihash has correct length",
              mh_multihash_length(sha3_example, sizeof(sha3_example)) ==
                  mh_hash_default_length(MH_H_SHA3_512));
    return NULL;
}

static char *mu_all_tests(void) {
    mu_run_test(test_multihash_hash_decoding_works);
    mu_run_test(test_multihash_length_decoding_works);
    return NULL;
}

char error_buf[256];

static char *test_multihash_new_crafts_right_multihash(void) {
    int error;
    unsigned char buf[256]; // much bigger than needed
    size_t digest_len = -1;
    const unsigned char *digest = NULL;

    error = mh_multihash_digest(sha1_example, sha1_example_length, &digest,
                                &digest_len);
    mu_assert("getting digest", error == MH_E_NO_ERROR);

    error = mh_new(buf, MH_H_SHA1, digest, digest_len);
    mu_assert("creating multihash", error == MH_E_NO_ERROR);

    mu_assert("crafted multihash is the same",
              memcmp(sha1_example, buf, sha1_example_length) == 0);

    return NULL;
}

static char *test_multihash_new_is_reversible(void) {
    int error = MH_E_NO_ERROR;
    int code = MH_H_SHA3_512;
    const unsigned char *digest = random_512;
    const size_t digest_len = 512 / 8;

    unsigned char mh[256];
    const size_t mh_len = mh_new_length(code, digest_len);

    error = mh_new(mh, MH_H_SHA3_512, digest, digest_len);
    mu_assert("creating multihash", error == MH_E_NO_ERROR);

    mu_assert("reading code", mh_multihash_hash(mh, mh_len) == MH_H_SHA3_512);
    mu_assert("reading length",
              mh_multihash_length(mh, mh_len) == (int)digest_len);

    return NULL;
}

static char *mu_all_tests(void) {
    mu_run_test(test_multihash_new_crafts_right_multihash);
    mu_run_test(test_multihash_new_is_reversible);
    return NULL;
}

*/