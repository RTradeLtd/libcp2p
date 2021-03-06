#pragma once

#include "network/socket.h"

// from
// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c/9142150#9142150

/*!
 * @brief utility function to send an int through a socket
 * @details handles situations where we may send less than expected bytes
 * @return Success: 0
 * @return Failure: -1
 */
int send_int_tcp(int num, int fd);
/*!
 * @brief utility function to read an int through a socket
 * @details handles situations where we may read less than expected bytes
 * @return Success: 0
 * @return Failure: -1
 */
int receive_int_tcp(int *num, int fd);