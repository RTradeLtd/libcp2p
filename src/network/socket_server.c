/*! @file socket_server.c
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#include "network/socket_server.h"
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

/*! @brief returns a new socket server bound to the port number and ready to accept
 * connections
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t config) {

    SOCKET_OPTS opts[2] = {REUSEADDR, NOBLOCK};

    addr_info tcp_hints;
    addr_info udp_hints;
    addr_info *tcp_bind_address = NULL;
    addr_info *udp_bind_address = NULL;

    int rc = 0;

    fd_set socket_set;

    FD_ZERO(&socket_set);

    for (int i = 0; i < config.num_addrs; i++) {
        char *ip = calloc(sizeof(unsigned char), 1024);
        rc = multiaddress_get_ip_address(&config.addrs[i], &ip);
        if (rc != 1) {
            thl->log(thl, 0, "failed to get ip address from multiaddr", LOG_LEVELS_ERROR);
            return NULL; /*! @todo free up existing sockets */
        }
        int port = multiaddress_get_ip_port(&config.addrs[i]);
        if (port == -1) {
            thl->log(thl, 0, "failed to get ip port from multiaddr", LOG_LEVELS_ERROR);
            return NULL; /*! @todo free up existing sockets */
        }
        bool is_tcp;
        bool is_udp;
        if (strstr((&config.addrs[i])->string, "/tcp/") != NULL) {
            is_tcp = true;
        }
        if (strstr((&config.addrs[i])->string, "/udp/") != NULL) {
            is_udp = true;
        }
        if (is_tcp == false && is_udp == false) {
            thl->log(thl, 0, "invalid multiaddress provided", LOG_LEVELS_ERROR);
            return NULL; /*! @todo free up existing sockets */
        }
        char *cport = multiaddress_get_ip_port_c(&config.addrs[i]);
        if (is_tcp) {
            rc = getaddrinfo(ip, cport, &tcp_hints, &tcp_bind_address);
            if (rc != 0) {
                thl->log(thl, 0, "failed to get tcp addr info", LOG_LEVELS_ERROR);
                return NULL; /*! @todo free up existing sockets */
            }
            int tcp_socket_num = get_new_socket(thl, tcp_bind_address, opts, 2, false);
            if (tcp_socket_num == -1) {
                thl->log(thl, 0, "failed to get new tcp socket", LOG_LEVELS_ERROR);
                return NULL; /*! @todo free up existing sockets */
            }

            listen(tcp_socket_num, config.max_connections);
            if (errno != 0) {
                thl->logf(thl, 0, LOG_LEVELS_ERROR,
                        "failed to start listening on tcp socket with error %s",
                        strerror(errno));
                return NULL; /*! @todo free up existing sockets */
            }
            FD_SET(tcp_socket_num, &socket_set);
            free(ip);
            free(cport);
        }
        if (is_udp) {
            rc = getaddrinfo(ip, cport, &udp_hints, &udp_bind_address);
            if (rc != 0) {
                thl->log(thl, 0, "failed to get udp addr info", LOG_LEVELS_ERROR);
                return NULL; /*! @todo free up existing sockets */
            }
            int udp_socket_num = get_new_socket(thl, udp_bind_address, opts, 2, false);
            if (udp_socket_num == -1) {
                thl->log(thl, 0, "failed to get new udp socket", LOG_LEVELS_ERROR);
                return NULL; /*! @todo free up existing sockets */
            }
            FD_SET(udp_socket_num, &socket_set);
            free(ip);
            free(cport);
        }
    }

    socket_server_t *server =
        calloc(sizeof(socket_server_t), sizeof(socket_server_t));
    if (server == NULL) {
        thl->log(thl, 0, "failed to calloc socket server", LOG_LEVELS_ERROR);
        goto EXIT;
    }

    server->thpool = thpool_init(config.num_threads);
    server->socket_set = socket_set;
    server->task_func_tcp = config.fn_tcp;
    server->task_func_udp = config.fn_udp;
    server->thl = thl;
    server->thl->log(server->thl, 0, "initialized server", LOG_LEVELS_INFO);

    freeaddrinfo(tcp_bind_address);
    freeaddrinfo(udp_bind_address);

    pthread_mutex_init(&shutdown_mutex, NULL);

    return server;

EXIT:

    if (tcp_bind_address != NULL) {
        freeaddrinfo(tcp_bind_address);
    }

    if (udp_bind_address != NULL) {
        freeaddrinfo(udp_bind_address);
    }

    for (int i = 0; i < 65536; i++) {
        if (FD_ISSET(i, &socket_set)) {
            close(i);
        }
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    srv->thl->log(srv->thl, 0, "closing sockets", LOG_LEVELS_INFO);
    for (int i = 0; i < 65536; i++) {
        if (FD_ISSET(i, &srv->socket_set)) {
            close(i);
        }
    }
    srv->thl->log(srv->thl, 0, "waiting for existing tasks to exit",
                  LOG_LEVELS_INFO);
    thpool_destroy(srv->thpool);
    srv->thl->log(srv->thl, 0, "all taskes exited, goodbye", LOG_LEVELS_INFO);
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
    fd_set socket_list;
    int max_socket_number;

    // initialize the fd_set
    FD_ZERO(&socket_list);
    for (int i = 0; i < 65536; i++) {
        if (FD_ISSET(i, &srv->socket_set)) {
            if (i > max_socket_number) {
                max_socket_number = i;
            }
        }
    }
    max_socket_number += 1;
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
        // create a temporary working copy copy of socket_list
        fd_set working_copy = srv->socket_set;

        int rc = select(max_socket_number, &working_copy, NULL, NULL, &tmt);

        switch (rc) {
            case 0:
                srv->thl->log(srv->thl, 0,
                              "no sockets are ready for processing, sleeping",
                              LOG_LEVELS_DEBUG);
                sleep(0.50);
                continue;
            case -1:
                srv->thl->logf(srv->thl, 0, LOG_LEVELS_ERROR,
                               "an error occured while running select: %s",
                               strerror(errno));
                sleep(0.50);
                return;
        }
        // sleep before looping again
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