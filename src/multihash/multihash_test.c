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

void test_bin_to_64(void **state) {
    unsigned char *p = malloc(sizeof(unsigned char) * 256);
    size_t len;
    int rc = libp2p_crypto_encoding_base64_encode(
        "123456781234567812345678",
        strlen("123456781234567812345678"),
        p,
        256,
        &len
    );
    assert(rc == 1);
    assert(
        strcmp(
            p, "BBB="
        ) == 0
    );
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



int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bin_to_64),
        cmocka_unit_test(test_error_messages_exist),
        cmocka_unit_test(test_all_hashes_have_names),
        cmocka_unit_test(test_name_is_null_when_out_of_bands),
        cmocka_unit_test(test_all_hashes_have_lengths),
        cmocka_unit_test(test_error_when_out_of_bands),
        cmocka_unit_test(test_lengths_are_correct_for_known_codes)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
