/*! @file socket_server.c
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#include "utils/thread_pool.h"
#include "network/socket_server.h"
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

    bool tcp_set = false;
    bool udp_set = false;

    int tcp_socket_num = 0;
    int udp_socket_num = 0;
    int rc = 0;

    if (config.tcp_port_number != NULL) {
        memset(&tcp_hints, 0, sizeof(tcp_hints));
        tcp_hints.ai_family = AF_INET;
        tcp_hints.ai_socktype = SOCK_STREAM;
        /*! @warning support non wildcard
         * @todo support non wildcard
         */
        tcp_hints.ai_flags = AI_PASSIVE;
        tcp_set = true;
    }

    if (config.udp_port_number != NULL) {
        memset(&udp_hints, 0, sizeof(udp_hints));
        udp_hints.ai_family = AF_INET;
        udp_hints.ai_socktype = SOCK_DGRAM;
        /*! @warning support non wildcard
         * @todo support non wildcard
         */
        udp_hints.ai_flags = AI_PASSIVE;
        udp_set = true;
    }

    // setup tcp socket
    if (tcp_set == true) {

        rc = getaddrinfo(0, config.tcp_port_number, &tcp_hints, &tcp_bind_address);
        if (rc != 0) {
            thl->log(thl, 0, "failed to get tcp addr info", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        tcp_socket_num = get_new_socket(thl, tcp_bind_address, opts, 2);
        if (tcp_socket_num == -1) {
            thl->log(thl, 0, "failed to get new tcp socket", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        listen(tcp_socket_num, config.max_connections);
        if (errno != 0) {
            thl->logf(thl, 0, LOG_LEVELS_ERROR,
                      "failed to start listening on tcp socket with error %s",
                      strerror(errno));
            goto EXIT;
        }

        thl->log(thl, 0, "accepting tcp connections", LOG_LEVELS_INFO);
    }

    // setup udp socket
    if (udp_set == true) {

        rc = getaddrinfo(0, config.udp_port_number, &udp_hints, &udp_bind_address);
        if (rc != 0) {
            thl->log(thl, 0, "failed to get udp addr info", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        udp_socket_num = get_new_socket(thl, udp_bind_address, opts, 2);
        if (udp_socket_num == -1) {
            thl->log(thl, 0, "failed to get new udp socket", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        thl->log(thl, 0, "accepting udp connections", LOG_LEVELS_INFO);
    }

    socket_server_t *server =
        calloc(sizeof(socket_server_t), sizeof(socket_server_t));
    if (server == NULL) {
        thl->log(thl, 0, "failed to calloc socket server", LOG_LEVELS_ERROR);
        goto EXIT;
    }

    server->thpool = thpool_init(config.num_threads);
    server->udp_socket_number = udp_socket_num;
    server->tcp_socket_number = tcp_socket_num;
    server->thl = thl;
    server->thl->log(server->thl, 0, "initialized server", LOG_LEVELS_INFO);

    

    freeaddrinfo(tcp_bind_address);
    freeaddrinfo(udp_bind_address);

    return server;

EXIT:

    if (tcp_bind_address != NULL) {
        freeaddrinfo(tcp_bind_address);
    }

    if (udp_bind_address != NULL) {
        freeaddrinfo(udp_bind_address);
    }

    if (tcp_socket_num != 0) {
        close(tcp_socket_num);
    }

    if (udp_socket_num != 0) {
        close(udp_socket_num);
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    if (srv->tcp_socket_number > 0) {
        close(srv->tcp_socket_number);
        srv->thl->log(srv->thl, 0, "shut down tcp socket", LOG_LEVELS_INFO);
    }
    if (srv->udp_socket_number > 0) {
        close(srv->udp_socket_number);
        srv->thl->log(srv->thl, 0, "shut down udp socket", LOG_LEVELS_INFO);
    }
    /*!
      * @todo should we wait before destroy?
    */
    // thpool_wait(srv->thpool);
    thpool_destroy(srv->thpool);
    srv->thl->log(srv->thl, 0, "server shutdown, goodbye", LOG_LEVELS_INFO);
    clear_thread_logger(srv->thl);
    free(srv);
}

/*!
  * @brief starts the socket server which processes new connections
  * @details when a new connection is accepted (tcp) OR we can receive data on a udp socket, the given threadpool_task_func is used to process that client connection
  * @param srv an instance of a socket_server_t that has been initialized through new_socket_server
  * @param fn the function use to handle new connections
  * @note this is a blocking call, if you want it to be non blocking launch it in a pthread
*/
void start_socket_socker(socket_server_t *srv, threadpool_task_func *fn) {
    // set the task function to process new connections
    srv->task_func = fn;
    fd_set socket_list;
    int max_socket_number;

    // initialize the fd_set
    FD_ZERO(&socket_list);

    if (srv->tcp_socket_number > 0 && srv->tcp_socket_number > srv->udp_socket_number) {
        max_socket_number = srv->tcp_socket_number + 1;
    } else if (srv->udp_socket_number > 0 && srv->udp_socket_number > srv->tcp_socket_number) {
        max_socket_number = srv->udp_socket_number + 1;
    }

    if (srv->tcp_socket_number > 0) {
        FD_SET(srv->tcp_socket_number, &socket_list);
    }
    if (srv->udp_socket_number > 0) {
        FD_SET(srv->udp_socket_number, &socket_list);
    }

   for (;;) {
        /*!
         * @todo enable customizable timeout
        */
        timeout tmt;
        tmt.tv_sec = 3;
        tmt.tv_usec = 0;
        // create a temporary working copy copy of socket_list
        fd_set working_copy = socket_list;

        int rc = select(max_socket_number, &working_copy, NULL, NULL, &tmt);
        
        switch (rc) {
            case 0:
                srv->thl->log(srv->thl, 0, "no sockets are ready for processing, sleeping", LOG_LEVELS_DEBUG);
                sleep(0.50);
                break;
            case -1:
                srv->thl->logf(srv->thl, 0, LOG_LEVELS_ERROR, "an error occured while running select: %s", strerror(errno));
                sleep(0.50);
                break;
        }

        /*!
         * @todo figure out why select isn't handling the udp connection
         * @warning does not work with udp
        */
        if (FD_ISSET(srv->udp_socket_number, &working_copy)) {
            printf("udp connection available\n");
        }

        // accept tcp connections if they are available
        if (FD_ISSET(srv->tcp_socket_number, &working_copy)) {
            client_conn_t *conn = accept_client_conn(srv);
            if (conn == NULL) {
                printf("failed to accept connection\n");
                continue;
            }
            conn_handle_data_t *chdata = calloc(sizeof(conn_handle_data_t), sizeof(conn_handle_data_t));
            chdata->srv = srv;
            chdata->conn = conn;
            thpool_add_work(srv->thpool, srv->task_func, chdata);
        }
        
        /*! @todo process udp connections */
   }
}

/*!
 * @brief example function used to showcase how you can handle connections
 * @note in general should accept a conn_handle_data_t type but this is implementation define
*/
void example_task_func(void *data) {
    conn_handle_data_t *hdata = (conn_handle_data_t *)data;
    hdata->srv->thl->log(hdata->srv->thl, 0, "waiting to receive data", LOG_LEVELS_INFO);
    char buffer[2048];
    int rc = read(hdata->conn->socket_number, buffer, 2048);
    switch (rc) {
        case 0:
            hdata->srv->thl->log(hdata->srv->thl, 0, "client disconnected", LOG_LEVELS_DEBUG);
            return;
        case -1:
            hdata->srv->thl->logf(hdata->srv->thl, 0, LOG_LEVELS_ERROR, "error encountered during read %s", strerror(errno));
            return;
        default:
            // connection was successful and we read some data
            break;
    }
    send(hdata->conn->socket_number, buffer, (size_t)rc, 0);
    /*! @todo figure out proper close procedures
    */
   close(hdata->conn->socket_number);
   free(hdata->conn);
   free(hdata);
}

/*! @brief helper function for accepting client connections
  * times out new attempts if they take 3 seconds or more
  * @return Failure: NULL client conn failed
  * @return Success: non-NULL populated client_conn object
*/
client_conn_t *accept_client_conn(socket_server_t *srv) {
    // temporary variable for storing socket address
    sock_addr_storage addr_temp;
    // set client_len
    // i tried doing `(sock_addr *)&sizeof(addr_temp)
    // in the `accept` function call but it didnt work
    socklen_t client_len = sizeof(addr_temp);
    int client_socket_num = accept(
        srv->tcp_socket_number,
        (sock_addr *)&addr_temp,
        &client_len
    );
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
    char *addr_info = get_name_info((sock_addr *)connection->address);
    srv->thl->logf(srv->thl, 0, LOG_LEVELS_INFO, "accepted new connection: %s", addr_info);
    free(addr_info);
    return connection;
}