/*! @file socket_server.h
  * @brief used to create a tcp/udp socket server listening on multiaddrs
*/

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include "socket_client.h" // this also imports socket.h

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

/*! @brief returns a new socket server bound to the port number and ready to accept connections
*/
socket_server_t *new_socket_server(addr_info hints, socket_server_config_t config);

/*! @brief listens for new connections and spawns a thread to process the connection
  * thread that is created to process the connection runs as a detached thread
  * will poll for new connections to accept every 500 miliseconds
  * @param data void pointer to a socket_server struct
  * @note detached thread created calling async_handle_conn_func
  * @warning may change the 500 milisecond sleep
*/
void *async_listen_func(void *data);

/*! @brief handles connections in a dedicated pthread   
  * is laucnched in a pthread by async_listen_func when any new connection is received
  * @param data `void *` to a conn_handle_data object
  * @note uses `select` to determine if we can read data from the connection
  * @note select runs for 3 seconds before timing out and releasing resources with the connection
  * @warning currently implements an example echo client
  * @warning you will want to adapt to your specific use case
*/
void *async_handle_conn_func(void *data);

/*! @brief helper function for accepting client connections
  * times out new attempts if they take 3 seconds or more
  * @return Failure: NULL client conn failed
  * @return Success: non-NULL populated client_conn object
*/
client_conn_t *accept_client_conn(socket_server_t *srv);