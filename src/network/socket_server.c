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

    int max_socket_num = 0;
    int rc = 0;

    fd_set grouped_socket_set;
    fd_set tcp_socket_set;
    fd_set udp_socket_set;

    FD_ZERO(&tcp_socket_set);
    FD_ZERO(&udp_socket_set);
    FD_ZERO(&grouped_socket_set);

    char *ip = calloc(sizeof(char), 1024);
    for (int i = 0; i < config.num_addrs; i++) {
        memset(ip, 0, 1024);
        rc = multiaddress_get_ip_address(&config.addrs[i], &ip);
        if (rc != 1) {
            thl->log(thl, 0, "failed to get ip address from multiaddr",
                     LOG_LEVELS_ERROR);
            free(ip);
            goto EXIT;
        }
        int port = multiaddress_get_ip_port(&config.addrs[i]);
        if (port == -1) {
            thl->log(thl, 0, "failed to get ip port from multiaddr",
                     LOG_LEVELS_ERROR);
            free(ip);
            goto EXIT;
        }
        bool is_tcp = false;
        bool is_udp = false;
        if (strstr((&config.addrs[i])->string, "/tcp/") != NULL) {
            is_tcp = true;
        }
        if (strstr((&config.addrs[i])->string, "/udp/") != NULL) {
            is_udp = true;
        }
        if (is_tcp == false && is_udp == false) {
            thl->log(thl, 0, "invalid multiaddress provided", LOG_LEVELS_ERROR);
            free(ip);
            goto EXIT;
        }
        char *cport = multiaddress_get_ip_port_c(&config.addrs[i]);
        if (is_tcp) {
            addr_info *tcp_bind_address = NULL;
            memset(&tcp_hints, 0, sizeof(tcp_hints));
            tcp_hints.ai_family = AF_INET;
            tcp_hints.ai_socktype = SOCK_STREAM;
            /*! @warning support non wildcard
             * @todo support non wildcard
             */
            tcp_hints.ai_flags = AI_PASSIVE;
            rc = getaddrinfo(ip, cport, &tcp_hints, &tcp_bind_address);
            if (rc != 0) {
                thl->log(thl, 0, "failed to get tcp addr info", LOG_LEVELS_ERROR);
                free(ip);
                goto EXIT;
            }
            int tcp_socket_num =
                get_new_socket(thl, tcp_bind_address, opts, 2, false);
            if (tcp_socket_num == -1) {
                thl->log(thl, 0, "failed to get new tcp socket", LOG_LEVELS_ERROR);
                free(ip);
                goto EXIT;
            }

            listen(tcp_socket_num, config.max_connections);
            if (errno != 0) {
                thl->logf(thl, 0, LOG_LEVELS_ERROR,
                          "failed to start listening on tcp socket with error %s",
                          strerror(errno));
                free(ip);
                goto EXIT;
            }
            FD_SET(tcp_socket_num, &tcp_socket_set);
            FD_SET(tcp_socket_num, &grouped_socket_set);
            if (tcp_socket_num > max_socket_num) {
                max_socket_num = tcp_socket_num;
            }
            free(tcp_bind_address);
        }
        if (is_udp) {
            addr_info *udp_bind_address = NULL;
            memset(&udp_hints, 0, sizeof(udp_hints));
            udp_hints.ai_family = AF_INET;
            udp_hints.ai_socktype = SOCK_DGRAM;
            /*! @warning support non wildcard
             * @todo support non wildcard
             */
            udp_hints.ai_flags = AI_PASSIVE;
            rc = getaddrinfo(ip, cport, &udp_hints, &udp_bind_address);
            if (rc != 0) {
                thl->log(thl, 0, "failed to get udp addr info", LOG_LEVELS_ERROR);
                free(ip);
                goto EXIT;
            }
            int udp_socket_num =
                get_new_socket(thl, udp_bind_address, opts, 2, false);
            if (udp_socket_num == -1) {
                thl->log(thl, 0, "failed to get new udp socket", LOG_LEVELS_ERROR);
                free(ip);
                goto EXIT;
            }
            FD_SET(udp_socket_num, &udp_socket_set);
            FD_SET(udp_socket_num, &grouped_socket_set);
            if (udp_socket_num > max_socket_num) {
                max_socket_num = udp_socket_num;
            }
            free(udp_bind_address);
        }
    }
    free(ip);
    socket_server_t *server =
        calloc(sizeof(socket_server_t), sizeof(socket_server_t));
    if (server == NULL) {
        thl->log(thl, 0, "failed to calloc socket server", LOG_LEVELS_ERROR);
        goto EXIT;
    }
    max_socket_num += 1;
    server->thpool = thpool_init(config.num_threads);
    server->max_socket_num = max_socket_num;
    server->grouped_socket_set = grouped_socket_set;
    server->tcp_socket_set = tcp_socket_set;
    server->udp_socket_set = udp_socket_set;
    server->task_func_tcp = config.fn_tcp;
    server->task_func_udp = config.fn_udp;
    server->thl = thl;
    server->thl->log(server->thl, 0, "initialized server", LOG_LEVELS_INFO);

    pthread_mutex_init(&shutdown_mutex, NULL);

    return server;

