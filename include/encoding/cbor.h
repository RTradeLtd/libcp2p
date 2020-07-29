/*!
 * @file cbor.h
 * @brief utilities for working with cbor encoding/decoding
 * @details provides helper functions for working with CBOR encoding/decoding
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/*!
 * @struct cbor_encoded_data
 * @typedef cbor_encoded_data_t
 * @brief wraps data returned from encoding with tinycbor
 */
typedef struct cbor_encoded_data {
    uint8_t *data; /*! @brief the actual cbor encoded data */
    size_t len;    /*! @brief the length of the cbor encoded data */
} cbor_encoded_data_t;

/*!
 * @brief free up resources associated with an instance of cbor_encoded_t
 * @param in pointer to an instance of cbor_encoded_data_t
 */
void free_cbor_encoded_data(cbor_encoded_data_t *in);

/*!
 * @brief allocates a chunk of memory for a new cbor_encoded_data_t instance
 * @param data pointer to the data we are copying into the new struct
 * @param len size of the data we are copying in
 * @return Success: pointer to an instance of a new cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *new_cbor_encoded_data(uint8_t *data, size_t len);

/*!
 * @brief fills `buffer` with the data to send
 * @details if the total size needed is greater than max_len the call fails
 * @return Success: 0
 * @return Failure: -1
 */
int get_encoded_send_buffer(cbor_encoded_data_t *msg, unsigned char *buffer,
                            size_t max_len);

/*!
 * @brief gets the size of a buffer needed for sending cbor_encoded_data_t
 * @return Success: size of buffer
 * @return Failure: -1
 */
size_t get_encoded_send_buffer_len(cbor_encoded_data_t *msg);
