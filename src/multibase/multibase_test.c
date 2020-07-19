#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>
#include "multibase/multibase.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void test_multibase_encode_decode_base16(void **state) {
    unsigned char *input = "hello world";
    unsigned char *output = calloc(sizeof(unsigned char), 1024);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE16, 
        input,
        strlen((char *)input),
        output,
        1024,
        &len
    );
    assert(rc == 1);
    // assert that the first element is the base16 identifier
    assert(output[0] == MULTIBASE_BASE16);
    assert(
        strcmp((char *)output, "f68656c6c6f20776f726c64") == 0
    );

    unsigned char *decoded_output = calloc(sizeof(unsigned char), len);
    size_t written;
    rc = multibase_decode(output, len, decoded_output, len, &written);
    assert(rc == 1);
    assert(
        strcmp(
            (char *)input,
            (char *)decoded_output
        ) == 0
    );
}

void test_multibase_encode_decode_base32(void **state) {
    unsigned char *input = "hello world";
    unsigned char *output = calloc(sizeof(unsigned char), 1024);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE32,
        input,
        strlen((char *)input),
        output,
        1024,
        &len
    );
    assert(rc == 1);
    assert(output[0] == MULTIBASE_BASE32);
    assert(
        strcmp(
            (char *)output,
            "bNBSWY3DPEB3W64TMMQ======"
        ) == 0
    );
    unsigned char *decoded_output = calloc(sizeof(unsigned char), len);
    size_t res;
    rc = multibase_decode(
        output,
        len,
        decoded_output,
        len,
        &res
    );
    assert(rc == 1);
    assert(
        strcmp(
            (char *)input,
            (char *)decoded_output
        ) == 0
    );
}

void test_multibase_encode_decode_base58(void **state) {
    unsigned char *input = "hello world";
    unsigned char *output = calloc(sizeof(unsigned char), 1024);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE58_BTC,
        input,
        strlen((char *)input),
        output,
        1024,
        &len
    );
    assert(rc == 1);
    assert(output[0] == MULTIBASE_BASE58_BTC);
    assert(
        strcmp(
            (char *)output,
            "zStV1DL6CwTryKyV"
        ) == 0
    );
    /* base58 decode is broken
    unsigned char *decoded_output = calloc(sizeof(unsigned char), len);
    size_t res;
    rc = multibase_decode(
        output,
        len,
        decoded_output,
        len,
        &res
    );
    printf("%s\n", decoded_output);
    assert(rc == 1);
    assert(
        strcmp(
            (char *)input,
            (char *)decoded_output
        ) == 0
    );*/
}

void test_multibase_encode_decode_base64(void **state) {
    unsigned char *input = "hello world";
    unsigned char *output = calloc(sizeof(unsigned char), 1024);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE64,
        input,
        strlen((char *)input),
        output,
        1024,
        &len
    );
    assert(rc == 1);
    assert(output[0] == MULTIBASE_BASE64);
    assert(
        strcmp(
            (char *)output,
            "maGVsbG8gd29ybGQ="
        ) == 0
    );

    unsigned char *decoded_output = calloc(sizeof(unsigned char), len);
    size_t res;
    rc = multibase_decode(
        output,
        len,
        decoded_output,
        len,
        &res
    );
    assert(rc == 1);
    assert(
        strcmp(
            (char *)input,
            (char *)decoded_output
        ) == 0
    );
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multibase_encode_decode_base16),
        cmocka_unit_test(test_multibase_encode_decode_base32),
        cmocka_unit_test(test_multibase_encode_decode_base58),
        cmocka_unit_test(test_multibase_encode_decode_base64)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}