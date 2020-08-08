// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! @file socket_server.c
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#include "network/socket_server.h"
#include "encoding/cbor.h"
#include "network/messages.h"
#include "peerstore/peerstore.h"
#include "thirdparty/thread_pool/thread_pool.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
/*!
 * @brief internal mutex lock used for signalling shutdown in async
 * start_socket_server function calls
 */
pthread_mutex_t shutdown_mutex;

bool do_shutdown = false;

/*!
 * @brief used to create a TCP/UDP socket server ready to accept connections
 * @param thl an instance of a thread_logger
 * @param config the configuration settings used for the tcp/udp server
 * @param sock_opts an array of options to configure the sockets we open with
 * @param num_opts the number of socket options we are using, providing a number that
 * does not match the actual number of options is undefined behavior
 * @return Success: pointer to a socket_server_t instance
 * @return Failure: NULL pointer
 * @details once you have used the config and created a new server with
 * @note once you have used the config and created a new server with
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t *config,
                                   SOCKET_OPTS sock_opts[], int num_opts) {

    int max_socket_num = 0;
    fd_set grouped_socket_set;
    fd_set tcp_socket_set;

    // initialize the file descriptor groups
    FD_ZERO(&tcp_socket_set);
    FD_ZERO(&grouped_socket_set);

    // if 0 default to 100
    if (config->max_peers == 0) {
        config->max_peers = 100;
    }

    if (config->private_key_path == NULL) {
        LOG_ERROR(thl, 0, "no private key path in config");
        return NULL;
    }

    for (int i = 0; i < config->num_addrs; i++) {

        addr_info *bind_address = multi_addr_to_addr_info(config->addrs[i]);
        if (bind_address == NULL) {
            LOG_ERROR(thl, 0, "failed to get addr info");
            goto EXIT;
        }

        bool is_tcp = false;

        if (bind_address->ai_socktype == SOCK_STREAM) {
            is_tcp = true;
        }

        // handle a tcp multi_address
        if (is_tcp) {

            int tcp_socket_num =
                get_new_socket(thl, bind_address, sock_opts, num_opts, false, true);
            if (tcp_socket_num == -1) {
                freeaddrinfo(bind_address);
                LOG_ERROR(thl, 0, "failed to get new tcp socket");
                goto EXIT;
            }

            listen(tcp_socket_num, config->max_connections);
            if (errno != 0) {
                freeaddrinfo(bind_address);
                LOGF_ERROR(thl, 0,
                           "failed to start listening on tcp socket with error %s",
                           strerror(errno));
                goto EXIT;
            }

            FD_SET(tcp_socket_num, &tcp_socket_set);
            FD_SET(tcp_socket_num, &grouped_socket_set);

            if (tcp_socket_num > max_socket_num) {
                max_socket_num = tcp_socket_num;
            }

            if (config->recv_timeout_sec > 0) {
                set_socket_recv_timeout(tcp_socket_num, config->recv_timeout_sec);
            }

            freeaddrinfo(bind_address);
        }
    }

    socket_server_t *server = calloc(1, sizeof(socket_server_t));
    if (server == NULL) {
        LOG_ERROR(thl, 0, "failed to calloc socket server");
        goto EXIT;
    }

    // increase max socket number by 1 for select usage
    max_socket_num += 1;

    // initialize our peerstore
    server->pstore = peerstore_new_peerstore((size_t)config->max_peers);
    if (server->pstore == NULL) {
        LOG_ERROR(thl, 0, "failed to create peerstore");
        free(server);
        goto EXIT;
    }

    // load the private key
    server->private_key =
        libp2p_crypto_ecdsa_private_key_from_file(config->private_key_path);
    if (server->private_key == NULL) {
        LOG_ERROR(thl, 0, "failed to load private key");
        peerstore_free_peerstore(server->pstore);
        free(server);
        goto EXIT;
    }

    // load our public key
    server->public_key = libp2p_crypto_ecdsa_keypair_public(server->private_key);
    if (server->public_key == NULL) {
        LOG_ERROR(thl, 0, "failed to load public key");
        peerstore_free_peerstore(server->pstore);
        libp2p_crypto_ecdsa_free(server->private_key);
        free(server);
        goto EXIT;
    }

    // load our peer identifier
    server->peer_id = libp2p_crypto_ecdsa_keypair_peerid(server->private_key);
    if (server->peer_id == NULL) {
        LOG_ERROR(thl, 0, "failed to load peerid");
        peerstore_free_peerstore(server->pstore);
        libp2p_crypto_ecdsa_free(server->private_key);
        libp2p_crypto_public_key_free(server->public_key);
        free(server);
        goto EXIT;
    }

    // setup remaining server components
    server->thpool = thpool_init(config->num_threads);
    server->max_socket_num = max_socket_num;
    server->grouped_socket_set = grouped_socket_set;
    server->tcp_socket_set = tcp_socket_set;
    server->task_func_tcp = config->fn_tcp;
    server->thl = thl;
    pthread_mutex_init(&shutdown_mutex, NULL);
    LOG_INFO(server->thl, 0, "initialized server");
    return server;

EXIT:

    for (int i = 0; i < max_socket_num; i++) {
        if (FD_ISSET(i, &tcp_socket_set)) {
            close(i);
        }
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    LOG_INFO(srv->thl, 0, "closing sockets");

    for (int i = 0; i < srv->max_socket_num; i++) {

        if (FD_ISSET(i, &srv->tcp_socket_set)) {

            LOGF_INFO(srv->thl, 0, "closing tcp socket number %i", i);

            close(i);
        }
    }

    LOG_INFO(srv->thl, 0, "waiting for existing tasks to exit");

    thpool_destroy(srv->thpool);

    pthread_mutex_destroy(&shutdown_mutex);

    peerstore_free_peerstore(srv->pstore);

    libp2p_crypto_ecdsa_free(srv->private_key);

    libp2p_crypto_public_key_free(srv->public_key);

    libp2p_peer_id_free(srv->peer_id);

    LOG_INFO(srv->thl, 0, "all tasks exited, goodbye");

    clear_thread_logger(srv->thl);

    free(srv);
}

/*!
 * @brief starts the socket server which processes new connections
 * @details when a new connection is accepted (tcp) OR we can receive data on a udp
 * socket, the given handle_conn_func is used to process that client connection
 * @param srv an instance of a socket_server_t that has been initialized through
 * new_socket_server
 */
