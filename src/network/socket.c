/*! @file socket.c
  * @brief general socket related tooling
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// sys/time.h is needed for the timeval
//  #include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include "network/socket.h"
#include "utils/logger.h"

/*! @brief  gets an available socket attached to bind_address
  * @return Success: file descriptor socket number greater than 0
  * @return Failure: -1
  * initializers a socket attached to bind_address with sock_opts, and binds the address
*/
int get_new_socket(thread_logger *thl, addr_info *bind_address, SOCKET_OPTS sock_opts[], int num_opts) {
     // creates the socket and gets us its file descriptor
    int listen_socket_num = socket(
        bind_address->ai_family,
        bind_address->ai_socktype,
        bind_address->ai_protocol
    );
    int one;
    int rc;
    bool passed;
    for (int i = 0; i < num_opts; i++) {
        switch (sock_opts[i]) {
            case REUSEADDR:
                one = 1;
                // set socket options before doing anything else
                // i tried setting it after listen, but I don't think that works
                rc = setsockopt(listen_socket_num, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
                if (rc != 0) {
                    thl->log(thl, 0, "failed to set socket reuse addr", LOG_LEVELS_ERROR);
                    return -1;
                }
                thl->log(thl, 0, "set socket opt REUSEADDR", LOG_LEVELS_INFO);
                break;
            case BLOCK:
                passed = set_socket_blocking_status(listen_socket_num, true);
                if (passed == false) {
                    thl->log(thl, 0, "failed to set socket blocking mode", LOG_LEVELS_ERROR);
                    return -1;
                }
                thl->log(thl, 0, "set socket opt BLOCK", LOG_LEVELS_INFO);
                break;
            case NOBLOCK:
                passed = set_socket_blocking_status(listen_socket_num, false);
                if (passed == false) {
                    thl->log(thl, 0, "failed to set socket blocking mode", LOG_LEVELS_ERROR);
                    return -1;
                }
                thl->log(thl, 0, "set socket opt NOBLOCK", LOG_LEVELS_INFO);
                break;
            default:
                thl->log(thl, 0, "invalid socket option", LOG_LEVELS_ERROR);
                return -1;
        }
    }
    // binds the address to the socket
    bind(
        listen_socket_num,
        bind_address->ai_addr,
        bind_address->ai_addrlen
    );
    if (errno != 0) {
        thl->logf(thl, 0, LOG_LEVELS_ERROR, "socket bind failed with error %s", strerror(errno));
        return -1;
    }
    return listen_socket_num;
}


/*! @brief used to enable/disable blocking sockets
  * @return Failure: false
  * @return Success: true
  * @note see https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking/1549344#1549344
*/
bool set_socket_blocking_status(int fd, bool blocking) {
    if (fd < 0) {
        return false;
    } else {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            return false;
        }
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
    }
}

/*! @brief returns the address the client is connecting from
*/
char  *get_name_info(sock_addr *client_address) {
    char address_info[256]; // destroy when function returns
    getnameinfo(
        client_address,
        sizeof(*client_address),
        address_info, // output buffer
        sizeof(address_info), // size of the output buffer
        0, // second buffer which outputs service name
        0, // length of the second buffer
        NI_NUMERICHOST    // want to see hostnmae as an ip address
    );
    char *addr = malloc(sizeof(address_info));
    if (addr == NULL) {
        return NULL;
    }
    strcpy(addr, address_info);
    return addr;
}

/*! @brief generates an addr_info struct with defaults
  * defaults is IPv4, TCP, and AI_PASSIVE flags
*/
addr_info default_hints() {
    addr_info hints;
    memset(&hints, 0, sizeof(hints));
    // change to AF_INET6 to use IPv6
    hints.ai_family = AF_INET;
    // indicates TCP, if you want UDP use SOCKT_DGRAM
    hints.ai_socktype = SOCK_STREAM;
    // indicates to getaddrinfo we want to bind to the wildcard address
    hints.ai_flags = AI_PASSIVE;
    return hints;
}