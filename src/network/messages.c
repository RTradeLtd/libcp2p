/*! @file messages.c
 * @brief defines message types and tooling for a very minimal RPC framework
 * @note when sending messages anytime you send a new message, you must first send a
 * single byte that indicates the size of the messsage we are sending
 * @note this helps ensure that we can appropriately handle new requests and allocate
 * enough memory
 * @details the message types here are intended to be served as a building block for
 * your own applications
 * @details at a minimum the types here are setup to establish a secure
 * communications channel using ECDSA keys and ECDH key agreement
 */

#include "network/messages.h"
#include "encoding/cbor.h"
#include "tinycbor/cbor.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*!
 * @brief used to cbor encode a message_t instance
 * @param msg pointer to an instance of message_t
 * @return Success: pointer to an instance of cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *cbor_encode_message_t(message_t *msg) {
    uint8_t buf[sizeof(message_t) + msg->len];
    CborEncoder encoder, array_encoder;
    CborError err;

    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);

    err = cbor_encoder_create_array(&encoder, &array_encoder, 3);
    if (err != CborNoError) {
        printf("failed to create array: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_simple_value(&array_encoder, msg->type);
    if (err != CborNoError) {
        printf("failed to encode simple value: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_int(&array_encoder, (int64_t)msg->len);
    if (err != CborNoError) {
        printf("failed to encode integer: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_byte_string(&array_encoder, msg->data, msg->len);
    if (err != CborNoError) {
        printf("failed to encode byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encoder_close_container(&encoder, &array_encoder);
    if (err != CborNoError) {
        printf("failed to close container: %s\n", cbor_error_string(err));
        return NULL;
    }

    size_t size = cbor_encoder_get_buffer_size(&encoder, buf);

    cbor_encoded_data_t *cbdata = new_cbor_encoded_data(buf, size);
    if (cbdata == NULL) {
        return NULL;
    }
    return cbdata;
}

/*!
 * @brief used to cbor decode data into a message_t instance
 * @param input an instance of cbor_encoded_data_t containing the data to decode
 * @return Success: poitner to an instance of message_t
 * @return Failure: NULL pointer
 */
message_t *cbor_decode_message_t(cbor_encoded_data_t *input) {
    CborParser parser;
    CborValue value, recurse;
    CborError err;

    err = cbor_parser_init(input->data, input->len, 0, &parser, &value);
    if (err != CborNoError) {
        printf("failed to init parser: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_array = cbor_value_is_array(&value);
    if (is_array == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

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

    uint8_t message_type;
    err = cbor_value_get_simple_type(&recurse, &message_type);
    if (err != CborNoError) {
        printf("failed to get simple value: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance(&recurse);
    if (err != CborNoError) {
        printf("failed to advanced iter: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_int = cbor_value_is_integer(&recurse);
    if (is_int == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    int64_t len;
    err = cbor_value_get_int64(&recurse, &len);
    if (err != CborNoError) {
        printf("failed to get int: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance(&recurse);
    if (err != CborNoError) {
        printf("failed to advance iter: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool is_byte = cbor_value_is_byte_string(&recurse);
    if (is_byte == false) {
        printf("unexpected value encountered: %s\n", cbor_error_string(err));
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

    err = cbor_value_leave_container(&value, &recurse);
    if (err != CborNoError) {
        printf("failed to leave container: %s\n", cbor_error_string(err));
        return NULL;
    }

    message_t *msg = calloc(1, sizeof(message_t));
    if (msg == NULL) {
        return NULL;
    }
    msg->data = calloc(1, data_len);
    if (msg->data == NULL) {
        free(msg);
        return NULL;
    }
    memcpy(msg->data, data_buffer, data_len);
    msg->type = (MESSAGE_TYPES_T)message_type;
    msg->len = data_len;
    return msg;
}

/*! 
 * @brief frees up resources allocated for an instance of message_t
 * @param msg an instance of message_t
 */
void free_message_t(message_t *msg) {
    free(msg->data);
    free(msg);
}

/*!
 * @brief returns the size of a message_t instance
 */
size_t size_of_message_t(message_t *msg) {
    if (msg == NULL) {
        return 0;
    }

    size_t size = 0;
    size += sizeof(*msg);
    size += msg->len;

    return size;
}

/*!
  * @brief fills `buffer` with the data to send
  * @details if the total size needed is greater than max_len the call fails
  * @return Success: 0
  * @return Failure: -1
*/
int get_message_t_send_buffer(message_t *msg, unsigned char *buffer, size_t max_len) {
    if (msg->len + 1 > max_len || max_len == 0) {
        return -1;
    }
    buffer[0] = msg->len;
    memcpy(buffer + 1, msg->data, max_len - 1); // max_len -1  because 1 byte for the size
    return 0;
}
/*!
  * @brief gets the size of a buffer needed for sending message_t
  * @return Success: size of buffer
  * @return Failure: -1
*/
size_t get_message_t_send_buffer_len(message_t *msg) {
    /*!
      * @todo sizeof(size_t) is 8 bytes, maybe we dont need this?
      * @todo in theory we only need 1 byte to store the value of size_t
    */
    return msg->len + 1;
}