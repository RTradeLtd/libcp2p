/*!
 * @file cbor.c
 * @brief utilities for working with cbor encoding/decoding
 * @details provides helper functions for working with CBOR encoding/decoding
 */

#include "encoding/cbor.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @brief free up resources associated with an instance of cbor_encoded_t
 * @param in pointer to an instance of cbor_encoded_data_t
 */
void free_cbor_encoded_data(cbor_encoded_data_t *in) {
    free(in->data);
    free(in);
}

/*!
 * @brief allocates a chunk of memory for a new cbor_encoded_data_t instance
 * @param data pointer to the data we are copying into the new struct
 * @param len size of the data we are copying in
 * @return Success: pointer to an instance of a new cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *new_cbor_encoded_data(uint8_t *data, size_t len) {
    cbor_encoded_data_t *out = calloc(1, sizeof(cbor_encoded_data_t));
    if (out == NULL) {
        return NULL;
    }
    out->data = calloc(1, len);
    if (out->data == NULL) {
        free(out);
        return NULL;
    }
    memcpy(out->data, data, len);
    out->len = len;
    return out;
}