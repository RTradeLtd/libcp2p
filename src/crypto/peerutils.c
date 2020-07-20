#include <stdlib.h>
#include <string.h>

#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "encoding/base58.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include "multibase/multibase.h"

/**
 * @brief returns a libp2p peerid from the sha256 hash of a public key
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
                       size_t ID_BUF_SIZE) // b58 encoded ID buf
{
    unsigned char temp_buffer[*rezbuflen];
    memset(temp_buffer, 0, *rezbuflen);

    // wrap the base58 into a multihash
    int retVal = mh_new(temp_buffer, MH_H_SHA2_256, ID_BUF, ID_BUF_SIZE);
    if (retVal < 0)
        return 0;

    return multibase_encode(
        MULTIBASE_BASE58_BTC,
        temp_buffer,
        ID_BUF_SIZE + 2, // 2 (1 for code, 1 for digest_len)
        pointyaddr,
        *rezbuflen,
        rezbuflen
    );
}

/****
 * Make a SHA256 hash of what is usually the DER formatted private key.
 * @param result where to store the result. Should be 32 chars long
 * @param texttohash the text to hash. A DER formatted public key
 * @param text_size the size of the text
 */
/*
void ID_FromPK_non_null_terminated(char * result,unsigned char * texttohash,
size_t text_size)
{

        libp2p_crypto_hashing_sha256(texttohash, text_size, (unsigned
char*)result);
}
*/

/****
 * Make a SHA256 hash of what is usually the DER formatted private key.
 * @param result where to store the result. Should be 32 chars long
 * @param texttohash a null terminated string of the text to hash
 */
/*
void ID_FromPK(char * result,unsigned char * texttohash)
{
   ID_FromPK_non_null_terminated(result,texttohash,strlen((char*)texttohash));
}
*/
