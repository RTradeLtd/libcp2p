#include "utils/logger.h"
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
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

void *start_socker_server_wrapper(void *data) {
    socket_server_t *server = (socket_server_t *)data;
    start_socket_server(server, example_task_func_tcp, example_task_func_udp);
    pthread_exit(NULL);
}

void test_new_socket_server(void **state) {
    thread_logger *thl = new_thread_logger(false);
    socket_server_config_t config = {.listen_address = "127.0.0.1", .max_connections = 100, .tcp_port_number = "9090", .udp_port_number = "9091", .num_threads = 6 };
    socket_server_t *server = new_socket_server(thl, config);
    assert(server != NULL);
    pthread_t thread;
    pthread_create(&thread, NULL, start_socker_server_wrapper, server);

    addr_info hint;
    addr_info *peer_address = calloc(sizeof(addr_info), sizeof(addr_info));
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;
    int rc = getaddrinfo("127.0.0.1", "9091", &hint, &peer_address);
    assert(rc == 0);
    char *addr = get_name_info(peer_address);
    printf("client addr: %s\n", addr);

    socket_client_t *client = new_socket_client(thl, hint, "127.0.0.1", "9091");
    assert(client != NULL);

    socket_client_sendto(client, peer_address, "hello world\n");

    pthread_join(thread, NULL);
    // free_socket_server(server);
}

int main(void) {
    const struct CMUnitTest tests[] = {
       cmocka_unit_test(test_new_socket_server)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}