#include "crypto/key.h"
#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "protobuf/protobuf.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

/*!
 * @brief used to cbor decode a uint8_t pointer and return a public_key_t object
 */
public_key_t *libp2p_crypto_public_key_cbor_decode(cbor_encoded_data_t *data) {
    CborParser parser;
    CborValue value;
    CborError err;

    cbor_parser_init(data->data, data->len, 0, &parser, &value);

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

    err = cbor_value_advance(&recurse);
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

    err = cbor_value_advance(&recurse);
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
 * @brief used to cbor encode a public_key_t object
 */
cbor_encoded_data_t *libp2p_crypto_public_key_cbor_encode(public_key_t *pub_key,
                                                          size_t *bytes_written) {

    uint8_t buf[pub_key->data_size + sizeof(pub_key) + 3];
    CborEncoder encoder, array_encoder;
    CborError err;

    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);
    err = cbor_encoder_create_array(&encoder, &array_encoder, 3);
    if (err != CborNoError) {
        printf("failed to create map\n");
        return NULL;
    }

    err = cbor_encode_simple_value(&array_encoder, pub_key->type);
    if (err != CborNoError) {
        printf("failed to encode simple values\n");
        return NULL;
    }

    err = cbor_encode_byte_string(&array_encoder, pub_key->data, pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encode byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_int(&array_encoder, (int64_t)pub_key->data_size);
    if (err != CborNoError) {
        printf("failed to encdoe int\n");
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
    memcpy(out, buf, size);
    cbor_encoded_data_t *cbdata =
        calloc(sizeof(cbor_encoded_data_t), sizeof(cbor_encoded_data_t));
    cbdata->data = out;
    cbdata->len = size;
    return cbdata;
}