#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "multibase/multibase.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include <stdlib.h>
#include <string.h>


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
                              unsigned char *public_key, size_t public_key_len) {

    unsigned char temp_hash_output[32];
    memset(temp_hash_output, 0, 1024);

    int rc = libp2p_crypto_hashing_sha256(public_key, public_key_len, temp_hash_output);
    if (rc != 1) {
        return 0;
    }

    return libp2p_new_peer_id(output, output_len, temp_hash_output, 32);
}

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
                       unsigned char *input_hash, size_t input_size) {
    unsigned char temp_buffer[*output_len];
    memset(temp_buffer, 0, *output_len);

    // wrap the base58 into a multihash
    int retVal = mh_new(temp_buffer, MH_H_SHA2_256, input_hash, input_size);
    if (retVal < 0)
        return 0;

    retVal = multibase_encode(MULTIBASE_BASE32, temp_buffer,
                            input_size + 2, // 2 (1 for code, 1 for digest_len)
                            output, *output_len, output_len);

    if (retVal != 1) {
        return 0;
    }

    peer_id_t *pid = calloc(1, sizeof(peer_id_t));
    if (pid == NULL) {
        return NULL;
    }

    pid->data = output;
    pid->len = *output_len;

    return pid;
}