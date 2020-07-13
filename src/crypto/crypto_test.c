#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include "crypto/sha512.h"
#include "crypto/encoding/base64.h"

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

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_libp2p_crypto_hashing_sha512)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}