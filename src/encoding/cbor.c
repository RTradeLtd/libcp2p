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

/*!
  * @brief fills `buffer` with the data to send
  * @details if the total size needed is greater than max_len the call fails
  * @return Success: 0
  * @return Failure: -1
*/
int get_encoded_send_buffer(cbor_encoded_data_t *msg, unsigned char *buffer, size_t max_len) {
    if (msg->len + 1 > max_len || max_len == 0) {
        return -1;
    }
    buffer[0] = msg->len;
    memcpy(buffer + 1, msg->data, max_len - 1); // max_len -1  because 1 byte for the size
    return 0;
}
/*!
  * @brief gets the size of a buffer needed for sending cbor_encoded_data_t
  * @return Success: size of buffer
  * @return Failure: -1
*/
size_t get_encoded_send_buffer_len(cbor_encoded_data_t *msg) {
    /*!
      * @todo sizeof(size_t) is 8 bytes, maybe we dont need this?
      * @todo in theory we only need 1 byte to store the value of size_t
    */
    return msg->len + 1;
}