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
 * @brief contains the peer identifier (base encoded multihash of public key
 * @note not thread safe, should we add locking capabilities?
 */
typedef struct peer_id {
    /*! @brief the actual peer_id data */
    unsigned char *data;
    /*! @brief the size of the data member */
    size_t len;
} peer_id_t;

/*!
 * @brief generates a peerID for the public key using SHA256 as the hashing algorithm
 * @details is a wrapper around libp2p_new_peer_id that abstracts away hashing and
 * memory allocs and stuff like that
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param public_key the public key in PEM format to hash
 * @param public_key_len the length of the public key
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_new_peer_id_sha256(unsigned char *output, size_t *output_len,
                                     unsigned char *public_key,
                                     size_t public_key_len);

/**
 * @brief constructs a peer identifier from the given public key
 * @details this is responsible for taking the hash of a public key
 * @details turning it into a multihash, and the base encoding it
 * @note is currently hard coded for the input hash being SHA256
 * @note if you allocated memory for output param you can free it up after this
 * function returns
 * @note this is because we copy the data placed there into the returned struct
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param input_hash the input public key hash, usually sha256
 * @param input_size the input size, usually 32 bytes
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_new_peer_id(unsigned char *output, size_t *output_len,
                              unsigned char *input_hash, size_t input_size);

/*!
 * @brief free up resources allocated for an instance of peer_id_t
 */
void libp2p_peer_id_free(peer_id_t *pid);