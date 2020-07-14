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
#include "crypto/sha1.h"
#include "crypto/ecdsa.h"
#include "crypto/encoding/base64.h"
#include "mbedtls/base64.h"
#include "crypto/peerutils.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

// also tests getting the associated public key
void test_libp2p_crypto_ecdsa_keypair_generation(void **state) {
    unsigned char *output = malloc(sizeof(unsigned char) * 1024);
    int rc = libp2p_crypto_ecdsa_keypair_generation(output, MBEDTLS_ECP_DP_SECP256R1);
    assert(rc == 1);
    assert(strlen((char *)output) > 0);
    printf("%s\n", output);
    ecdsa_private_key_t *pk = libp2p_crypto_ecdsa_pem_to_private_key(output);
    assert(pk != NULL);

    unsigned char *output2 = malloc(sizeof(unsigned char) * 1024);
    rc = mbedtls_pk_write_key_pem(&pk->pk_ctx, output2, 1024);
    assert(rc == 0);
    assert(
        strcmp(
            (char *)output2,
            (char *)output
        ) == 0
    );
    unsigned char *public_key = libp2p_crypto_ecdsa_keypair_public(pk);
    assert(public_key != NULL);
    printf("%s\n", public_key);
    free(public_key);
    rc = libp2p_crypto_ecdsa_free(pk);
    assert(rc == 0);
}

void test_libp2p_crypto_hashing_sha1_hmac(void **state) {
    mbedtls_sha1_context ctx;
    int rc = libp2p_crypto_hashing_sha1_init(&ctx);
    assert(rc == 1);
    uint8_t msg[] ="this is a test message";
    rc = libp2p_crypto_hashing_sha1_update(
        &ctx,
        msg,
        strlen((char *)msg)
    );
    assert(rc == 1);
    uint8_t *output = malloc(sizeof(uint8_t) * 32 + 1);
    rc = libp2p_crypto_hashing_sha1_finish(
        &ctx,
        output
    );
    assert(strlen((char *)output) == 20);
    assert(rc == 1);

    uint8_t *base64_encoded = malloc(sizeof(uint8_t) * 64 + 1);
    size_t len;
    rc = libp2p_crypto_encoding_base64_encode(
        output,
        strlen((char *)output),
        base64_encoded,
        sizeof(uint8_t) * 64 + 1,
        &len
    );
    assert(rc == 1);
    assert(
        strcmp(
            (char *)base64_encoded,
            "Hsoholv7PQXA8Y39TT9JTsIz7XE="
        ) == 0
    );

    uint8_t *base64_decoded = malloc(sizeof(uint8_t) * 32 + 1);
    rc = libp2p_crypto_encoding_base64_decode(
        base64_encoded,
        strlen((char *)base64_encoded),
        base64_decoded,
        sizeof(uint8_t) * 32 + 1,
        &len
    );
    assert(rc == 1);
    assert(
        memcmp(
            base64_decoded,
            output,
            strlen((char *)output)
        ) == 0
    );
    rc = libp2p_crypto_hashing_sha1_free(&ctx);
    assert(rc == 1);
}

