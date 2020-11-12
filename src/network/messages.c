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
#include "network/socket.h"
#include "network/utils.h"
#include "tinycbor/cbor.h"
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

/*!
 * @brief helper function to return a message_t object for sending
 * @details if initiate is true, the message type is MESSAGE_HELLO_INT
 * @details if initiate is false, the message type is MESSAGE_HELLO_FIN
 * @param msg the message_hello_t type we want to encode into message_t format
 * @param initiate True: set return message_t type to MESSAGE_HELLO_INT
 * @param initiate False: set return message_t type to MESSAGE_HELLO_FIN
 * @return Success: pointer to a message_t instance containing the encoded input
 * message
 * @return Failure: NULL pointer
 */
message_t *message_hello_t_to_message_t(message_hello_t *msg, bool initiate) {
    cbor_encoded_data_t *encoded = cbor_encode_hello_t(msg);
    if (encoded == NULL) {
        return NULL;
    }

    message_t *output = calloc(1, sizeof(message_t));
    if (output == NULL) {
        free_cbor_encoded_data(encoded);
        return NULL;
    }

    output->data = calloc(1, encoded->len);
    if (output->data == NULL) {
        free_cbor_encoded_data(encoded);
        free(output);
        return NULL;
    }

    memcpy(output->data, encoded->data, encoded->len);
    output->len = encoded->len;

    // free up as we no longer need
    free_cbor_encoded_data(encoded);

    if (initiate == true) {
        output->type = MESSAGE_HELLO_INT;
    } else {
        output->type = MESSAGE_HELLO_FIN;
    }

    return output;
}

/*!
 * @brief used to create a new message_hello_t using the given values
 * @details this copies the values given and allocates memory to store them
 * accordingly
 * @todo add a test specifically for this function
 */
message_hello_t *new_message_hello_t(unsigned char *peer_id,
                                     unsigned char *public_key, size_t peer_id_len,
                                     size_t public_key_len) {

    message_hello_t *msg = calloc(1, sizeof(message_hello_t));
    if (msg == NULL) {
        return NULL;
    }

    msg->peer_id = calloc(1, peer_id_len);
    if (msg->peer_id == NULL) {
        free(msg);
        return NULL;
    }

    msg->public_key = calloc(1, public_key_len);
    if (msg->public_key == NULL) {
        free(msg->peer_id);
        free(msg);
        return NULL;
    }

    memcpy(msg->peer_id, peer_id, peer_id_len);
    memcpy(msg->public_key, public_key, public_key_len);

    msg->public_key_len = public_key_len;
    msg->peer_id_len = peer_id_len;

    return msg;
}

/*!
 * @brief used to cbor encoded a message_hello_t instance
 * @details the resulting data and length fields can be used with
 * @details the message_t instance to send peer information to another peer
 */
