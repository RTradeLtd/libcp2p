#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include "crypto/sha512.h"
#include "crypto/sha256.h"
#include "crypto/encoding/base64.h"
#include "mbedtls/base64.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void test_libp2p_crypto_hashing_sha512(void **state) {
    unsigned char *input = (unsigned char *)"some input text";
    unsigned char *output = malloc(sizeof(unsigned char) * 64 + 1);
    int rc = libp2p_crypto_hashing_sha512(input, strlen((char *)input), output);
    assert(rc == 64);
    unsigned char *base64_output = malloc(sizeof(unsigned char) * 512);
    size_t bytes_written;
    rc = libp2p_crypto_encoding_base64_encode(
        output, 
        sizeof(output),
        base64_output,
        (size_t)(sizeof(unsigned char) * 512), 
        &bytes_written
    );
    assert(rc);
    assert(strcmp((char *)base64_output, "/3SZKSbv6ZQ=") == 0);
    free(output);
    free(base64_output);
}

void test_libp2p_crypto_hashing_sha256(void **state) {
    uint8_t input[] = "some input text";
    uint8_t *output = malloc(sizeof(uint8_t) * 256);
    // unsigned char *input = (unsigned char *)"some input text";
    // unsigned char *output = malloc(sizeof(unsigned char *) * 256 + 1);
    int rc = libp2p_crypto_hashing_sha256(
        input, strlen(input),
        output
    );
    assert(rc);
    assert(strlen((char *)output) == 32);
    uint8_t *base64_output = malloc(sizeof(uint8_t) * 128 + 1);
    size_t len;

    rc = mbedtls_base64_encode(
        base64_output, 
        128, 
        &len, 
        output, 
        strlen((char *)output)
    );
    assert(rc == 0);
    assert(strcmp((char *)base64_output, "ELK1zDN90JmPWKbSaORqMQ1B17AJHbDNx5DrcINHY28=") == 0);

    uint8_t *decode_output = malloc(sizeof(uint8_t) * 256);
    rc = mbedtls_base64_decode(
        decode_output,
        sizeof(uint8_t) * 256,
        &len,
        // to_decode,
        // strlen(to_decode)
        base64_output,
        strlen((char *)base64_output)
    );
    assert(rc == 0);
    assert(
        memcmp(
            decode_output,
            output,
            strlen(output)
        ) == 0
    );
    assert(
        strcmp(
            (char *)decode_output,
            (char *)output
        ) == 0
    );
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_libp2p_crypto_hashing_sha512),
        cmocka_unit_test(test_libp2p_crypto_hashing_sha256),
        cmocka_unit_test(test_libp2p_crypto_hashing_sha256)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}