EXIT:
    free(ip);
    for (int i = 0; i < 65536; i++) {
        if (FD_ISSET(i, &tcp_socket_set)) {
            close(i);
        }
        if (FD_ISSET(i, &udp_socket_set)) {
            close(i);
        }
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    srv->thl->log(srv->thl, 0, "closing sockets", LOG_LEVELS_INFO);
    for (int i = 0; i < srv->max_socket_num; i++) {
        if (FD_ISSET(i, &srv->tcp_socket_set)) {
            close(i);
        }
        if (FD_ISSET(i, &srv->udp_socket_set)) {
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
        /*! @todo enable udp socket
         */
        fd_set working_copy = srv->grouped_socket_set;

        int rc = select(srv->max_socket_num, &working_copy, NULL, NULL, &tmt);

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

        for (int i = 0; i < srv->max_socket_num; i++) {
            if (FD_ISSET(i, &working_copy)) {
                if (FD_ISSET(i, &srv->tcp_socket_set)) {
                    client_conn_t *conn = accept_client_conn(srv, i);
                    if (conn == NULL) {
                        sleep(0.50);
                        continue;
                    }
                    conn_handle_data_t *chdata = calloc(sizeof(conn_handle_data_t),
                                                        sizeof(conn_handle_data_t));
                    chdata->srv = srv;
                    chdata->conn = conn;
                    thpool_add_work(srv->thpool, srv->task_func_tcp, chdata);
                }
                if (FD_ISSET(i, &srv->udp_socket_set)) {
                    client_conn_t *conn =
                        calloc(sizeof(client_conn_t), sizeof(client_conn_t));
                    if (conn == NULL) {
                        srv->thl->log(srv->thl, 0, "failed to calloc client_t",
                                      LOG_LEVELS_ERROR);
                        sleep(0.50);
                        continue;
                    }
                    conn->socket_number = i;
                    conn_handle_data_t *chdata = calloc(sizeof(conn_handle_data_t),
                                                        sizeof(conn_handle_data_t));
                    if (chdata == NULL) {
                        free(conn);
                        srv->thl->log(srv->thl, 0, "failed to calloc client_t",
                                      LOG_LEVELS_ERROR);
                        sleep(0.50);
                        continue;
                    }
                    chdata->srv = srv;
                    chdata->conn = conn;
                    thpool_add_work(srv->thpool, srv->task_func_udp, chdata);
                }
            }
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

/*! @brief helper function for accepting client connections
 * times out new attempts if they take 3 seconds or more
 * @return Failure: NULL client conn failed
 * @return Success: non-NULL populated client_conn object
 */
client_conn_t *accept_client_conn(socket_server_t *srv, int socket_num) {
    // temporary variable for storing socket address
    sock_addr_storage addr_temp;
    // set client_len
    // i tried doing `(sock_addr *)&sizeof(addr_temp)
    // in the `accept` function call but it didnt work
    socklen_t client_len = sizeof(addr_temp);
    int client_socket_num = accept(socket_num, (sock_addr *)&addr_temp, &client_len);
    // socket number less than 0 is an error
    if (client_socket_num < 0) {
        return NULL;
    }
    client_conn_t *connection = calloc(sizeof(client_conn_t), sizeof(client_conn_t));
    if (connection == NULL) {
        return NULL;
    }
    connection->address = &addr_temp;
    connection->socket_number = client_socket_num;
    char *addr_inf = get_name_info((sock_addr *)connection->address);
    srv->thl->logf(srv->thl, 0, LOG_LEVELS_INFO, "accepted new connection: %s",
                   addr_inf);
    free(addr_inf);
    return connection;
}