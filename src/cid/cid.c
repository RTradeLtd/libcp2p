/**
 * Content ID
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cid/cid.h"
#include "encoding/base32.h"
#include "multibase/multibase.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include "protobuf/varint.h"

enum WireType ipfs_cid_message_fields[] = {WIRETYPE_VARINT, WIRETYPE_VARINT,
                                           WIRETYPE_LENGTH_DELIMITED};

size_t ipfs_cid_protobuf_encode_size(const struct Cid *cid) {
    if (cid != NULL)
        return 11 + 12 + cid->hash_length + 11;
    return 0;
}

int ipfs_cid_protobuf_encode(const struct Cid *cid, unsigned char *buffer,
                             size_t buffer_length, size_t *bytes_written) {
    size_t bytes_used;
    *bytes_written = 0;
    int retVal = 0;
    if (cid != NULL) {
        retVal = protobuf_encode_varint(1, ipfs_cid_message_fields[0], cid->version,
                                        buffer, buffer_length, &bytes_used);
        if (retVal == 0)
            return 0;
        *bytes_written += bytes_used;
        retVal = protobuf_encode_varint(
            2, ipfs_cid_message_fields[1], cid->codec, &buffer[*bytes_written],
            buffer_length - (*bytes_written), &bytes_used);
        if (retVal == 0)
            return 0;
        *bytes_written += bytes_used;
        retVal = protobuf_encode_length_delimited(
            3, ipfs_cid_message_fields[2], (char *)cid->hash, cid->hash_length,
            &buffer[*bytes_written], buffer_length - (*bytes_written), &bytes_used);
        if (retVal == 0)
            return 0;
        *bytes_written += bytes_used;
    }
    return 1;
}

int ipfs_cid_protobuf_decode(unsigned char *buffer, size_t buffer_length,
                             struct Cid **output) {
    // short cut for nulls
    if (buffer_length == 0) {
        *output = NULL;
        return 1;
    }

    size_t pos = 0;
    int version = 0;
    unsigned char *hash;
    size_t hash_length;
    char codec = 0;
    int retVal = 0;

    while (pos < buffer_length) {
        size_t bytes_read = 0;
        int field_no;
        enum WireType field_type;
        if (protobuf_decode_field_and_type(&buffer[pos], buffer_length, &field_no,
                                           &field_type, &bytes_read) == 0) {
            return 0;
        }
        pos += bytes_read;
        switch (field_no) {
            case (1):
                version =
                    varint_decode(&buffer[pos], buffer_length - pos, &bytes_read);
                pos += bytes_read;
                break;
            case (2):
                codec =
                    varint_decode(&buffer[pos], buffer_length - pos, &bytes_read);
                pos += bytes_read;
                break;
            case (3):
                retVal = protobuf_decode_length_delimited(
                    &buffer[pos], buffer_length - pos, (char **)&hash, &hash_length,
                    &bytes_read);
                if (retVal == 0)
                    return 0;
                pos += bytes_read;
                break;
        }
    }

    *output = ipfs_cid_new(version, hash, hash_length, codec);
    retVal = *output != NULL;
    free(hash);
    return retVal;
}

/**
 * Create a new CID based on the given hash
 * @param version the version
 * @param hash the multihash
 * @param hash_length the length of the multihash in bytes
 * @param codec the codec to be used (NOTE: For version 0, this should be
 * CID_DAG_PROTOBUF)
 * @returns the new Cid or NULL if there was a problem
 */
struct Cid *ipfs_cid_new(int version, const unsigned char *hash, size_t hash_length,
                         const char codec) {
    struct Cid *cid = (struct Cid *)malloc(sizeof(struct Cid));
    if (cid != NULL) {
        cid->hash_length = hash_length;
        if (hash_length == 0 || hash == NULL) {
            cid->hash = NULL;
        } else {
            cid->hash = (unsigned char *)malloc(sizeof(unsigned char) * hash_length);
            if (cid->hash == NULL) {
                free(cid);
                return NULL;
            }
            memcpy(cid->hash, hash, hash_length);
        }
        // assign other values
        cid->version = version;
        cid->codec = codec;
    }
    return cid;
}

/***
 * Free the resources from a Cid
 * @param cid the struct
 * @returns 1
 */
int ipfs_cid_free(struct Cid *cid) {
    if (cid != NULL) {
        if (cid->hash != NULL) {
            free(cid->hash);
            cid->hash = NULL;
        }
        free(cid);
    }
    return 1;
}

/***
 * Make a copy of a Cid
 * @param original the original
 * @returns a copy of the original
 */
struct Cid *ipfs_cid_copy(const struct Cid *original) {
    struct Cid *copy = (struct Cid *)malloc(sizeof(struct Cid));
    if (copy != NULL) {
        copy->codec = original->codec;
        copy->version = original->version;
        copy->hash_length = original->hash_length;
        copy->hash = (unsigned char *)malloc(original->hash_length);
        if (copy->hash == NULL) {
            ipfs_cid_free(copy);
            return NULL;
        }
        memcpy(copy->hash, original->hash, original->hash_length);
    }
    return copy;
}

/***
 * Create a CID from an ipfs or ipns string (i.e. "/ipns/QmAb12CD..."
 * @param incoming the incoming string
 * @param cid the resultant Cid
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_cid_decode_hash_from_ipfs_ipns_string(const char *incoming,
                                               struct Cid **cid) {
    if (incoming == NULL)
        return 0;
    if (strstr(incoming, "/ipfs/") != incoming &&
        strstr(incoming, "/ipns/") != incoming)
        return 0;
    const char *base32 = &incoming[6];
    char *slash = strstr(incoming, "/");
    if (slash != NULL)
        slash[0] = '\0';
    return ipfs_cid_decode_hash_from_base32((unsigned char *)base32, strlen(base32),
                                            cid);
}

/***
 * Fill a Cid struct based on a base 58 encoded multihash
 * @param incoming the string
 * @param incoming_size the size of the string
 * @cid the Cid struct to fill
 * @return true(1) on success
 */
