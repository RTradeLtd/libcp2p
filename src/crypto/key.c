#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "crypto/key.h"
#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "protobuf/protobuf.h"
#include <tinycbor/cbor.h>

/**
 * Utilities for public and private keys
 */

public_key_t *libp2p_crypto_public_key_new(void) {
    public_key_t *retVal = malloc(sizeof(public_key_t));
    if (retVal == NULL)
        return NULL;
    retVal->type = KEYTYPE_INVALID;
    retVal->data = NULL;
    retVal->data_size = 0;
    return retVal;
}

void libp2p_crypto_public_key_free(public_key_t *in) {
    free(in->data);
    free(in);
}


/***
 * Calculates an approximate required size of a buffer for protobuf encoding a
 * public key
 * @param in the public key to examine
 * @returns the size in bytes
 */
size_t libp2p_crypto_public_key_protobuf_encode_size(const public_key_t *in) {
    return 11 + 11 + in->data_size;
}

/***
 * Encode a PublicKey into a protobuf
 * @param in the public_key_t
 * @param buffer where to put the results
 * @param max_buffer_length the size of the buffer
 * @param bytes_written how many bytes were used in the buffer
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_public_key_protobuf_encode(const public_key_t *in,
                                             unsigned char *buffer,
                                             size_t max_buffer_length,
                                             size_t *bytes_written) {
    // data & data_size
    size_t bytes_used = 0;
    *bytes_written = 0;
    int retVal = 0;
    // key type (RSA vs ...)
    retVal =
        protobuf_encode_varint(1, WIRETYPE_VARINT, in->type, &buffer[*bytes_written],
                               max_buffer_length - *bytes_written, &bytes_used);
    *bytes_written += bytes_used;
    // public key
    retVal = protobuf_encode_length_delimited(
        2, WIRETYPE_LENGTH_DELIMITED, (char *)in->data, in->data_size,
        &buffer[*bytes_written], max_buffer_length - *bytes_written, &bytes_used);
    if (retVal == 0)
        return 0;
    *bytes_written += bytes_used;
    return 1;
}

/**
 * Unmarshal a public key from a protobuf
 * @param buffer the protobuf
 * @param buffer_length the length of the protobuf
 * @param out the pointer to the public_key_t that will be allocated
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_public_key_protobuf_decode(unsigned char *buffer,
                                             size_t buffer_length,
                                             public_key_t **out) {

    // first field is type (RSA vs ED25519)
    // second field is the public key

    size_t pos = 0;
    int retVal = 0;

    if ((*out = libp2p_crypto_public_key_new()) == NULL)
        goto exit;

    while (pos < buffer_length) {
        size_t bytes_read = 0;
        int field_no;
        enum WireType field_type;
        if (protobuf_decode_field_and_type(&buffer[pos], buffer_length, &field_no,
                                           &field_type, &bytes_read) == 0) {
            goto exit;
        }
        pos += bytes_read;
        switch (field_no) {
            case (1): // type
                if (protobuf_decode_varint(&buffer[pos], buffer_length - pos,
                                           (long long unsigned int *)&((*out)->type),
                                           &bytes_read) == 0)
                    goto exit;
                pos += bytes_read;
                break;
            case (2): // key
                if (protobuf_decode_length_delimited(
                        &buffer[pos], buffer_length - pos, (char **)&((*out)->data),
                        &((*out)->data_size), &bytes_read) == 0)
                    goto exit;
                pos += bytes_read;
                break;
        }
    }

    retVal = 1;

exit:
    if (retVal == 0) {
        libp2p_crypto_public_key_free(*out);
    }
    return retVal;
}

private_key_t *libp2p_crypto_private_key_new(void) {
    private_key_t *retVal = malloc(sizeof(private_key_t));
    if (retVal == NULL)
        return NULL;
    retVal->type = KEYTYPE_INVALID;
    retVal->data = NULL;
    retVal->data_size = 0;
    return retVal;
}

void libp2p_crypto_private_key_free(private_key_t *in) {
    if (in != NULL) {
        if (in->data != NULL)
            free(in->data);
        free(in);
        in = NULL;
    }
}

int libp2p_crypto_private_key_copy(const private_key_t *source,
                                   private_key_t *destination) {
    if (source != NULL && destination != NULL) {
        destination->type = source->type;
        destination->data = (unsigned char *)malloc(source->data_size);
        if (destination->data != NULL) {
            memcpy(destination->data, source->data, source->data_size);
            destination->data_size = source->data_size;
            return 1;
        }
        libp2p_crypto_private_key_free(destination);
    }
    return 0;
}

size_t libp2p_crypto_private_key_protobuf_encode_size(const private_key_t *in) {
    return 22 + in->data_size;
}

int libp2p_crypto_private_key_protobuf_encode(const private_key_t *in,
                                              unsigned char *buffer,
                                              size_t max_buffer_length,
                                              size_t *bytes_written) {
    *bytes_written = 0;
    size_t bytes_used;
    // type (RSA vs ED25519)
    if (!protobuf_encode_varint(1, WIRETYPE_VARINT, in->type,
                                &buffer[*bytes_written],
                                max_buffer_length - *bytes_written, &bytes_used))
        return 0;
    *bytes_written += bytes_used;
    // private key
    if (!protobuf_encode_length_delimited(
            2, WIRETYPE_LENGTH_DELIMITED, (char *)in->data, in->data_size,
            &buffer[*bytes_written], max_buffer_length - *bytes_written,
            &bytes_used))
        return 0;
    *bytes_written += bytes_used;
    return 1;
}

/**
 * Unmarshal a private key from a protobuf
 * @param buffer the protobuf
 * @param buffer_length the length of the protobuf
 * @param out the pointer to the private_key_t that will be allocated
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_private_key_protobuf_decode(unsigned char *buffer,
                                              size_t buffer_length,
                                              private_key_t **out) {

    // first field is type (RSA vs ED25519)
    // second field is the public key

    size_t pos = 0;
    int retVal = 0;

    if ((*out = libp2p_crypto_private_key_new()) == NULL)
        goto exit;

    while (pos < buffer_length) {
        size_t bytes_read = 0;
        int field_no;
        enum WireType field_type;
        if (protobuf_decode_field_and_type(&buffer[pos], buffer_length, &field_no,
                                           &field_type, &bytes_read) == 0) {
            goto exit;
        }
        pos += bytes_read;
        switch (field_no) {
            case (1): // type
                if (protobuf_decode_varint(&buffer[pos], buffer_length - pos,
                                           (long long unsigned int *)&((*out)->type),
                                           &bytes_read) == 0)
                    goto exit;
                pos += bytes_read;
                break;
            case (2): // key
                if (protobuf_decode_length_delimited(
                        &buffer[pos], buffer_length - pos, (char **)&((*out)->data),
                        &((*out)->data_size), &bytes_read) == 0)
                    goto exit;
                pos += bytes_read;
                break;
        }
    }

    retVal = 1;

exit:
    if (retVal == 0) {
        libp2p_crypto_private_key_free(*out);
    }
    return retVal;
}

/**
 * convert a public key into a peer id
 * @param public_key the public key struct
 * @param peer_id the results, in a null-terminated string
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_public_key_to_peer_id(public_key_t *public_key,
                                        char *peer_id) {

    /**
     * Converting to a peer id involves protobufing the public_key_t,
     * SHA256 it, turn it into a MultiHash and base58 it
     */
    size_t protobuf_len = libp2p_crypto_public_key_protobuf_encode_size(public_key);
    unsigned char protobuf[protobuf_len];

    libp2p_crypto_public_key_protobuf_encode(public_key, protobuf, protobuf_len,
                                             &protobuf_len);

    unsigned char hashed[32];
    // libp2p_crypto_hashing_sha256(public_key->data, public_key->data_size,
    // hashed);
    libp2p_crypto_hashing_sha256(protobuf, protobuf_len, hashed);
    size_t final_id_size = 100;
    unsigned char final_id[final_id_size];
    memset(final_id, 0, final_id_size);
    // turn it into a multihash and base58 it
    if (!libp2p_new_peer_id(final_id, &final_id_size, hashed, 32))
        return 0;
    memset(peer_id, 0, final_id_size + 1);
    memcpy(peer_id, final_id, final_id_size);
    return 1;
}