void start_socket_server(socket_server_t *srv) {
    /*!
     * @todo enable customizable timeout
     */
    timeout tmt;
    tmt.tv_sec = 3;
    tmt.tv_usec = 0;

    for (;;) {
        if (do_shutdown == true) {
            LOG_INFO(srv->thl, 0, "shutdown signal received, exiting");
            return;
        }

        /*! * @note copy the main socket list containing both tcp and udp sockets
         */
        fd_set working_copy = srv->grouped_socket_set;

        int rc = select(srv->max_socket_num, &working_copy, NULL, NULL, &tmt);

        switch (rc) {
            case 0:
                sleep(0.50);
                continue;
            case -1:
                LOGF_DEBUG(srv->thl, 0, "an error occured while running select: %s",
                           strerror(errno));
                sleep(0.50);
                return;
        }

        /*!
         * @brief iterate over all known sockets
         * @note this will likely search a few extra sockets, but the overhead should
         * be neglibie
         */
        for (int i = 0; i < srv->max_socket_num; i++) {

            // check to see whether or not we have a socket with this number
            if (FD_ISSET(i, &working_copy)) {

                // if it is a tcp socket, handle it with the tcp worker func
                if (FD_ISSET(i, &srv->tcp_socket_set)) {

                    client_conn_t *conn = accept_client_conn(srv, i);
                    if (conn == NULL) {
                        sleep(0.50);
                        continue;
                    }

                    conn_handle_data_t *chdata =
                        calloc(1, sizeof(conn_handle_data_t));
                    if (chdata == NULL) {
                        close(conn->socket_number);
                        free(conn);
                        sleep(0.50);
                        continue;
                    }
                    chdata->srv = srv;
                    chdata->conn = conn;
                    thpool_add_work(srv->thpool, srv->task_func_tcp, chdata);
                }
            }
        }
        // sleep bfor 500 miliseconds before looping again
        sleep(0.50);
    }
}

/*!
 * @brief used to signal that we should exit the main start_socket_server function
 * @note this is only useful if you launch start_socket_server in a thread
 */
void signal_shutdown() {
    pthread_mutex_lock(&shutdown_mutex);
    do_shutdown = true;
    pthread_mutex_unlock(&shutdown_mutex);
}

/*! @brief helper function for accepting client connections
 * times out new attempts if they take 3 seconds or more
 * @return Failure: NULL client conn failed
 * @return Success: non-NULL populated client_conn object
 */
