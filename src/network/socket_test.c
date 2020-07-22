#include "utils/logger.h"
#include "network/socket_server.h"
#include "multiaddr/multiaddr.h"
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
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"


/*!
 * @brief example function used to showcase how you can udp connections
 * @note in general should accept a conn_handle_data_t type but this is implementation defined
*/
void example_task_func_udp(void *data) {
    conn_handle_data_t *hdata = (conn_handle_data_t *)data;
    sock_addr client_address;
    socklen_t len = sizeof(client_address);
    char buffer[2048];
    int bytes_received = recvfrom(
        hdata->conn->socket_number,
        buffer,
        2048,
        0,
        &client_address,
        &len
    );
    if (bytes_received == -1) {
        free(hdata->conn);
        free(hdata);
        return;
    }
    hdata->srv->thl->logf(
        hdata->srv->thl,
        0,
        LOG_LEVELS_INFO,
        "received message from client %s",
        buffer
    );
   free(hdata->conn);
   free(hdata);
}

/*!
 * @brief example function used to showcase how you can handle connections
 * @note in general should accept a conn_handle_data_t type but this is implementation define
*/
void example_task_func_tcp(void *data) {
    conn_handle_data_t *hdata = (conn_handle_data_t *)data;
    char buffer[2048];
    int rc = read(hdata->conn->socket_number, buffer, 2048);
    switch (rc) {
        case 0:
            hdata->srv->thl->log(hdata->srv->thl, 0, "client disconnected", LOG_LEVELS_DEBUG);
            goto EXIT;
        case -1:
            hdata->srv->thl->logf(hdata->srv->thl, 0, LOG_LEVELS_ERROR, "error encountered during read %s", strerror(errno));
            goto EXIT;
        default:
            // connection was successful and we read some data
            goto EXIT;
    }
    send(hdata->conn->socket_number, buffer, (size_t)rc, 0);
    /*! @todo figure out proper close procedures
    */
EXIT:
   close(hdata->conn->socket_number);
   free(hdata->conn);
   free(hdata);
}

void start_socker_server_wrapper(void *data) {
    socket_server_t *server = (socket_server_t *)data;
    start_socket_server(server);
}

/*!
  * @brief in this we reuse the thread pool to start the socket server listening process, but you will likely want to do this from your main thread
*/
void test_new_socket_server(void **state) {
    thread_logger *thl = new_thread_logger(false);
    socket_server_config_t *config = new_socket_server_config(2);
    config->max_connections = 100;
    config->num_threads = 6;
    config->fn_tcp = example_task_func_tcp;
    config->fn_udp = example_task_func_udp;

    multi_addr_t *tcp_addr = multiaddress_new_from_string("/ip4/127.0.0.1/tcp/9090");
    multi_addr_t *udp_addr = multiaddress_new_from_string("/ip4/127.0.0.1/udp/9091");
    // multi_addr_t *addrs[2] = {tcp_addr, udp_addr};
    config->addrs = calloc(sizeof(multi_addr_t), sizeof(tcp_addr) + sizeof(udp_addr));
    config->addrs[0] = tcp_addr;
    config->addrs[1] = udp_addr;
    config->num_addrs = 2;

    //   .num_threads = 6, .fn_tcp = example_task_func_tcp, .fn_udp = example_task_func_udp };
    socket_server_t *server = new_socket_server(thl, *config);
    assert(server != NULL);
    free_socket_server_config(config);
    thpool_add_work(server->thpool, start_socker_server_wrapper, server);

    addr_info hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;

    socket_client_t *client = new_socket_client(thl, hint, "127.0.0.1", "9091");
    assert(client != NULL);

    addr_info *peer_address;
    int rc = getaddrinfo("127.0.0.1", "9091", &hint, &peer_address);
    assert(rc == 0);

    socket_client_sendto(client, peer_address, "hello world\n");
    sleep(2);
    close(client->socket_number);
    free(client);
    freeaddrinfo(peer_address);
    signal_shutdown();
    sleep(5);
    free_socket_server(server);
    // free(config.addrs);
}

int main(void) {
    const struct CMUnitTest tests[] = {
       cmocka_unit_test(test_new_socket_server)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}