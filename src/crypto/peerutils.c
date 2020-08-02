#include <stdlib.h>
#include <string.h>
#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "multibase/multibase.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"

/**
 * base58 encode a string NOTE: this also puts the prefix 'Qm' in front as the
 * ID is a multihash
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to store size of buffer
 * @param input_hash the input public key hash, usually sha256
 * @param input_size the input size, usually 32 bytes
 * @return Success: 1
 * @return Failure: 0
 */
int libp2p_new_peer_id(unsigned char *output, size_t *output_len,
                       unsigned char *input_hash,
                       size_t input_size)
{
    unsigned char temp_buffer[*output_len];
    memset(temp_buffer, 0, *output_len);

    // wrap the base58 into a multihash
    int retVal = mh_new(temp_buffer, MH_H_SHA2_256, input_hash, input_size);
    if (retVal < 0)
        return 0;

    return multibase_encode(MULTIBASE_BASE32, temp_buffer,
                            input_size + 2, // 2 (1 for code, 1 for digest_len)
                            output, *output_len, output_len);
}