/*!
  * @brief used to cbor decode a uint8_t pointer and return a public_key_t object
*/
public_key_t *libp2p_crypto_public_key_cbor_decode(
    uint8_t *buffer
) {
    size_t buffer_len = sizeof(buffer);
    printf("%lu\n", buffer_len);
    return NULL;
}

/*!
  * @brief used to cbor encode a public_key_t object
*/
uint8_t *libp2p_crypto_public_key_cbor_encode(
    public_key_t *pub_key,
    size_t *bytes_written
) {
    
    uint8_t buf[pub_key->data_size + sizeof(pub_key)];
    CborEncoder encoder, map_encoder;
    CborError err;

    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);

    /*!
      * @brief we Use CborIdefiniteLength to work around an error
      * @brief the error is "too few items added to container" when closing the container
    */
    err = cbor_encoder_create_map(&encoder, &map_encoder, CborIndefiniteLength);
    if (err != CborNoError) {
        printf("failed to create map\n");
        return NULL;
    }
    
    err = cbor_encode_simple_value(&map_encoder, pub_key->type);
    if (err != CborNoError) {
        printf("failed to encode simple values\n");
        return NULL;
    }

    err = cbor_encode_byte_string(&map_encoder, pub_key->data, pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encode byte string\n");
        return NULL;
    }

    err = cbor_encode_int(&map_encoder, (int64_t)pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encdoe int\n");
        return NULL;
    }

    err = cbor_encoder_close_container(&encoder, &map_encoder);
    if (err != CborNoError) {
        printf("failed to close container: %s\n", cbor_error_string(err));
        return NULL;
    }
    
    size_t size = cbor_encoder_get_buffer_size(&encoder, buf);
    *bytes_written = size;
    uint8_t *out = calloc(sizeof(uint8_t), size);
    memcpy(out, buf, size);
    return out;
}