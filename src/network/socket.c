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

/*! @file socket.c
 * @brief general socket related tooling
 */

#include "network/socket.h"
#include "thirdparty/logger/logger.h"
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
 * @brief sets a socket recv timeout
 * @param fd the file descriptor of the socket to apply operations to
 * @param seconds the seconds to timeout a recv or recvfrom after
 * @warning how does this workon UDP socket
 */
int set_socket_recv_timeout(int fd, int seconds) {
    timeout tmt;
    tmt.tv_sec = seconds;
    tmt.tv_usec = 0;
    int rc =
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tmt, sizeof(tmt));
    if (rc == -1) {
        printf("failed to set socket opt: %s\n", strerror(errno));
    }
    return rc;
}

/*! @brief  gets an available socket attached to bind_address
 * @return Success: file descriptor socket number greater than 0
 * @return Failure: -1
 * initializers a socket attached to bind_address with sock_opts, and binds the
 * address
 */
int get_new_socket(thread_logger *thl, addr_info *bind_address,
                   SOCKET_OPTS sock_opts[], int num_opts, bool is_client,
                   bool is_tcp) {
    // creates the socket and gets us its file descriptor
    int listen_socket_num =
        socket(bind_address->ai_family, bind_address->ai_socktype,
               bind_address->ai_protocol);
    if (listen_socket_num <= 0) {
        LOG_ERROR(thl, 0, "socket creation failed");
        return -1;
    }
    int one;
    int rc;
    bool passed;
    for (int i = 0; i < num_opts; i++) {
        switch (sock_opts[i]) {
            case REUSEADDR:
                one = 1;
                // set socket options before doing anything else
                // i tried setting it after listen, but I don't think that works
                rc = setsockopt(listen_socket_num, SOL_SOCKET, SO_REUSEADDR, &one,
                                sizeof(int));
                if (rc != 0) {
                    LOG_ERROR(thl, 0, "failed to set socket reuse addr");
                    return -1;
                }
                LOG_INFO(thl, 0, "set socket opt REUSEADDR");
                break;
            case BLOCK:
                passed = set_socket_blocking_status(listen_socket_num, true);
                if (passed == false) {
                    LOG_ERROR(thl, 0, "failed to set socket blocking mode");
                    return -1;
                }
                LOG_INFO(thl, 0, "set socket opt BLOCK");
                break;
            case NOBLOCK:
                passed = set_socket_blocking_status(listen_socket_num, false);
                if (passed == false) {
                    LOG_ERROR(thl, 0, "failed to set socket blocking mode");
                    return -1;
                }
                LOG_INFO(thl, 0, "set socket opt NOBLOCK");
                break;
            default:
                LOG_ERROR(thl, 0, "invalid socket option");
                return -1;
        }
    }

    // if client skip bind
    if (is_client == true) {
        if (is_tcp) {
            /*! @todo should we not do this on UDP connections?? */
            rc = connect(listen_socket_num, bind_address->ai_addr,
                         bind_address->ai_addrlen);
            if (rc != 0) {
                close(listen_socket_num);
                return -1;
            }
        }
        return listen_socket_num;
    }

    // binds the address to the socket
    bind(listen_socket_num, bind_address->ai_addr, bind_address->ai_addrlen);
    if (errno != 0) {
        LOGF_ERROR(thl, 0, "socket bind failed with error %s", strerror(errno));
        return -1;
    }
    return listen_socket_num;
}

/*! @brief used to enable/disable blocking sockets
 * @return Failure: false
 * @return Success: true
 * @note see
 * https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking/1549344#1549344
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
 * @note this only works with tcp
 * @todo enable udp
 */
char *get_name_info(sock_addr *client_address) {
    char address_info[256]; // destroy when function returns
    getnameinfo(client_address, sizeof(*client_address),
                address_info,         // output buffer
                sizeof(address_info), // size of the output buffer
                0,                    // second buffer which outputs service name
                0,                    // length of the second buffer
                NI_NUMERICHOST        // want to see hostnmae as an ip address
    );
    char *addr = calloc(1, sizeof(address_info));
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

/*!
 * @brief used to check if a receive or send with a socket failed
 */
bool recv_or_send_failed(thread_logger *thl, int rc) {
    switch (rc) {
        case 0:
            if (thl != NULL) {
                LOG_DEBUG(thl, 0, "client disconnected");
            }
            return true;
        case -1:
            if (thl != NULL) {
                LOGF_DEBUG(thl, 0, "error encountered during read %s",
                           strerror(errno));
            }
            return true;
        default:
            // connection was successful and we read some data
            return false;
    }
}

/*!
 * @brief returns an addr_info representation of the multiaddress
 * @details useful for taking a multi address and getting the needed information for
 * using
 * @details the address with the sendto function
 * @param address the multi address to parse
 * @note does not free up resources associated with address param
 * @warning only supports TCP and UDP multiaddress(es)
 * @return Success: pointer to an addr_info instance
 * @return Failure: NULL pointer
 */
addr_info *multi_addr_to_addr_info(multi_addr_t *address) {
    bool is_udp = false;
    bool is_tcp = false;
    addr_info hints;
    memset(&hints, 0, sizeof(addr_info));

    if (strstr(address->string, "/tcp/") != NULL) {
        hints.ai_socktype = SOCK_STREAM;
        is_tcp = true;
    }

    if (strstr(address->string, "/udp/") != NULL) {
        hints.ai_socktype = SOCK_DGRAM;
        is_udp = true;
    }

    if (is_udp == false && is_tcp == false) {
        return NULL;
    }

    char ip[1024];
    char cport[7];
    memset(ip, 0, 1024);
    memset(cport, 0, 1024);

    int rc = multi_address_get_ip_address(address, ip);
    if (rc != 1) {
        return NULL;
    }

    int port = multi_address_get_ip_port(address);
    if (port == -1) {
        return NULL;
    }
    sprintf(cport, "%i", port);

    int ip_family = multi_address_get_ip_family(address);
    if (ip_family == 0) {
        return NULL;
    }

    hints.ai_family = ip_family;
    hints.ai_flags = AI_PASSIVE;

    addr_info *ret_address;

    rc = getaddrinfo(ip, cport, &hints, &ret_address);
    if (rc != 0) {
        freeaddrinfo(ret_address);
        return NULL;
    }

    return ret_address;
}