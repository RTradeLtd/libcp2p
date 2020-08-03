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

#pragma once

#include "crypto/key.h"
#include "crypto/peerutils.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/pk.h"
#include <pthread.h>

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

/*!
 * @brief used to save a private key at the given path
 * @param pk an instance of ecdsa_private_key_t
 * @param path the location on disk to save file at
 * @return Success: 0
 * @return Failure: 1
 */
int libp2p_crypto_ecdsa_private_key_save(ecdsa_private_key_t *pk, char *path);

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
 * @brief returns the public key associated with the private key in PEM format
 * @details the returned struct is suitable for encoding into CBOR and sending to
 * peers
 * @warning caller must free returned data when no longer
 * @todo this currently relies in usage of `str..` we should use `mem...` instead
 * @return Success: an instance of public_key_t with the corresponding information
 * @return Failure: NULL pointer
 */
public_key_t *libp2p_crypto_ecdsa_keypair_public(ecdsa_private_key_t *pk);

/*! @brief returns a peer_id_t struct for the given private key
 * @details this is useful for exchanging peer identifier information with
 * @details anyone who connects to our host
 * @warning caller must free returned pointer when no longer needed
 * @details to get the peerid we take a sha256 hash of the public key file in PEM
 * format
 * @details we then generate a multihash of that sha256, and base58 encode it
 * @todo this currently relies on hard coded assumptions about hashing algorithm
 * @todo in the future this will change
 * @param pk a loaded ecdsa_private_key_t instance
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_crypto_ecdsa_keypair_peerid(ecdsa_private_key_t *pk);

/*!
 * @brief frees up resources allocated for the private key
 */
int libp2p_crypto_ecdsa_free(ecdsa_private_key_t *pk);

/*!
 * @brief loads an ECDSA private key from a file containing a PEM key
 * @param path the path on disk to a PEM file containing an ECDSA private key
 * @return Success: pointer to an instance of an ecdsa_private_key_t type
 * @return Failure: NULL pointer
 */
ecdsa_private_key_t *libp2p_crypto_ecdsa_private_key_from_file(char *path);