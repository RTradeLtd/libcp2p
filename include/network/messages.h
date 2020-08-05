/*! @file messages.h
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

#pragma once

#include "encoding/cbor.h"
#include "network/socket.h"
#include "thirdparty/logger/logger.h"
#include <stdbool.h>
#include <stddef.h>

/*!
 * @enum MESSAGE_TYPES
 * @typedef MESSAGE_TYPES_T
 * @brief denotes a type of message in a format for inclusion in structs
 * @details the actual value of the message is a macro defined at the top of
 * messages.h
 */
typedef enum MESSAGE_TYPES {
    /*!
     * @brief indicates we are sending a message wanting the peerID
     */
    MESSAGE_WANT_PEER_ID = 0,
    /*!
     * @brief indicates we have a peer id
     * @details sent in response to a WANT_PEERID message
     */
    MESSAGE_HAVE_PEER_ID = 1,
    /*!
     * @brief indicates we want a public key
     */
    MESSAGE_WANT_PUB_KEY = 2,
    /*!
     * @brief indicates we have a public key
     * @details sent in response to a WANT_PUB_KEY messages
     */
    MESSAGE_HAVE_PUB_KEY = 3,
    /*!
     * @brief indicates we want to start an ECDH key exchange
     */
    MESSAGE_START_ECDH = 4,
    /*!
     * @brief indicates we are willing to start an ECDH key exchange and tells the
     * other peer to begin it
     */
    MESSAGE_BEGIN_ECDH = 5,
    /*!
     * @brief used for exchanging hello protocol messages
     * @details the hello protocol is used to exchange peer information (peer id and
     * public key)
     */
    MESSAGE_HELLO = 6,
    /*!
      * @brief indicates we are done with the current message exchange
      * @details the proper way to shutdown a connection to a peer is to
      * @details send an RPC message with the type of MESSAGE_FINISHED followed by closing both sides of the connection
    */
    MESSAGE_FINISHED = 7,
    /*!
     * @brief an arbitrary message type for implementation defined behavior
     */
    MESSAGE_ARBITRARY = 99
} MESSAGE_TYPES_T;

/*!
 * @struct message
 * @typedef message_t
 * @brief a structure wrapping a message used by libcp2p
 * @details a message is sent to a peer, or received from a peer during
 * communications
 */
typedef struct message {
    MESSAGE_TYPES_T type; /*! @brief the type of message */
    size_t len;           /*! @brief the size of the data contained in the message */
    unsigned char *data;  /*! @brief the actual data in the message */
} message_t;

/*!
 * @struct message_hello
 * @typedef message_hello_t
 * @brief used to specify the peerid and public key for a MESSAGE_HELLO RPC call
 * @details when sending a MESSAGE_HELLO RPC to a peer, the message data must be
 * message_hello_t in CBOR encoding
 */
typedef struct message_hello {
    /*! @brief the size of peer_id */
    size_t peer_id_len;
    /*! @brief the size of public_key */
    size_t public_key_len;
    /*! @brief a peer's identifier */
    unsigned char *peer_id;
    /*! @brief the corresponding public key for the peerid */
    unsigned char *public_key;
} message_hello_t;

/*!
 * @brief used to create a new message_hello_t using the given values
 * @details this copies the values given and allocates memory to store them
 * accordingly
 */
message_hello_t *new_message_hello_t(unsigned char *peer_id,
                                     unsigned char *public_key, size_t peer_id_len,
                                     size_t public_key_len);

/*!
 * @brief used to cbor encoded a message_hello_t instance
 * @details the resulting data and length fields can be used with
 * @details the message_t instance to send peer information to another peer
 */
cbor_encoded_data_t *cbor_encode_hello_t(message_hello_t *msg_hello);

/*!
 * @brief used to cbor decode encoded data returning an instance of message_hello_t
 */
message_hello_t *cbor_decode_hello_t(cbor_encoded_data_t *input);

/*!
 * @brief used to free up resources associated with a message_hello_t instance
 */
void free_message_hello_t(message_hello_t *msg_hello);

/*!
 * @brief used to cbor encode a message_t instance
 * @param msg pointer to an instance of message_t
 * @return Success: pointer to an instance of cbor_encoded_data_t
 * @return Failure: NULL pointer
 */
cbor_encoded_data_t *cbor_encode_message_t(message_t *msg);

/*!
 * @brief used to cbor decode data into a message_t instance
 * @param input an instance of cbor_encoded_data_t containing the data to decode
 * @return Success: poitner to an instance of message_t
 * @return Failure: NULL pointer
 */
message_t *cbor_decode_message_t(cbor_encoded_data_t *input);

/*!
 * @brief frees up resources allocated for an instance of message_t
 * @param msg an instance of message_t
 */
void free_message_t(message_t *msg);

/*!
 * @brief returns the size of a message_t instance
 */
size_t size_of_message_t(message_t *msg);

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
                          size_t max_buffer_len);

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
int handle_send(thread_logger *thl, int socket_number, message_t *msg);