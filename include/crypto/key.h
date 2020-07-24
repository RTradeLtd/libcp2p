#pragma once

#include <stdlib.h>

/**
 * Utilities for public keys
 */

typedef enum { KEYTYPE_INVALID, KEYTYPE_ECDSA } key_type_t;

typedef struct cbor_encoded_data {
    uint8_t *data;
    size_t len;
} cbor_encoded_data_t;

typedef struct public_key {
    key_type_t type;
    unsigned char *data;
    size_t data_size;
} public_key_t;

typedef struct private_key {
    key_type_t type;
    unsigned char *data;
    size_t data_size;
} private_key_t;

public_key_t *libp2p_crypto_public_key_new(void);
void libp2p_crypto_public_key_free(public_key_t *in);

private_key_t *libp2p_crypto_private_key_new(void);
void libp2p_crypto_private_key_free(private_key_t *in);
int libp2p_crypto_private_key_copy(const private_key_t *source,
                                   private_key_t *destination);

/*!
 * @brief used to cbor decode a uint8_t pointer and return a public_key_t object
 */
public_key_t *libp2p_crypto_public_key_cbor_decode(cbor_encoded_data_t *data);

/*!
 * @brief used to cbor encode a public_key_t object
 */
cbor_encoded_data_t *libp2p_crypto_public_key_cbor_encode(public_key_t *pub_key,
                                                          size_t *bytes_written);

/**
 * @brief Unmarshal a public key from a protobuf
 */
int libp2p_crypto_public_key_protobuf_decode(unsigned char *buffer,
                                             size_t buffer_length,
                                             public_key_t **out);

size_t libp2p_crypto_public_key_protobuf_encode_size(const public_key_t *in);

int libp2p_crypto_public_key_protobuf_encode(const public_key_t *in,
                                             unsigned char *buffer,
                                             size_t max_buffer_length,
                                             size_t *bytes_written);
// private key
int libp2p_crypto_private_key_protobuf_decode(unsigned char *buffer,
                                              size_t buffer_length,
                                              private_key_t **out);

size_t libp2p_crypto_private_key_protobuf_encode_size(const private_key_t *in);

int libp2p_crypto_private_key_protobuf_encode(const private_key_t *in,
                                              unsigned char *buffer,
                                              size_t max_buffer_length,
                                              size_t *bytes_written);

/**
 * @brief convert a public key into a peer id
 * @param public_key the public key struct
 * @param peer_id the results, in a null-terminated string
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_public_key_to_peer_id(public_key_t *public_key, char *peer_id);
