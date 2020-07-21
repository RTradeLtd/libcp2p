/*! @file socket_client.h
  * @brief stuff related to socket clients (ie, connecting to socket servers)
*/

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include "socket.h"
#include "utils/logger.h"

/*! @typedef socket_client
  * @struct socket_client
  * a generic tcp/udp socket client
*/
typedef struct socket_client {
    int socket_number;
} socket_client_t;


/*! @brief returns a new socket client connected to `addr:port`
*/
socket_client_t *new_socket_client(thread_logger *thl, addr_info hints, char *addr, char *port);