cbor_encoded_data_t *cbor_encode_hello_t(message_hello_t *msg_hello) {
    size_t maddr_size;
    for (size_t i = 0; i < msg_hello->num_addrs; i++) {
        maddr_size += msg_hello->addrs[i]->bsize;
    }
    // maddr_size covers the actual size of the multiaddrs, and sizeof(size_t)
    // covers the size of the size_t variable
    uint8_t buf[sizeof(message_hello_t) + msg_hello->peer_id_len +
                msg_hello->public_key_len + maddr_size + sizeof(size_t)];
    CborEncoder encoder, array_encoder;
    CborError err;

    cbor_encoder_init(&encoder, buf, sizeof(buf), 0);

    err = cbor_encoder_create_array(&encoder, &array_encoder, 4);
    if (err != CborNoError) {
        printf("failed to create array: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_int(&array_encoder, (int64_t)msg_hello->peer_id_len);
    if (err != CborNoError) {
        printf("failed to encode int: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_int(&array_encoder, (int64_t)msg_hello->public_key_len);
    if (err != CborNoError) {
        printf("failed to encode int: %s\n", cbor_error_string(err));
        return NULL;
    }

    // encode the total number of multiaddresses
    err = cbor_encode_int(&array_encoder, (int64_t)msg_hello->num_addrs);
    if (err != CborNoError) {
        printf("failed to encode int: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_byte_string(&array_encoder, msg_hello->peer_id,
                                  msg_hello->peer_id_len);
    if (err != CborNoError) {
        printf("failed to encode byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_encode_byte_string(&array_encoder, msg_hello->public_key,
                                  msg_hello->public_key_len);
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

    return new_cbor_encoded_data(buf, size);
}

/*!
 * @brief used to cbor decode encoded data returning an instance of message_hello_t
 */
message_hello_t *cbor_decode_hello_t(cbor_encoded_data_t *input) {
    CborParser parser;
    CborValue value, recurse;
    CborError err;

    err = cbor_parser_init(input->data, input->len, 0, &parser, &value);
    if (err != CborNoError) {
        printf("failed to init parser: %s\n", cbor_error_string(err));
        return NULL;
    }

    bool ok = cbor_value_is_array(&value);
    if (ok == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    err = cbor_value_enter_container(&value, &recurse);
    if (err != CborNoError) {
        printf("failed to enter container: %s\n", cbor_error_string(err));
        return NULL;
    }

    ok = cbor_value_is_integer(&recurse);
    if (ok == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    int64_t peer_id_len;
    err = cbor_value_get_int64(&recurse, &peer_id_len);
    if (err != CborNoError) {
        printf("failed to get value: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance(&recurse);
    if (err != CborNoError) {
        printf("failed to advance: %s\n", cbor_error_string(err));
        return NULL;
    }

    ok = cbor_value_is_integer(&recurse);
    if (ok == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    int64_t public_key_len;
    err = cbor_value_get_int64(&recurse, &public_key_len);
    if (err != CborNoError) {
        printf("failed to get value: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_advance(&recurse);
    if (err != CborNoError) {
        printf("failed to advance: %s\n", cbor_error_string(err));
        return NULL;
    }

    ok = cbor_value_is_byte_string(&recurse);
    if (ok == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    uint8_t peer_id_buf[peer_id_len];
    size_t peer_id_buf_len = peer_id_len;
    err = cbor_value_copy_byte_string(&recurse, peer_id_buf, &peer_id_buf_len,
                                      &recurse);
    if (err != CborNoError) {
        printf("failed to copy byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    ok = cbor_value_is_byte_string(&recurse);
    if (ok == false) {
        printf("unexpected value encountered\n");
        return NULL;
    }

    uint8_t public_key_buf[public_key_len];
    size_t public_key_buf_len = public_key_len;
    err = cbor_value_copy_byte_string(&recurse, public_key_buf, &public_key_buf_len,
                                      &recurse);
    if (err != CborNoError) {
        printf("failed to copy byte string: %s\n", cbor_error_string(err));
        return NULL;
    }

    err = cbor_value_leave_container(&value, &recurse);
    if (err != CborNoError) {
        printf("failed to leave container: %s\n", cbor_error_string(err));
        return NULL;
    }

    message_hello_t *msg_hello = calloc(1, sizeof(message_hello_t));
    if (msg_hello == NULL) {
        return NULL;
    }

    msg_hello->peer_id = calloc(1, peer_id_buf_len);
    if (msg_hello->peer_id == NULL) {
        free(msg_hello);
        return NULL;
    }

    msg_hello->public_key = calloc(1, public_key_buf_len);
    if (msg_hello->public_key == NULL) {
        free(msg_hello->peer_id);
        free(msg_hello);
        return NULL;
    }

    memcpy(msg_hello->peer_id, peer_id_buf, peer_id_buf_len);
    memcpy(msg_hello->public_key, public_key_buf, public_key_buf_len);
    msg_hello->peer_id_len = peer_id_buf_len;
    msg_hello->public_key_len = public_key_buf_len;

    return msg_hello;
}

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
 * @brief used to free up resources associated with a message_hello_t instance
 */
void free_message_hello_t(message_hello_t *msg_hello) {
    free(msg_hello->peer_id);
    free(msg_hello->public_key);
    free(msg_hello);
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
 * @brief used to handle receiving data from a TCP socket
 * @details it is designed to reduce the manual overhead with regards to processing
 * messages
 * @details because the first byte of any data stream coming in defines the size of
 * the total data to receive
 * @details and the remaining data defines the actual cbor encoded data. therefore we
 * need to properly parse this information
 * @details and the manner of processing is useful to either the server or client
 * side of things
 * @param thl an instance of a thread_logger, can be NULL to disable logging
 * @param socket_num the file descriptor of the socket to receive from
 * @param max_buffer_len specifies the maximum buffer length we are willing to
 * allocate memory for
 * @return Success: pointer to a chunk of memory containing the received RPC message
 * @return Failure: NULL pointer
 * @warning we will allocate slightly more memory than max_buffer_len since we have
 * to decode the received message into a message_t type
 */
message_t *handle_receive(thread_logger *thl, int socket_number,
                          size_t max_buffer_len) {

    size_t rc = 0;
    bool failed = false;
    int message_size = 0;

    int rci = receive_int_tcp(&message_size, socket_number);

    if (rci == -1) {
        return NULL;
    }

    /*!
     * @brief abort further handling if message size is less than or equal to 0
     * @brief greater than the max RPC message size OR greater than the buffer
     */
    if (message_size <= 0 || message_size >= MAX_RPC_MSG_SIZE_KB ||
        message_size > (int)max_buffer_len) {
        if (thl != NULL) {
            LOG_DEBUG(thl, 0, "invalid msg size");
        }
        return NULL;
    }

    unsigned char buffer[max_buffer_len];
    memset(buffer, 0, max_buffer_len);

    rc = recv(socket_number, buffer, max_buffer_len, 0);

    failed = recv_or_send_failed(thl, rc);
    if (failed == true) {
        return NULL;
    }

    // dont allocate memory for this struct so we can use stack memory
    cbor_encoded_data_t cbdata = {.len = rc, .data = buffer};

    return cbor_decode_message_t(&cbdata);
}

/*!
 * @brief used to handle sending data through a TCP socket
 * @details designed to reduce manual overhead with sending RPC messages
 * @details it takes care of encoding the given message_t object into a CBOR object
 * @details and then sending the CBOR object through the wire
 * @param thl an instance of a thread_logger, can be NULL to disable logging
 * @param socket_num the file descriptor of the socket to receive from
 * @param msg the actual message we want to send
 * message to. must not be NULL if is_tcp is false
 * @return Success: 0
 * @return Failure: -1
 */
int handle_send(thread_logger *thl, int socket_number, message_t *msg) {

    cbor_encoded_data_t *cbdata = cbor_encode_message_t(msg);
    if (cbdata == NULL) {
        return -1;
    }

    size_t cbor_len = cbdata->len;

    unsigned char send_buffer[cbor_len];
    memset(send_buffer, 0, cbor_len);
    memcpy(send_buffer, cbdata->data, cbor_len);

    free_cbor_encoded_data(cbdata);

    int rc = rc = send_int_tcp((int)cbor_len, socket_number);
    if (rc == -1) {
        return -1;
    }

    rc = send(socket_number, send_buffer, sizeof(send_buffer), 0);

    bool failed = recv_or_send_failed(thl, rc);
    if (failed == true) {
        return -1;
    }

    return 0;
}