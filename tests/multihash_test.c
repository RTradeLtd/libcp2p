#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include "multihash/errors.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include "encoding/base64.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

/*
    examples from https://github.com/raduiliescu83/c-multihash/commit/e9847bb8520ae554a7ddfaa47b17577b30d0d56f
*/

unsigned char sha1_example [] = {
    MH_H_SHA1,
    20,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d};
size_t sha1_example_length = 22;

unsigned char sha2_example [] = {
    MH_H_SHA2_256,
    32,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x00, 0x01};
size_t sha2_example_length = 22;

unsigned char sha3_example [] = {
    MH_H_SHA3_512,
    64,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x11, 0x0a, 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x00, 0x01, 0x02, 0x03};
size_t sha3_example_length = 34;

unsigned char random_512 [64];

void init_random_512(void) {
    int i;
    for (i = 0; i<64;i++) {
        random_512[i] = (unsigned char) rand();
    }
}

void test_bin_to_64(void **state) {
    unsigned char *p = malloc(sizeof(unsigned char) * 256);
    size_t len;
    int rc = libp2p_encoding_base64_encode(
        "123456781234567812345678",
        strlen("123456781234567812345678"),
        p,
        256,
        &len
    );
    assert(rc == 1);
    printf("%s\n", p);
    assert(
        strcmp(
            p, "MTIzNDU2NzgxMjM0NTY3ODEyMzQ1Njc4"
        ) == 0
    );
    free(p);
}

void test_error_messages_exist(void **state) {
    int i = -1;
    for (; i > MH_E_LAST; i--) {
        assert(
            strlen(mh_error_string(i)) > 0
        );
        assert(
            strcmp(
               mh_error_string(-10000), mh_error_string(i)
            ) != 0
        );
    }
}

void test_all_hashes_have_names(void **state) {
    int i = 0;
    for (; i < MH_H_COUNT; i++) {
        assert(
            mh_hash_name(mh_all_hashes[i]) != NULL
        );
    }   
}

void test_name_is_null_when_out_of_bands(void **state) {
    assert(
        mh_hash_name(0) == NULL
    );
}

void test_all_hashes_have_lengths(void **state) {
    int i = 0;
    int length = 0;
    for (; i < MH_H_COUNT; i++) {
        length = mh_hash_default_length(mh_all_hashes[i]);
        assert(
            length > 0
        );
    }    
}

void test_error_when_out_of_bands(void **state) {
    assert(
        mh_hash_default_length(1 << 20) == MH_E_UNKNOWN_CODE
    );
}

void test_lengths_are_correct_for_known_codes(void **state) {
#define hlen mh_hash_default_length
    assert(
        hlen(MH_H_SHA1) == 20
    );
    assert(
        hlen(MH_H_SHA3_256) == 32
    );
    assert(
        hlen(MH_H_SHAKE_128) == 16
    );
#undef hlen    
}


void test_multihash_new_is_reversible(void **state) {
    int error = MH_E_NO_ERROR;
    int code = MH_H_SHA3_512;
    const unsigned char *digest = random_512;
    const size_t digest_len = 512 / 8;

    unsigned char mh[256];
    const size_t mh_len = mh_new_length(code, digest_len);

    error = mh_new(mh, MH_H_SHA3_512, digest, digest_len);
    assert(error == MH_E_NO_ERROR);

    assert(mh_multihash_hash(mh, mh_len) == MH_H_SHA3_512);
    assert(mh_multihash_length(mh, mh_len) == (int)digest_len);
}

void test_multihash_hash_decoding_works(void **state) {
    // TODO(bonedaddy): add SHA1
     assert(mh_multihash_hash(sha2_example, sizeof(sha2_example)) ==
                  MH_H_SHA2_256);
     assert(mh_multihash_hash(sha3_example, sizeof(sha3_example)) ==
                  MH_H_SHA3_512);
}

void test_multihash_length_decoding_works(void **state) {
     assert(mh_multihash_length(sha1_example, sizeof(sha1_example)) ==
                  mh_hash_default_length(MH_H_SHA1));
     assert(mh_multihash_length(sha2_example, sizeof(sha2_example)) ==
                  mh_hash_default_length(MH_H_SHA2_256));
     assert(mh_multihash_length(sha3_example, sizeof(sha3_example)) ==
                  mh_hash_default_length(MH_H_SHA3_512));
}


void test_multihash_new_crafts_right_multihash(void **state) {
    int error;
    unsigned char buf[256]; // much bigger than needed
    size_t digest_len = -1;
    const unsigned char *digest = NULL;

    error = mh_multihash_digest(sha1_example, sha1_example_length, &digest,
                                &digest_len);
    assert(error == MH_E_NO_ERROR);

    error = mh_new(buf, MH_H_SHA1, digest, digest_len);
    assert(error == MH_E_NO_ERROR);
    assert(memcmp(sha1_example, buf, sha1_example_length) == 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bin_to_64),
        cmocka_unit_test(test_error_messages_exist),
        cmocka_unit_test(test_all_hashes_have_names),
        cmocka_unit_test(test_name_is_null_when_out_of_bands),
        cmocka_unit_test(test_all_hashes_have_lengths),
        cmocka_unit_test(test_error_when_out_of_bands),
        cmocka_unit_test(test_lengths_are_correct_for_known_codes),
        cmocka_unit_test(test_multihash_new_is_reversible),
        cmocka_unit_test(test_multihash_hash_decoding_works),
        cmocka_unit_test(test_multihash_length_decoding_works),
        cmocka_unit_test(test_multihash_new_crafts_right_multihash)
        
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
