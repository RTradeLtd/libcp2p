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


void test_new_socket_server(void **state) {
    thread_logger *thl = new_thread_logger(false);
    socket_server_config_t config = {.listen_address = "127.0.0.1", .max_connections = 100, .tcp_port_number = "9090", .udp_port_number = "9091", .num_threads = 6 };
    socket_server_t *server = new_socket_server(thl, config);
    assert(server != NULL);
    printf("%i\n", server->tcp_socket_number);
    printf("%i\n", server->udp_socket_number);
    start_socket_socker(server, example_task_func);
    pthread_join(server->main_thread, NULL);
    free_socket_server(server);
}

int main(void) {
    const struct CMUnitTest tests[] = {
       cmocka_unit_test(test_new_socket_server)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}