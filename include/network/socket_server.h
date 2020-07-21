/*! @file socket_server.h
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#pragma once

#include "network/socket_client.h" // this also imports socket.h
#include "utils/logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

typedef struct socket_server_config {
    char *udp_port_number;
    char *tcp_port_number;
    char *listen_address;
    int max_connections;
} socket_server_config_t;

typedef struct socket_server {
    int udp_socket_number;
    int tcp_socket_number;
    pthread_t thread;
    thread_logger *thl;
} socket_server_t;

/*! @typedef client_conn
 * @struct client_conn
 * @brief a structure containing a file descriptor and address information
 * @todo
 *   - enable a queue/list of these
 */
typedef struct client_conn {
    int socket_number;
    sock_addr_storage *address;
} client_conn_t;

/*! @typedef conn_handle_data
 * @struct conn_handle_data
 * @brief struct containing arguments passed into pthread
 */
typedef struct conn_handle_data {
    pthread_t thread;
    socket_server_t *srv;
    client_conn_t *conn;
} conn_handle_data_t;

/*!
  * @brief defines a function signature that is used for processing new connections
*/
typedef void *(handle_conn_func)(void *data);


/*! @brief returns a new socket server bound to the port number and ready to accept
 * connections
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t config);

/*! @brief listens for new connections and spawns a thread to process the connection
 * thread that is created to process the connection runs as a detached thread
 * will poll for new connections to accept every 500 miliseconds
 * @param data void pointer to a socket_server struct
 * @note detached thread created calling async_handle_conn_func
 * @warning may change the 500 milisecond sleep
 */
void *async_listen_func(void *data);

/*! @brief helper function for accepting client connections
 * times out new attempts if they take 3 seconds or more
 * @return Failure: NULL client conn failed
 * @return Success: non-NULL populated client_conn object
 */
client_conn_t *accept_client_conn(socket_server_t *srv);

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv);

/*!
  * @brief starts the socket server which processes new connections
  * @details when a new connection is accepted (tcp) OR we can receive data on a udp socket, the given handle_conn_func is used to process that client connection
  * @param srv an instance of a socket_server_t that has been initialized through new_socket_server
  * @param fn the function use to handle new connections
*/
void start_socket_socker(socket_server_t *srv, handle_conn_func *fn);

/*!
  * @brief an example function show cases how to use handle_conn_func
  * @details showcases how to use start_socket_server and handle_conn_func to create a TCP/UDP socket server that echos data back to client
  * @warning should not be used for actual production uses
*/
void *example_handle_conn_func(void *data);