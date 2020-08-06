#include "testutils/testutils.h"
#include "crypto/ecdsa.h"
#include <assert.h>
#include <mbedtls/ecp.h>
#include <stdlib.h>

ecdsa_private_key_t *new_ecdsa_private_key(void) {
    ecdsa_private_key_t *pk =
        libp2p_crypto_ecdsa_keypair_generation(MBEDTLS_ECP_DP_SECP256R1);
    assert(pk != NULL);
    return pk;
}