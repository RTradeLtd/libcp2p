
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

#include <string.h>
#include "crypto/ecdsa.h"
#include "crypto/util.h"

// you can change this to your needs
#define ECPARAMS MBEDTLS_ECP_DP_SECP256R1

struct EcdsaPublicKey {
    unsigned char *somedata;
    size_t length;
};

/*! @brief used to generate an ECDSA keypair
  * @param private_key the place to store the private key
  * @returns Fail: 0
  * @returns Success: 1
*/
int libp2p_crypto_ecdsa_generation_keypair(struct EcdsaPrivateKey *private_key) {
    mbedtls_pk_context ecdsa_key_pair;
    mbedtls_entropy_context entropy_context;
    mbedtls_ctr_drbg_context ctr_drb_context;
    const char *pers = "ecdsa";

    mbedtls_ctr_drbg_init(&ctr_drb_context);
    mbedtls_entropy_init(&entropy_context);
    mbedtls_pk_init(&ecdsa_key_pair);
    mbedtls_pk_setup(&ecdsa_key_pair, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    
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
    /*rc = mbedtls_ecp_gen_key(
        MBEDTLS_ECP_DP_BP384R1,
        mbedtls_pk_ec(ecdsa_key_pair),
        mbedtls_ctr_drbg_random,
        &ctr_drb_context    
    );*/
    rc = mbedtls_ecdsa_genkey(
        mbedtls_pk_ec(ecdsa_key_pair),
        ECPARAMS,
        mbedtls_ctr_drbg_random,
        &ctr_drb_context

    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return 0;
    }
    unsigned char *outbuf = malloc(sizeof(unsigned char) * 1024);
    rc = mbedtls_pk_write_key_pem(&ecdsa_key_pair, outbuf, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return 0;
    }
    printf("%lu\n", strlen((char *)outbuf));
    return 1;
    /*
    mbedtls_ecdsa_context ecdsa_sign_context, ecdsa_verify_context;
    mbedtls_entropy_context entropy_context;
    mbedtls_ctr_drbg_context ctr_drb_context;

    // TODO(bonedaddy): determine whether or not we should change this
    const char *pers = "ecdsa";
    // initalize mbedtls structs
    mbedtls_ctr_drbg_init(&ctr_drb_context);
    mbedtls_entropy_init(&entropy_context);

    int rc = mbedtls_ctr_drbg_seed(
        &ctr_drb_context,
        mbedtls_entropy_func,
        &entropy_context,
        (const unsigned char *)pers,
        strlen(pers) != 0
    );
    if (rc != 0) {
        // TODO(bonedaddy): free up memory
        return 0;
    }

    rc = mbedtls_ecdsa_genkey(
        &ecdsa_sign_context,
        ECPARAMS,
        mbedtls_ctr_drbg_random,
        &ctr_drb_context
    );
    if (rc != 0) {
        // TODO(bonedaddy): free up memory
        return 0;
    }
    unsigned char *buf = malloc(sizeof(unsigned char) * 1024);
    mbedtls_pk_write_key_pem(&ecdsa_sign_context, buf, sizeof(unsigned char) * 1024);

    return 0;
    */
}