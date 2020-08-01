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

#include "network/utils.h"
#include "network/messages.h"
#include "encoding/cbor.h"
#include "network/socket.h"
#include "tinycbor/cbor.h"
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

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
 * @brief used to handle receiving data from a UDP or TCP socket
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
 * @param is_tcp indicates whether this is a TCP socket
 * @param max_buffer_len specifies the maximum buffer length we are willing to
 * allocate memory for
 * @return Success: pointer to an a chunk of memory containing an instance of
 * cbor_encoded_data_t
 * @return Failure: NULL pointer
 * @warning we will allocate slightly more memory than max_buffer_len since this
 * refers to the maximum buffer size of the data member of a cbor_encoded_data_t
 * instance, althoug h it will only be a few bytes more
 */
message_t *handle_receive(thread_logger *thl, int socket_number, bool is_tcp,
                          size_t max_buffer_len) {

    size_t rc = 0;
    bool failed = false;
    int message_size = 0;

    int rci = receive_int(&message_size, socket_number);
    if (rci == -1) {
        return NULL;
    }
    printf("message size: %i\n", message_size);
    /*!
     * @brief abort further handling if message size is less than or equal to 0
     * @brief greater than the max RPC message size OR greater than the buffer
     */
    if (message_size <= 0 || message_size >= MAX_RPC_MSG_SIZE_KB ||
        message_size > (int)max_buffer_len) {
        if (thl != NULL) {
            thl->log(thl, 0, "invalid message size", LOG_LEVELS_DEBUG);
        }
        return NULL;
    }

    unsigned char buffer[max_buffer_len];
    memset(buffer, 0, max_buffer_len);

    if (is_tcp == true) {
        rc = recv(socket_number, buffer, max_buffer_len, 0);
    } else {
        rc = recvfrom(socket_number, buffer, max_buffer_len, 0, NULL, NULL);
    }

    failed = recv_or_send_failed(thl, rc);
    if (failed == true) {
        return NULL;
    }

    // dont allocate memory for this struct so we can use stack memory
    cbor_encoded_data_t cbdata = {.len = rc, .data = buffer};

    return cbor_decode_message_t(&cbdata);
}

/*!
 * @brief used to handle sending data through a TCP or UDP socket
 * @details designed to reduce manual overhead with sending RPC messages
 * @details it takes care of encoding the given message_t object into a CBOR object
 * @details and then sending the CBOR object through the wire
 * @return Success: 0
 * @return Failure: -1
 */
int handle_send(thread_logger *thl, int socket_number, bool is_tcp, message_t *msg,
                addr_info *peer_address) {

    if (is_tcp == false && peer_address == NULL) {
        return -1;
    }

    cbor_encoded_data_t *cbdata = cbor_encode_message_t(msg);
    if (cbdata == NULL) {
        return -1;
    }

    size_t cbor_len = get_encoded_send_buffer_len(cbdata);
    if (cbor_len <= 0) {
        free_cbor_encoded_data(cbdata);
        return -1;
    }

    unsigned char send_buffer[cbor_len];
    memset(send_buffer, 0, cbor_len);
    memcpy(send_buffer, cbdata->data, cbor_len - 1);
    // int rc = get_encoded_send_buffer(cbdata, send_buffer, cbor_len);

    // free memory as the cbor encoded data struct is no longer needed
    free_cbor_encoded_data(cbdata);

//    if (rc == -1) {
//        return -1;
//    }
    int rc = 0;
    if (is_tcp == true) {
        // send the buffer size
        rc = send_int((int)cbor_len, socket_number);
        if (rc == -1) {
            printf("failed to send message size\n");
        }
        rc = send(socket_number, send_buffer, sizeof(send_buffer), 0);
    } else {
        /*!
         * @todo we likely need to refactor this and send two datagrams
         * @todo the first datagram containing the messsage size and
         * @todo the second datagram containing the actual message data
         */
        rc = sendto(socket_number, send_buffer, sizeof(send_buffer), 0,
                    peer_address->ai_addr, peer_address->ai_addrlen);
    }

    bool failed = recv_or_send_failed(thl, rc);
    if (failed == true) {
        return -1;
    }

    return 0;
}