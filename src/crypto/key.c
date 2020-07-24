/*! @file key.c
 * @brief public/private key management functions
 * @details provides tooling for working with public/private keys and transmitting
 * them over the wire
 * @warning none of these functions and structures are thread safe
 */

#include "crypto/key.h"
#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "protobuf/protobuf.h"
#include "tinycbor/cbor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @brief returns a new instance of public_key_t
 * @returns Success: pointer to an instance of public_key_t
 * @returns Failure: NULL pointer
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

/*!
 * @brief free up resources associated with a public_key_t instance
 */
void libp2p_crypto_public_key_free(public_key_t *in) {
    free(in->data);
    free(in);
}

/*!
 * @brief returns a new instance of private_key_t
 * @returns Success: pointer to an instance of private_key_t
 * @returns Failure: NULL pointer
 */
private_key_t *libp2p_crypto_private_key_new(void) {
    private_key_t *retVal = malloc(sizeof(private_key_t));
    if (retVal == NULL)
        return NULL;
    retVal->type = KEYTYPE_INVALID;
    retVal->data = NULL;
    retVal->data_size = 0;
    return retVal;
}

/*!
 * @brief frees up resources associated with a private_key_t instance
 */
void libp2p_crypto_private_key_free(private_key_t *in) {
    if (in != NULL) {
        if (in->data != NULL)
            free(in->data);
        free(in);
        in = NULL;
    }
}

/*!
 * @brief copies a private key
 * @param source the key we are copying
 * @param destination the location to copy to
 * @return Success: 0
 * @return Failure: 1
 * @note does not allocate memory for caller
 */
int libp2p_crypto_private_key_copy(const private_key_t *source,
                                   private_key_t *destination) {
    if (source != NULL && destination != NULL) {
        destination->type = source->type;
        destination->data = (unsigned char *)malloc(source->data_size);
        if (destination->data == NULL) {
            return 1;
        }
        memcpy(destination->data, source->data, source->data_size);
        destination->data_size = source->data_size;
        return 0;
    }
    return 1;
}

/*!
 * @brief used to cbor data a chunk of data
 * @param data an instance of cbor_encoded_data_t created with
 * libp2p_crypto_public_key_cbor_encode
 * @returns Success: pointer to a public_key_t instance
 * @returns Failure: NULL pointer
 */
public_key_t *libp2p_crypto_public_key_cbor_decode(cbor_encoded_data_t *data) {
    CborParser parser;
    CborValue value;
    CborError err;

    err = cbor_parser_init(data->data, data->len, 0, &parser, &value);
    if (err != CborNoError) {
        printf("failed to init parser: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_array = cbor_value_is_array(&value);
    if (is_array == false) {
        return NULL;
    }

    CborValue recurse;
    err = cbor_value_enter_container(&value, &recurse);
    if (err != CborNoError) {
        printf("failed to enter container: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_simple = cbor_value_is_simple_type(&recurse);
    if (is_simple == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    uint8_t key_type;
    err = cbor_value_get_simple_type(&recurse, &key_type);
    if (err != CborNoError) {
        printf("failed to get simple value: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance_fixed(&recurse);
    if (err != CborNoError) {
        printf("failed to advanced iter: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_byte = cbor_value_is_byte_string(&recurse);
    if (is_byte == false) {
        printf("unexpected value encounterd\n");
        return NULL;
    }

    size_t data_len;
    err = cbor_value_get_string_length(&recurse, &data_len);
    if (err != CborNoError) {
        printf("failed to get string length: %s\n", cbor_error_string(err));
        return NULL;
    }
    uint8_t data_buffer[data_len];
    err = cbor_value_copy_byte_string(&recurse, data_buffer, &data_len, &recurse);
    if (err != CborNoError) {
        printf("failed to copy byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_int = cbor_value_is_integer(&recurse);
    if (is_int == false) {
        printf("unexpected value encountered wanted integer\n");
        return NULL;
    }

    int64_t key_size;
    err = cbor_value_get_int64(&recurse, &key_size);
    if (err != CborNoError) {
        printf("failed to get int: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance_fixed(&recurse);
    if (err != CborNoError) {
        printf("failed to advanced container: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_leave_container(&value, &recurse);
    if (err != CborNoError) {
        printf("failed to leave container: %s\n", cbor_error_string(err));
        return NULL;
    }

    public_key_t *pk = calloc(sizeof(public_key_t), sizeof(public_key_t));
    if (pk == NULL) {
        return NULL;
    }
    pk->data = calloc(sizeof(unsigned char), data_len);
    if (pk->data == NULL) {
        free(pk);
        return NULL;
    }
    memcpy(pk->data, data_buffer, data_len);
    pk->data_size = data_len;
    pk->type = (key_type_t)key_type;

    return pk;
}

/*!
 * @brief used to cbor encode a public_key_t type for sending over the wire
 * @param pub_key an instance of public_key_t fully filled out
 * @param bytes_written returns the number of bytes written
 * @return Success: pointer to an instance of cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *libp2p_crypto_public_key_cbor_encode(public_key_t *pub_key,
                                                          size_t *bytes_written) {

    uint8_t buf[pub_key->data_size + sizeof(pub_key) + 3];
    CborEncoder encoder, array_encoder;
    CborError err;

    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);
    err = cbor_encoder_create_array(&encoder, &array_encoder, 3);
    if (err != CborNoError) {
        printf("failed to create map: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_simple_value(&array_encoder, pub_key->type);
    if (err != CborNoError) {
        printf("failed to encode simple values: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_byte_string(&array_encoder, pub_key->data, pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encode byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_int(&array_encoder, (int64_t)pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encdoe int: %s\n", cbor_error_string(err));
        return NULL;
    }

    /*!
     * @todo figure out why this returns an error
     */
    err = cbor_encoder_close_container(&encoder, &array_encoder);
    if (err != CborNoError) {
        printf("failed to close container: %s\n", cbor_error_string(err));
        return NULL;
    }

    size_t size = cbor_encoder_get_buffer_size(&encoder, buf);
    *bytes_written = size;
    uint8_t *out = calloc(sizeof(uint8_t), size);
    if (out == NULL) {
        return NULL;
    }
    memcpy(out, buf, size);
    cbor_encoded_data_t *cbdata =
        calloc(sizeof(cbor_encoded_data_t), sizeof(cbor_encoded_data_t));
    if (cbdata == NULL) {
        free(out);
        return NULL;
    }
    cbdata->data = out;
    cbdata->len = size;
    return cbdata;
}