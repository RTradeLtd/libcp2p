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

#include "utils/logger.h"
#include "encoding/cbor.h"
#include <stddef.h>
#include <stdbool.h>

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
 * @struct message_send_opts
 * @typedef message_send_opts_t
 * @brief defines options used for sending messages to peers
 */
typedef struct message_send_opts {
    /*!
     * @brief the source file descriptor we are sending from
     */
    int src_fd;
    /*!
     * @brief the destination file descriptor we are sending too
     */
    int dst_fd;
} message_send_opts_t;

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
 * @brief used to send messages to peers
 * @param msg the actual message we are sending to a peer
 * @param opts specifies how to send the message to a peer
 */
int send_message(message_t *msg, message_send_opts_t opts);

/*!
 * @brief returns the size of a message_t instance
 */
size_t size_of_message_t(message_t *msg);


/*!
  * @brief used to handle receiving data from a UDP or TCP socket
  * @details it is designed to reduce the manual overhead with regards to processing messages
  * @details because the first byte of any data stream coming in defines the size of the total data to receive
  * @details and the remaining data defines the actual cbor encoded data. therefore we need to properly parse this information
  * @details and the manner of processing is useful to either the server or client side of things
  * @param socket_num the file descriptor of the socket to receive from
  * @param is_tcp indicates whether this is a TCP socket
  * @param max_buffer_len specifies the maximum buffer length we are willing to allocate memory for
  * @return Success: pointer to an a chunk of memory containing an instance of cbor_encoded_data_t
  * @return Failure: NULL pointer
  * @warning we will allocate slightly more memory than max_buffer_len since this refers to the maximum buffer size of the data member of a cbor_encoded_data_t instance, althoug h it will only be a few bytes more
*/
cbor_encoded_data_t *handle_receive(thread_logger *thl, int socket_number, bool is_tcp, size_t max_buffer_len);