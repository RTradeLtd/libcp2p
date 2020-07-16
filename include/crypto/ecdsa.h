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

/*! @file ecdsa.h
 * @brief provides ECDSA cryptography support
 */

#include "mbedtls/ecdsa.h"
#include "mbedtls/pk.h"
#include <pthread.h>

typedef struct ecdsa_public_key {
    unsigned char *data;
    int len;
} ecdsa_public_key_t;

/*! @struct ecdsa_private_key
 * @typedef ecdsa_private_key_t
 * @brief wraps mbedtls contexts
 * @warning the contexts are not thread safe, so you'll need to guard access using
 * the mutex
 */
typedef struct ecdsa_private_key {
    mbedtls_pk_context pk_ctx;
    mbedtls_ecdsa_context ecdsa_ctx;
    pthread_mutex_t mutex;
} ecdsa_private_key_t;

/*! @brief used to generate an ECDSA keypair
 * @param output a buffer to write the ecdsa keypair into
 * @returns Fail: 0
 * @returns Success: 1
 */
int libp2p_crypto_ecdsa_keypair_generation(unsigned char *output,
                                           mbedtls_ecp_group_id curve);

/*!
 * @brief parses a PEM encoded private key and returns a struct for use
 * @details the returned mbedtls_*_context in the struct are not suitable for
 * concurrent use, please access through mutex locks
 * @param pem_input the PEM encoded ECDSA private key
 * @return an initialize and populated ecdsa_private_key_t
 */
ecdsa_private_key_t *
libp2p_crypto_ecdsa_pem_to_private_key(unsigned char *pem_input);

/*!
 * @brief returns the public key associated with the private key
 * @param output a buffer to write the key into, ~256 bytes should suffice
 * @return 0 on success, otherwise returns the mbedtls errno
 */
ecdsa_public_key_t *libp2p_crypto_ecdsa_keypair_public(ecdsa_private_key_t *pk);

/*! @brief returns the peerid for the corresponding private key
 * @warning caller must free returned pointer when no longer needed
 * @details to get the peerid we take a sha256 hash of the public key file in PEM
 * format
 * @details we then generate a multihash of that sha256, and base58 encode it
 * @param pk a loaded ecdsa_private_key_t instance
 * @return pointer to an unsigned char peerID
 */
unsigned char *libp2p_crypto_ecdsa_keypair_peerid(ecdsa_private_key_t *pk);

/*!
 * @brief frees up resources allocated for the private key
 */
int libp2p_crypto_ecdsa_free(ecdsa_private_key_t *pk);