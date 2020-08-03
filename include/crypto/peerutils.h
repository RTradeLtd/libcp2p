/*! @file peerutils.h
 * @brief provides utilities for managing peer identifiers
 */

#pragma once

#include "crypto/sha256.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include "stdio.h"
#include "string.h"
#include <stdio.h>

/*!
 * @struct peer_id
 * @typedef peer_id_t
 * @brief a structure containing a peer identifier
 */
typedef struct peer_id {
    /*! @brief the actual peer_id data */
    unsigned char *data;
    /*! @brief the size of the data member */
    size_t len;
} peer_id_t;


/*!
 * @brief generates a SHA256 based PeerID
 * @details is a wrapper around libp2p_new_peer_id that abstracts away hashing and memory allocs
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param public_key the public key in PEM format to hash
 * @param public_key_len the length of the public key
 * @return Success: 1
 * @return Failure: 0
 */
peer_id_t *libp2p_new_peer_id_sha256(unsigned char *output, size_t *output_len,
                              unsigned char *public_key, size_t public_key_len);

/**
 * base58 encode a string NOTE: this also puts the prefix 'Qm' in front as the
 * ID is a multihash
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param input_hash the input public key hash, usually sha256
 * @param input_size the input size, usually 32 bytes
 * @return Success: 1
 * @return Failure: 0
 */
peer_id_t *libp2p_new_peer_id(unsigned char *output, size_t *output_len,
                       unsigned char *input_hash, size_t input_size);

/*!
  * @brief free up resources allocated for an instance of peer_id_t
*/
void libp2p_peer_id_free(peer_id_t *pid);