client_conn_t *accept_client_conn(socket_server_t *srv, int socket_num) {
    // temporary variable for storing socket address
    sock_addr_storage addr_temp;
    // set client_len
    // i tried doing `(sock_addr *)&sizeof(addr_temp)
    // in the `accept` function call but it didnt work
    socklen_t client_len = sizeof(addr_temp);
    int client_socket_num = accept(socket_num, (sock_addr *)&addr_temp, &client_len);
    // socket number less than 0 is an error
    if (client_socket_num < 0) {
        return NULL;
    }
    client_conn_t *connection = calloc(1, sizeof(client_conn_t));
    if (connection == NULL) {
        return NULL;
    }
    connection->socket_number = client_socket_num;
    if (srv->thl->debug == true) {
        char *addr_inf = get_name_info((sock_addr *)&addr_temp);
        LOGF_DEBUG(srv->thl, 0, "accepted new connection: %s", addr_inf);
        free(addr_inf);
    }
    return connection;
}

/*!
 * @brief used to free up resources allocated for socket_server_config_t
 * @param config an instance of socket_server_config_t initialized with
 * new_socket_server_config
 */
void free_socket_server_config(socket_server_config_t *config) {
    for (int i = 0; i < config->num_addrs; i++) {
        multi_address_free(config->addrs[i]);
    }
    free(config->addrs);
    free(config);
}

/*!
 * @brief used to initialize a socket_server_config_t object
 * @param num_addrs the number of multi_addr_t objects the addrs member will contain
 * @return Success: pointer to an initialized block of memory for
 * socket_server_config_t
 * @return Failure: NULL pointer
 */
socket_server_config_t *new_socket_server_config(int num_addrs) {
    socket_server_config_t *config = calloc(1, sizeof(socket_server_config_t));
    if (config == NULL) {
        return NULL;
    }
    config->addrs = calloc(1, sizeof(multi_addr_t) * 2);
    config->num_addrs = num_addrs;
    return config;
}

/*!
 * @brief handles receiving an rpc message from another peer
 * @note if you send an inbound message of `5hello` you'll invoke a debug handler to
 * print to stdout
 * @warning needs to check to see if the data we are getting is for a tcp or udp
 * connection
 * @warning if a tcp connection we need to close the socket (as its the socket
 * connecting to the client)
 * @warning if a udp connection we dont close the socket and simply free the
 * resources
 */
void handle_inbound_rpc(void *data) {
    conn_handle_data_t *hdata = NULL;
    hdata = (conn_handle_data_t *)data;
    if (hdata == NULL) {
        return;
    }

    for (;;) {
        // check to see if we should exit
        if (do_shutdown == true) {
            goto RETURN;
        }

        message_t *msg = handle_receive(hdata->srv->thl, hdata->conn->socket_number,
                                        MAX_RPC_MSG_SIZE_KB);

        if (msg == NULL) {
            goto RETURN;
        }

        bool success = false;
        bool ok = false;

        switch (msg->type) {
            case MESSAGE_START_ECDH:
                success = negotiate_secure_connection(hdata);
                if (success == false) {
                    LOG_DEBUG(hdata->srv->thl, 0,
                              "failed to start secure connection negotiation");
                    free_message_t(msg);
                    goto RETURN;
                } else {
                    LOG_DEBUG(hdata->srv->thl, 0,
                              "started secure connection negotiation");
                }
                break;
            case MESSAGE_BEGIN_ECDH:
                break;
            case MESSAGE_WANT_PEER_ID:
                break;
            case MESSAGE_HAVE_PEER_ID:
                break;
            case MESSAGE_WANT_PUB_KEY:
                break;
            case MESSAGE_HAVE_PUB_KEY:
                break;
            case MESSAGE_HELLO_INT:
                // fallthrough
            case MESSAGE_HELLO_FIN:
                ok = handle_hello_protocol(hdata, msg);
                if (ok == false) {
                    LOG_DEBUG(hdata->srv->thl, 0,
                              "failed to conduct hello protocol exchange");
                    free_message_t(msg);
                    goto RETURN;
                } else {
                    LOG_DEBUG(hdata->srv->thl, 0,
                              "successfully conducted hello protocol exchange");
                }
                break;
            case MESSAGE_ARBITRARY:
                break;
            default:
                break;
        }

        free_message_t(msg);
    }
RETURN:

    LOG_DEBUG(hdata->srv->thl, 0, "closing connection");

    close(hdata->conn->socket_number);
    free(hdata->conn);
    free(hdata);
}

/*!
 * @brief used to specify which syscall signals should trigger shutdown process
 */
void setup_signal_shutdown(int signals[], int num_signals) {
    for (int i = 0; i < num_signals; i++) {
        signal(signals[i], signal_shutdown);
    }
}

/*!
 * @brief used to negotiate a secure connection with the current connection
 */
