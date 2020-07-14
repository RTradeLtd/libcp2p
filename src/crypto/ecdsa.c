
// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/ecdsa.h"
// mbedtls stuff
#include "mbedtls/asn1write.h"
#include "mbedtls/bignum.h"
#include "mbedtls/config.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"
#include "mbedtls/platform.h"
#include "mbedtls/x509.h"
#include "mbedtls/error.h"
#include <pthread.h>
#include <string.h>
#include "crypto/ecdsa.h"
#include "crypto/util.h"


/*!
  * @brief frees up resources allocated for the private key
*/
int libp2p_crypto_ecdsa_free(ecdsa_private_key_t *pk) {
    mbedtls_pk_free(&pk->pk_ctx);
    mbedtls_ecdsa_free(&pk->ecdsa_ctx);
    pthread_mutex_destroy(&pk->mutex);
    free(pk);
    return 0;
}

/*! 
  * @brief returns the public key associated with the private key
  * @return the public key in PEM format
*/
unsigned char *libp2p_crypto_ecdsa_keypair_public(ecdsa_private_key_t *pk) {
    unsigned char output_buf[1024];
    int rc = mbedtls_pk_write_pubkey_pem(
        &pk->pk_ctx,
        output_buf,
        1024
    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }
    unsigned char *public_key = malloc(sizeof(unsigned char) * strlen((char *)output_buf) + 1);
    strcpy(
        (char *)public_key,
        (char *)output_buf
    );
    return public_key;
}

/*!
  * @brief parses a PEM encoded private key and returns a struct for use
  * @details the returned mbedtls_*_context in the struct are not suitable for concurrent use, please access through mutex locks
  * @param pem_input the PEM encoded ECDSA private key
  * @return an initialize and populated ecdsa_private_key_t
*/
ecdsa_private_key_t *libp2p_crypto_ecdsa_pem_to_private_key(unsigned char *pem_input) {
    mbedtls_pk_context pk_context;
    mbedtls_ecdsa_context ecdsa_context;

    mbedtls_pk_init(&pk_context);
    mbedtls_ecdsa_init(&ecdsa_context);

    int rc = mbedtls_pk_parse_key(
        &pk_context,
        pem_input,
        strlen((char *)pem_input) + 1,
        NULL,
        0
    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }

    rc = mbedtls_ecdsa_from_keypair(
        &ecdsa_context, 
        mbedtls_pk_ec(pk_context)
    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }
    ecdsa_private_key_t *pk = malloc(
        sizeof(ecdsa_private_key_t) + sizeof(pk_context) + sizeof(ecdsa_context)
    );
    pk->ecdsa_ctx = ecdsa_context;
    pk->pk_ctx = pk_context;
    pthread_mutex_init(&pk->mutex, NULL);

    return pk;
}

/*! @brief used to generate an ECDSA keypair
  * @param private_key the place to store the private key
  * @param curve the ECC curve to use for key generation
  * @returns Fail: 0
  * @returns Success: 1
*/
int libp2p_crypto_ecdsa_keypair_generation(unsigned char *output, mbedtls_ecp_group_id curve) {
    mbedtls_pk_context ecdsa_key_pair;
    mbedtls_entropy_context entropy_context;
    mbedtls_ctr_drbg_context ctr_drb_context;
    /*! * @todo figure out if we need this
    */
    const char *pers = "ecdsa";

    // initialize mbedtls context(s)
    mbedtls_ctr_drbg_init(&ctr_drb_context);
    mbedtls_entropy_init(&entropy_context);
    mbedtls_pk_init(&ecdsa_key_pair);
    mbedtls_pk_setup(&ecdsa_key_pair, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));

    // seed entropy generation
    int rc = mbedtls_ctr_drbg_seed(
        &ctr_drb_context,
        mbedtls_entropy_func,
        &entropy_context,
        (const unsigned char *)pers,
        strlen(pers) != 0
    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        // TODO(bonedaddy): free up memory
        return 0;
    }

    // generate the actual ecdsa keypair
    rc = mbedtls_ecdsa_genkey(
        mbedtls_pk_ec(ecdsa_key_pair),
        curve,
        mbedtls_ctr_drbg_random,
        &ctr_drb_context

    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return 0;
    }

    // write the private key in PEM format to output
    rc = mbedtls_pk_write_key_pem(&ecdsa_key_pair, output, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return 0;
    }

    // free up allocated resources
    mbedtls_pk_free(&ecdsa_key_pair);
    mbedtls_ctr_drbg_free(&ctr_drb_context);
    mbedtls_entropy_free(&entropy_context);
    return 1;
}