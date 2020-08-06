#include "network/utils.h"
#include "network/socket.h"
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

// from
// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c/9142150#9142150

/*!
 * @brief utility function to send an int through a socket
 * @details handles situations where we may send less than expected bytes
 * @return Success: 0
 * @return Failure: -1
 */
int send_int_tcp(int num, int fd) {
    int32_t conv = htonl(num);
    char *data = (char *)&conv;
    int left = sizeof(conv);
    int rc;
    do {
        if (do_shutdown == true) {
            return -1;
        }
        rc = send(fd, data, left, 0);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }

    } while (left > 0);
    return 0;
}

/*!
 * @brief utility function to read an int through a socket
 * @details handles situations where we may read less than expected bytes
 * @return Success: 0
 * @return Failure: -1
 */
int receive_int_tcp(int *num, int fd) {
    int32_t ret;
    char *data = (char *)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        if (do_shutdown == true) {
            return -1;
        }
        rc = recv(fd, data, left, 0);
        if (rc <= 0) { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);
    *num = ntohl(ret);
    return 0;
}