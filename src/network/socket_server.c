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
#include "utils/thread_pool.h"
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
/*!
 * @brief internal boolean variable used to signal async start_socket_server function
 * calls
 */
bool do_shutdown = false;

/*!
 * @brief used to check if a receive or send with a socket failed
 */
bool recv_or_send_failed(socket_server_t *srv, int rc);

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
 * new_socket_server() you can free the socket config with free_socket_config
 * @note once you have used the config and created a new server with
 * new_socket_server() you can free the socket config with free_socket_config
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t *config,
                                   SOCKET_OPTS sock_opts[], int num_opts) {

    addr_info tcp_hints;
    addr_info udp_hints;

    int max_socket_num = 0;
    int rc = 0;

    fd_set grouped_socket_set;
    fd_set tcp_socket_set;
    fd_set udp_socket_set;

    // initialize the file descriptor groups
    FD_ZERO(&tcp_socket_set);
    FD_ZERO(&udp_socket_set);
    FD_ZERO(&grouped_socket_set);

    char ip[1024];
    char cport[7];

    for (int i = 0; i < config->num_addrs; i++) {

        // zero ip and cport, overwriting previous data
        memset(ip, 0, 1024);
        memset(cport, 0, 7);

        // get the ip address associated with the multiaddr
        rc = multi_address_get_ip_address(config->addrs[i], ip);
        if (rc != 1) {
            thl->log(thl, 0, "failed to get ip address from multiaddr",
                     LOG_LEVELS_ERROR);
            goto EXIT;
        }

        // get the port for the address
        int port = multi_address_get_ip_port(config->addrs[i]);
        if (port == -1) {
            thl->log(thl, 0, "failed to get ip port from multiaddr",
                     LOG_LEVELS_ERROR);
            goto EXIT;
        }
        // store port number as a char *
        sprintf(cport, "%i", port);

        bool is_tcp = false;
        bool is_udp = false;

        // determine whether or not we support tcp and udp
        if (strstr(config->addrs[i]->string, "/tcp/") != NULL) {
            is_tcp = true;
        }
        if (strstr(config->addrs[i]->string, "/udp/") != NULL) {
            is_udp = true;
        }

        // if the multiaddr has neither a tcp or udp protocol, exit
        if (is_tcp == false && is_udp == false) {
            thl->log(thl, 0, "invalid multi_address provided", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        // handle a tcp multi_address
        if (is_tcp) {

            addr_info *tcp_bind_address = NULL;

            memset(&tcp_hints, 0, sizeof(tcp_hints));
            tcp_hints.ai_family = AF_INET;
            tcp_hints.ai_socktype = SOCK_STREAM;
            /*! @warning support non wildcard
             * @todo support non wildcard
             */
            tcp_hints.ai_flags = AI_PASSIVE;

            rc = getaddrinfo(ip, cport, &tcp_hints, &tcp_bind_address);
            if (rc != 0) {
                freeaddrinfo(tcp_bind_address);
                thl->log(thl, 0, "failed to get tcp addr info", LOG_LEVELS_ERROR);
                goto EXIT;
            }

            int tcp_socket_num = get_new_socket(thl, tcp_bind_address, sock_opts,
                                                num_opts, false, true);
            if (tcp_socket_num == -1) {
                freeaddrinfo(tcp_bind_address);
                thl->log(thl, 0, "failed to get new tcp socket", LOG_LEVELS_ERROR);
                goto EXIT;
            }

            listen(tcp_socket_num, config->max_connections);
            if (errno != 0) {
                freeaddrinfo(tcp_bind_address);
                thl->logf(thl, 0, LOG_LEVELS_ERROR,
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

            freeaddrinfo(tcp_bind_address);
        }

        // handle a udp multi_address
        if (is_udp) {

            addr_info *udp_bind_address = NULL;

            memset(&udp_hints, 0, sizeof(udp_hints));
            udp_hints.ai_family = AF_INET;
            udp_hints.ai_socktype = SOCK_DGRAM;
            /*! @warning support non wildcard
             * @todo support non wildcard
             */
            udp_hints.ai_flags = AI_PASSIVE;

            rc = getaddrinfo(ip, cport, &udp_hints, &udp_bind_address);
            if (rc != 0) {
                freeaddrinfo(udp_bind_address);
                thl->log(thl, 0, "failed to get udp addr info", LOG_LEVELS_ERROR);
                goto EXIT;
            }

            int udp_socket_num = get_new_socket(thl, udp_bind_address, sock_opts,
                                                num_opts, false, false);
            if (udp_socket_num == -1) {
                freeaddrinfo(udp_bind_address);
                thl->log(thl, 0, "failed to get new udp socket", LOG_LEVELS_ERROR);
                goto EXIT;
            }

            FD_SET(udp_socket_num, &udp_socket_set);
            FD_SET(udp_socket_num, &grouped_socket_set);

            if (udp_socket_num > max_socket_num) {
                max_socket_num = udp_socket_num;
            }

            if (config->recv_timeout_sec > 0) {
                set_socket_recv_timeout(udp_socket_num, config->recv_timeout_sec);
            }

            freeaddrinfo(udp_bind_address);
        }
    }

    socket_server_t *server = calloc(1, sizeof(socket_server_t));
    if (server == NULL) {
        thl->log(thl, 0, "failed to calloc socket server", LOG_LEVELS_ERROR);
        goto EXIT;
    }

    // increase max socket number by 1 for select usage
    max_socket_num += 1;

    // setup the server
    server->thpool = thpool_init(config->num_threads);
    server->max_socket_num = max_socket_num;
    server->grouped_socket_set = grouped_socket_set;
    server->tcp_socket_set = tcp_socket_set;
    server->udp_socket_set = udp_socket_set;
    server->task_func_tcp = config->fn_tcp;
    server->task_func_udp = config->fn_udp;
    server->thl = thl;
    pthread_mutex_init(&shutdown_mutex, NULL);
    server->thl->log(server->thl, 0, "initialized server", LOG_LEVELS_INFO);

    return server;

EXIT:

    for (int i = 0; i < max_socket_num; i++) {
        if (FD_ISSET(i, &tcp_socket_set)) {
            close(i);
        }
        if (FD_ISSET(i, &udp_socket_set)) {
            close(i);
        }
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    srv->thl->log(srv->thl, 0, "closing sockets", LOG_LEVELS_INFO);
    for (int i = 0; i < srv->max_socket_num; i++) {
        if (FD_ISSET(i, &srv->tcp_socket_set)) {
            close(i);
        }
        if (FD_ISSET(i, &srv->udp_socket_set)) {
            close(i);
        }
    }
    srv->thl->log(srv->thl, 0, "waiting for existing tasks to exit",
                  LOG_LEVELS_INFO);
    thpool_destroy(srv->thpool);
    srv->thl->log(srv->thl, 0, "all taskes exited, goodbye", LOG_LEVELS_INFO);
    pthread_mutex_destroy(&shutdown_mutex);
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
            srv->thl->log(srv->thl, 0, "shutdown signal received, exiting",
                          LOG_LEVELS_INFO);
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
                srv->thl->logf(srv->thl, 0, LOG_LEVELS_ERROR,
                               "an error occured while running select: %s",
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
                    chdata->is_tcp = true;
                    thpool_add_work(srv->thpool, srv->task_func_tcp, chdata);
                }

                // if it is a udp socket, handle it with the udp worker func
                if (FD_ISSET(i, &srv->udp_socket_set)) {

                    client_conn_t *conn = calloc(1, sizeof(client_conn_t));
                    if (conn == NULL) {
                        srv->thl->log(srv->thl, 0, "failed to calloc client_t",
                                      LOG_LEVELS_ERROR);
                        sleep(0.50);
                        continue;
                    }
                    conn->socket_number = i;

                    conn_handle_data_t *chdata =
                        calloc(1, sizeof(conn_handle_data_t));
                    if (chdata == NULL) {
                        free(conn);
                        srv->thl->log(srv->thl, 0, "failed to calloc client_t",
                                      LOG_LEVELS_ERROR);
                        sleep(0.50);
                        continue;
                    }

                    chdata->srv = srv;
                    chdata->conn = conn;
                    chdata->is_tcp = false;

                    thpool_add_work(srv->thpool, srv->task_func_udp, chdata);
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
    char *addr_inf = get_name_info((sock_addr *)&addr_temp);
    srv->thl->logf(srv->thl, 0, LOG_LEVELS_INFO, "accepted new connection: %s",
                   addr_inf);
    free(addr_inf);
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
    bool failed = false;
    int rc = 0;
    // read the first byte to determine the size of the buffer
    char first_byte[1];

    for (;;) {

        memset(first_byte, 0, 1);

        // check to see if we shoudl exit
        if (do_shutdown == true) {
            goto RETURN;
        }

        if (hdata->is_tcp == true) {
            // rc = read(hdata->conn->socket_number, first_byte, 1);
            rc = recv(hdata->conn->socket_number, first_byte, 1, 0);
        } else {
            rc = recvfrom(hdata->conn->socket_number, first_byte, 1, 0, NULL, NULL);
        }

        failed = recv_or_send_failed(hdata->srv, rc);
        if (failed == true) {
            goto RETURN;
        }

        /*!
         * @brief first attempt to derive the size by using atoi
         * @brief occasionally this will fail and give 0, even though
         * @brief there is a non-zero value there
         * @brief in cases where it fails casting to int returns the correct value
         * @warning in certain cases casting to int returns the wrong value too
         */
        int message_size = atoi(first_byte);
        if (message_size == 0) {
            message_size = (int)first_byte[0];
            if (message_size == 0) {
                goto RETURN;
            }
        }

        if (message_size <= 0 || message_size > 8192) {
            hdata->srv->thl->logf(hdata->srv->thl, 0, LOG_LEVELS_DEBUG,
                                  "invalid message size");
            // invalid first byte skip
            goto RETURN;
        }

        // declare an array on stack to hold the rest of the message data
        unsigned char message_data[message_size];
        memset(message_data, 0, message_size);

        if (hdata->is_tcp == true) {
            rc = read(hdata->conn->socket_number, message_data, message_size);
        } else {
            rc = recvfrom(hdata->conn->socket_number, message_data, message_size, 0,
                          NULL, NULL);
        }

        failed = recv_or_send_failed(hdata->srv, rc);
        if (failed == true) {
            goto RETURN;
        }

        if (message_size == 6) {
            if (memcmp(message_data, "hello", message_size) == 0) {
                printf("debug handler\n");
                printf("size: %i, data: %s\n", message_size, message_data);
                goto RETURN;
            }
        }
        cbor_encoded_data_t *cbdata =
            new_cbor_encoded_data(message_data, (size_t)message_size);
        if (cbdata == NULL) {
            goto RETURN;
        }
        message_t *msg = cbor_decode_message_t(cbdata);
        if (msg == NULL) {
            free_cbor_encoded_data(cbdata);
            goto RETURN;
        }

        printf("cbor decoded message data: %s\n", msg->data);

        free_message_t(msg);
        free_cbor_encoded_data(cbdata);
    }
RETURN:

    hdata->srv->thl->logf(hdata->srv->thl, 0, LOG_LEVELS_INFO,
                          "terminating connection. type tcp: %i", hdata->is_tcp);

    // if TCP, close the connection to the client socket
    if (hdata->is_tcp == true) {
        close(hdata->conn->socket_number);
    }
    if (hdata != NULL) {
        free(hdata->conn);
        free(hdata);
    }
}

/*!
 * @brief used to check if a receive or send with a socket failed
 */
bool recv_or_send_failed(socket_server_t *srv, int rc) {
    switch (rc) {
        case 0:
            srv->thl->log(srv->thl, 0, "client disconnected", LOG_LEVELS_DEBUG);
            return true;
        case -1:
            srv->thl->logf(srv->thl, 0, LOG_LEVELS_ERROR,
                           "error encountered during read %s", strerror(errno));
            return true;
        default:
            // connection was successful and we read some data
            return false;
    }
}

/*!
 * @brief used to specify which syscall signals should trigger shutdown process
 */
void setup_signal_shutdown(int signals[], int num_signals) {
    for (int i = 0; i < num_signals; i++) {
        signal(signals[i], signal_shutdown);
    }
}