int ipfs_cid_decode_hash_from_base32(const unsigned char *incoming,
                                     size_t incoming_length, struct Cid **cid) {
    int retVal = 0;

    if (incoming_length < 2)
        return 0;

    // is this a sha_256 multihash?
    if (incoming_length == 46 && incoming[0] == 'Q' && incoming[1] == 'm') {
        size_t hash_length = libp2p_encoding_base32_decode_size(incoming_length);
        unsigned char hash[hash_length];
        unsigned char *ptr = hash;
        retVal = libp2p_encoding_base32_decode(incoming, incoming_length, ptr,
                                               &hash_length);
        if (retVal == 0)
            return 0;
        // now we have the hash, build the object
        *cid = ipfs_cid_new(0, &hash[2], hash_length - 2, CID_DAG_PROTOBUF);
        return *cid != NULL;
    }

    // TODO: finish this
    /*
    // it wasn't a sha_256 multihash, try to decode it using multibase
    size_t buffer_size = multibase_decode_size(incoming_length);
    if (buffer_size == 0)
            return 0;
    unsigned char buffer[buffer_size];

    memset(buffer, 0, buffer_size);

    retVal = multibase_decode(incoming, incoming_length, buffer, buffer_size,
    &buffer_size); if (retVal == 0) return 0;

    return cid_cast(buffer, buffer_size, cid);
    */
    return 0;
}

/**
 * Turn a cid hash into a base 58
 * @param hash the hash to work with
 * @param hash_length the length of the existing hash
 * @param buffer where to put the results
 * @param max_buffer_length the maximum space reserved for the results
 * @returns true(1) on success
 */
int ipfs_cid_hash_to_base32(const unsigned char *hash, size_t hash_length,
                            unsigned char *buffer, size_t max_buffer_length) {

    int multihash_len = hash_length + 2;
    unsigned char multihash[multihash_len];
    if (mh_new(multihash, MH_H_SHA2_256, hash, hash_length) < 0) {
        return 0;
    }

    // base32
    size_t b32_size = libp2p_encoding_base32_encode_size(multihash_len);

    if (b32_size > max_buffer_length) // buffer too small
        return 0;

    if (libp2p_encoding_base32_encode(multihash, multihash_len, buffer,
                                      &max_buffer_length) == 0) {
        return 0;
    }

    return 1;
}

/***
 * Turn the hash of this CID into a c string
 * @param cid the cid
 * @param result a place to allocate and store the string
 * @returns a pointer to the string (*result) or NULL if there was a problem
 */
char *ipfs_cid_to_string(const struct Cid *cid, char **result) {
    size_t str_len = libp2p_encoding_base32_encode_size(cid->hash_length) + 1;
    char *str = (char *)malloc(str_len);
    *result = str;
    if (str != NULL) {
        if (!libp2p_encoding_base32_encode(cid->hash, cid->hash_length,
                                           (unsigned char *)str, &str_len)) {
            free(str);
            str = NULL;
        }
    }
    return str;
}

/***
 * Turn a multibase decoded string of bytes into a Cid struct
 * @param incoming the multibase decoded array
 * @param incoming_size the size of the array
 * @param cid the Cid structure to fill
 */
int ipfs_cid_cast(const unsigned char *incoming, size_t incoming_size,
                  struct Cid *cid) {
    if (incoming_size == 34 && incoming[0] == 18 && incoming[1] == 32) {
        // this is a multihash
        cid->hash_length = mh_multihash_length(incoming, incoming_size);
        cid->codec = CID_DAG_PROTOBUF;
        cid->version = 0;

        mh_multihash_digest(incoming, incoming_size, &cid->hash, &cid->hash_length);
        return 1;
    }

    // This is not a multihash. Perhaps it is using varints. Try to peel the
    // information out of the bytes. first the version
    int pos = 0;
    size_t num_bytes = 0;
    cid->version = varint_decode(&incoming[pos], incoming_size - pos, &num_bytes);
    if (num_bytes == 0 || cid->version > 1 || cid->version < 0)
        return 0;
    pos = num_bytes;
    // now the codec
    uint32_t codec = 0;
    codec = varint_decode(&incoming[pos], incoming_size - pos, &num_bytes);
    if (num_bytes == 0)
        return 0;
    cid->codec = codec;
    pos += num_bytes;
    // now what is left
    cid->hash_length = incoming_size - pos;
    cid->hash = (unsigned char *)(&incoming[pos]);

    return 1;
}

/**
 * Compare two cids
 *
 * TODO: find a common denominator between versions and codecs so that
 * we can compare apples to apples.
 *
 * @param a side A
 * @param b side B
 * @returns < 0 if side A is greater, > 0 if side B is greater, or 0 if equal
 */
int ipfs_cid_compare(const struct Cid *a, const struct Cid *b) {
    if (a == NULL && b == NULL)
        return 0;
    if (a != NULL && b == NULL)
        return -1;
    if (a == NULL && b != NULL)
        return 1;

    if (a->version != b->version) {
        return b->version - a->version;
    }
    if (a->codec != b->codec) {
        return ((int)b->codec - (int)a->codec);
    }
    if (a->hash_length != b->hash_length) {
        return b->hash_length - a->hash_length;
    }
    for (size_t i = 0; i < a->hash_length; i++) {
        if (a->hash[i] != b->hash[i]) {
            return ((int)b->hash[i] - (int)a->hash[i]);
        }
    }
    return 0;
}
