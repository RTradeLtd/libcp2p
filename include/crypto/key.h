/*! @file key.h
 * @brief public/private key management functions
 * @details provides tooling for working with public/private keys and transmitting
 * them over the wire
 * @warning none of these functions and structures are thread safe
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

/*!
 * @enum key_type_t
 * @brief denotes the type of key
 * @details indicates the type of cryptography used with the key
 */
typedef enum { KEYTYPE_INVALID, KEYTYPE_ECDSA } key_type_t;

/*!
 * @struct cbor_encoded_data
 * @typedef cbor_encoded_data_t
 * @brief wraps data returned from encoding with tinycbor
 */
typedef struct cbor_encoded_data {
    uint8_t *data; /*! @brief the actual cbor encoded data */
    size_t len;    /*! @brief the length of the cbor encoded data */
} cbor_encoded_data_t;

/*! @struct public_key
 * @typedef public_key_t
 * @brief a structure containing information needed for using a public key
 * @note to send over the wire use libp2p_crypto_public_key_cbor_encode
 * @details CBOR wire representation is `array[type, data, size]`
 */
typedef struct public_key {
    key_type_t type;     /*! @brief the type of the key */
    unsigned char *data; /*! @brief the key data in PEM format */
    size_t data_size;    /*! @brief the size of the key data */
} public_key_t;

/*! @struct private_key
 * @typedef private_key_t
 * @brief  a structure containing information needed for using a private key
 */
typedef struct private_key {
    key_type_t type;     /*! @brief the type of the key */
    unsigned char *data; /*! @brief the key data in PEM format */
    size_t data_size;
} private_key_t; /*! @brief the size of the key data */

/*!
 * @brief returns a new instance of public_key_t
 * @returns Success: pointer to an instance of public_key_t
 * @returns Failure: NULL pointer
 */
public_key_t *libp2p_crypto_public_key_new(void);

/*!
 * @brief free up resources associated with a public_key_t instance
 */
void libp2p_crypto_public_key_free(public_key_t *in);

/*!
 * @brief returns a new instance of private_key_t
 * @returns Success: pointer to an instance of private_key_t
 * @returns Failure: NULL pointer
 */
private_key_t *libp2p_crypto_private_key_new(void);

/*!
 * @brief frees up resources associated with a private_key_t instance
 */
void libp2p_crypto_private_key_free(private_key_t *in);

/*!
 * @brief copies a private key
 * @param source the key we are copying
 * @param destination the location to copy to
 * @return Success: 0
 * @return Failure: 1
 * @note does not allocate memory for caller
 */
int libp2p_crypto_private_key_copy(const private_key_t *source,
                                   private_key_t *destination);

/*!
 * @brief used to cbor data a chunk of data
 * @param data an instance of cbor_encoded_data_t created with
 * libp2p_crypto_public_key_cbor_encode
 * @returns Success: pointer to a public_key_t instance
 * @returns Failure: NULL pointer
 */
public_key_t *libp2p_crypto_public_key_cbor_decode(cbor_encoded_data_t *data);

/*!
 * @brief used to cbor encode a public_key_t type for sending over the wire
 * @param pub_key an instance of public_key_t fully filled out
 * @param bytes_written returns the number of bytes written
 * @return Success: pointer to an instance of cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *libp2p_crypto_public_key_cbor_encode(public_key_t *pub_key,
                                                          size_t *bytes_written);