void test_libp2p_crypto_hashing_sha256_hmac(void **state) {
    mbedtls_sha256_context ctx;
    int rc = libp2p_crypto_hashing_sha256_init(&ctx);
    assert(rc == 1);
    uint8_t msg[] ="this is a test message";
    rc = libp2p_crypto_hashing_sha256_update(
        &ctx,
        msg,
        strlen((char *)msg)
    );
    assert(rc == 1);
    uint8_t *output = malloc(sizeof(uint8_t) * 32 + 1);
    rc = libp2p_crypto_hashing_sha256_finish(
        &ctx,
        output
    );
    assert(strlen((char *)output) == 32);
    assert(rc == 1);

    uint8_t *base64_encoded = malloc(sizeof(uint8_t) * 64 + 1);
    size_t len;
    rc = libp2p_crypto_encoding_base64_encode(
        output,
        strlen((char *)output),
        base64_encoded,
        sizeof(uint8_t) * 64 + 1,
        &len
    );
    assert(rc == 1);
    assert( 
        strcmp(
            (char *)base64_encoded,
            "aVxNnFJUUB6I3NU+pXFhcmCRHAO8XLGuxQswxnVk7f0="
        ) == 0
    );

    uint8_t *base64_decoded = malloc(sizeof(uint8_t) * 32 + 1);
    rc = libp2p_crypto_encoding_base64_decode(
        base64_encoded,
        strlen((char *)base64_encoded),
        base64_decoded,
        sizeof(uint8_t) * 32 + 1,
        &len
    );
    assert(rc == 1);
    assert(
        memcmp(
            base64_decoded,
            output,
            strlen((char *)output)
        ) == 0
    );
    rc = libp2p_crypto_hashing_sha256_free(&ctx);
    assert(rc == 1);
}

void test_libp2p_crypto_hashing_sha512(void **state) {
    uint8_t input[] = "some input text";
    uint8_t *output = malloc(sizeof(uint8_t) * 256);
    int rc = libp2p_crypto_hashing_sha512(
        input, strlen((char *)input),
        output
    );
    assert(rc == 64);
    assert(strlen((char *)output) == 64);
    uint8_t *base64_output = malloc(sizeof(unsigned char) * 512);
    size_t len;
    rc = libp2p_crypto_encoding_base64_encode(
        output,
        strlen((char *)output),
        base64_output,
        512,
        &len
    );
    assert(rc == 1);
    assert(
        strcmp(
            (char *)base64_output,
            "/3SZKSbv6ZRZfi/hTh3q3nSyr4XrexgicQqQF56fsG5M1YFSc2OdbXcsmVxFdFSZTKz9sOKJt2Z7YfYlSlIKGg=="
        ) == 0
    );
    uint8_t *base64_decode = malloc(sizeof(unsigned char) * 512);
    rc = libp2p_crypto_encoding_base64_decode(
        base64_output,
        strlen((char *)base64_output),
        base64_decode,
        512,
        &len
    );
    assert(rc == 1);
    assert(
        memcmp(
            base64_decode,
            output,
            strlen((char *)output)
        ) == 0
    );
    assert(
        strcmp(
            (char *)base64_decode,
            (char *)output
        ) == 0
    );
}

void test_libp2p_crypto_hashing_sha256(void **state) {
    uint8_t input[] = "some input text";
    uint8_t *output = malloc(sizeof(uint8_t) * 256);
    // unsigned char *input = (unsigned char *)"some input text";
    // unsigned char *output = malloc(sizeof(unsigned char *) * 256 + 1);
    int rc = libp2p_crypto_hashing_sha256(
        input, strlen((char *)input),
        output
    );
    assert(rc);
    assert(strlen((char *)output) == 32);
    uint8_t *base64_output = malloc(sizeof(uint8_t) * 512);
    size_t len;

    rc = libp2p_crypto_encoding_base64_encode(
        output, 
        strlen((char *)output), 
        base64_output, 
        512, 
        &len
    );
    assert(rc == 1);
    assert(strcmp((char *)base64_output, "ELK1zDN90JmPWKbSaORqMQ1B17AJHbDNx5DrcINHY28=") == 0);

    uint8_t *decode_output = malloc(sizeof(uint8_t) * 512);
    rc = libp2p_crypto_encoding_base64_decode(
        base64_output,
        strlen((char *)base64_output),
        decode_output,
        512,
        &len
    );
    assert(rc == 1);
    assert(
        memcmp(
            decode_output,
            output,
            strlen((char *)output)
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
        cmocka_unit_test(test_libp2p_crypto_hashing_sha256_hmac),
        cmocka_unit_test(test_libp2p_crypto_hashing_sha1_hmac),
        cmocka_unit_test(test_libp2p_crypto_ecdsa_keypair_generation)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}