/*! @file socket_client.c
 * @brief client socket related tooling
 */

#include "network/socket_client.h"
#include "utils/logger.h"
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
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
/*! @brief returns a new socket client connected to `addr:port`
 */
socket_client_t *new_socket_client(thread_logger *thl, addr_info hints, char *addr,
                                   char *port) {
    addr_info *peer_address;
    int rc = getaddrinfo(addr, port, &hints, &peer_address);
    if (rc != 0) {
        freeaddrinfo(peer_address);
        return NULL;
    }

    int client_socket_num = get_new_socket(thl, peer_address, NULL, 0, true);
    if (client_socket_num == -1) {
        thl->log(thl, 0, "failed to get_new_socket", LOG_LEVELS_ERROR);
        freeaddrinfo(peer_address);
        return NULL;
    }

    socket_client_t *sock_client = calloc(sizeof(sock_client), sizeof(sock_client));
    if (sock_client == NULL) {
        thl->log(thl, 0, "failed to calloc socket_client_t", LOG_LEVELS_ERROR);
        freeaddrinfo(peer_address);
        return NULL;
    }
    sock_client->socket_number = client_socket_num;
    thl->log(thl, 0, "client successfully created", LOG_LEVELS_INFO);
    freeaddrinfo(peer_address);
    return sock_client;
}

/*!
  * @brief used to send a message through the connected socket number
  * @param client an instance of socket_client_t created with new_socket_client
  * @param peer_address the target address to connect to through the socket
  * @param message a null terminated pointer to a char
  * @returns Success: 1
  * @returns Failure: 0
*/
int socket_client_sendto(socket_client_t *client, addr_info *peer_address, char *message) {
    int bytes_sent = sendto(
        client->socket_number,
        message,
        strlen(message),
        0,
        peer_address->ai_addr,
        peer_address->ai_addrlen
    );
    if (bytes_sent == -1) {
        return 0;
    }
    /*! *@todo if we sent less than total size, send remaining
    */
   return 0;
}