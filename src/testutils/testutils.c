#include "testutils/testutils.h"
#include "crypto/ecdsa.h"
#include <assert.h>
#include <mbedtls/ecp.h>
#include <stdlib.h>

/*!
 * @brief returns a new PEM formatted ECDSA private key
 * @details no need to check returned value for NULL PTR as we use assert internally
 */
unsigned char *new_ecdsa_private_key_pem(void) {
    unsigned char *output = calloc(1, sizeof(unsigned char) * 1024);
    assert(output != NULL);
    int rc =
        libp2p_crypto_ecdsa_keypair_generation(output, MBEDTLS_ECP_DP_SECP256R1);
    assert(rc == 1);
    return output;
}

ecdsa_private_key_t *new_ecdsa_private_key(void) {
    unsigned char *pemmed = new_ecdsa_private_key_pem();
    ecdsa_private_key_t *pk = libp2p_crypto_ecdsa_pem_to_private_key(pemmed);
    assert(pk != NULL);
    free(pemmed);
    return pk;
}