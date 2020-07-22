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

#pragma GCC diagnostic ignored "-Wunused-parameter"
/*! @brief returns a new socket client connected to `addr:port`
 */
socket_client_t *new_socket_client(thread_logger *thl, addr_info hints, char *addr,
                                   char *port) {
    addr_info *peer_address;
    int rc = getaddrinfo(addr, port, &hints, &peer_address);
    if (rc != 0) {
        return NULL;
    }
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer,
                sizeof(address_buffer), service_buffer, sizeof(service_buffer), 0);
    printf("address buff: %s\n", address_buffer);
    printf("service buffer: %s\n", service_buffer);
    int client_socket_num = get_new_socket(thl, peer_address, NULL, 0);
    if (client_socket_num == -1) {
        thl->log(thl, 0, "failed to get_new_socket", LOG_LEVELS_ERROR);
        return NULL;
    }
    printf("socket num: %i\n", client_socket_num);
    socket_client_t *sock_client = malloc(sizeof(sock_client));
    if (sock_client == NULL) {
        return NULL;
    }
    sock_client->socket_number = client_socket_num;
    return sock_client;
}