bool negotiate_secure_connection(conn_handle_data_t *data) {
    message_t *msg = calloc(1, sizeof(message_t));
    if (msg == NULL) {
        return false;
    }

    msg->data = calloc(1, 2);
    if (msg->data == NULL) {
        free(msg);
        return false;
    }

    msg->type = MESSAGE_BEGIN_ECDH;
    msg->data[0] = 'o';
    msg->data[1] = 'k';
    msg->len = 2;

    int rc = handle_send(data->srv->thl, data->conn->socket_number, msg);

    free_message_t(msg);

    if (rc == -1) {
        return false;
    }

    return true;
}

/*!
 * @brief handles receiving a hello protocol message from another peer
 * @details is responsible for exchanging identification information with a peer
 * @details and updating our peerstore with the appropriate information
 */
bool handle_hello_protocol(conn_handle_data_t *data, message_t *msg) {

    // temporary struct to hold message data
    cbor_encoded_data_t cbdata = {.data = msg->data, .len = msg->len};

    // decode into the hello protocol message
    message_hello_t *msg_hello = cbor_decode_hello_t(&cbdata);
    if (msg_hello == NULL) {
        LOG_DEBUG(data->srv->thl, 0, "failed to cbor decode message_hello_t");
        return false;
    }

    // insert the peers information into our peerstore
    bool ok = peerstore_insert_peer(data->srv->pstore, msg_hello->peer_id,
                                    msg_hello->public_key, msg_hello->peer_id_len,
                                    msg_hello->public_key_len);

    free_message_hello_t(msg_hello);

    if (ok == false) {
        LOG_DEBUG(data->srv->thl, 0, "failed to isnert peer into peerstore");
        return ok;
    } else {
        LOG_DEBUG(data->srv->thl, 0, "successfully inserted peer into peerstore");
    }

    // if this is MESSAGE_HELLO_FIN it means we dont need to continue the exchange
    if (msg->type == MESSAGE_HELLO_FIN) {
        return ok;
    }

    message_hello_t *send_msg_hello = new_server_message_hello_t(data->srv);
    if (send_msg_hello == NULL) {
        LOG_DEBUG(data->srv->thl, 0, "failed to create message_hello_t from server");
        return false;
    }

    // send the MESSSAGE_HELLO_FIN message in the exchange
    message_t *send_msg = message_hello_t_to_message_t(send_msg_hello, false);

    free_message_hello_t(send_msg_hello);

    if (send_msg == NULL) {
        LOG_DEBUG(data->srv->thl, 0, "failed to get message_t");
        return false;
    }

    // send the data to our peer
    int rc = handle_send(data->srv->thl, data->conn->socket_number, send_msg);

    free_message_t(send_msg);

    if (rc == -1) {
        LOG_DEBUG(data->srv->thl, 0, "failed t osend message");
        return false;
    }

    return true;
}

/*!
 * @brief used for a server to send a message to another server
 * @details this is a sort of "bi-directional RPC method" whereby a server can send a
 * @details request to another server acting as a client, but enabling either us
 * @details or the peer to invoke RPC methods. Essentially it is like
 * handle_inbound_rpc
 * @details except it is responsible for sending requests to a remote server, and any
 * responses
 * @details from the server are ran through handle_inbound_rpc
 * @return Success: 0
 * @return Failure: -1
 */
int socket_server_send(socket_server_t *srv, multi_addr_t *to_address,
                       message_t *msg) {

    socket_client_t *srv_client = new_socket_client(srv->thl, to_address);
    if (srv_client == NULL) {
        return -1;
    }

    client_conn_t *conn_data = calloc(1, sizeof(client_conn_t));
    if (conn_data == NULL) {
        goto EXIT;
    }

    conn_data->socket_number = srv_client->socket_number;

    conn_handle_data_t *chdata = calloc(1, sizeof(conn_handle_data_t));
    if (chdata == NULL) {
        free(conn_data);
        goto EXIT;
    }

    chdata->conn = conn_data;
    chdata->srv = srv;

    // send the message to the peer
    handle_send(srv->thl, srv_client->socket_number, msg);

    // add work to the threadpool to handle responses to the message we just sent
    thpool_add_work(srv->thpool, srv->task_func_tcp, chdata);

    freeaddrinfo(srv_client->peer_address);
    free(srv_client);

    // return and dont free up resources as the task func handles this
    return 0;

EXIT:

    close(srv_client->socket_number);

    freeaddrinfo(srv_client->peer_address);

    free(srv_client);
    return -1;
}

/*!
 * @brief helper function to return a message_hello_t using our server values
 */
message_hello_t *new_server_message_hello_t(socket_server_t *srv) {
    return new_message_hello_t(srv->peer_id->data, srv->public_key->data,
                               srv->peer_id->len, srv->public_key->data_size);
}