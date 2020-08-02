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

/**
 * base58 encode a string NOTE: this also puts the prefix 'Qm' in front as the
 * ID is a multihash
 * @param pointyaddr where the results will go
 * @param rezbuflen the length of the results buffer. It will also put how much
 * was used here
 * @param ID_BUF the input text (usually a SHA256 hash)
 * @param ID_BUF_SIZE the input size (normally a SHA256, therefore 32 bytes)
 * @returns true(1) on success
 */
int libp2p_new_peer_id(unsigned char *pointyaddr, size_t *rezbuflen,
                       unsigned char *ID_BUF,
                       size_t ID_BUF_SIZE); // b32 encoded ID buf

/****
 * Make a SHA256 hash of what is usually the DER formatted private key.
 * @param result where to store the result. Should be 32 chars long
 * @param texttohash the text to hash
 * @param text_size the size of the text
 */
void ID_FromPK_non_null_terminated(char *result, unsigned char *texttohash,
                                   size_t text_size);

/****
 * Make a SHA256 hash of what is usually the DER formatted private key.
 * @param result where to store the result. Should be 32 chars long
 * @param texttohash a null terminated string of the text to hash
 */
void ID_FromPK(char *result, unsigned char *texttohash);
