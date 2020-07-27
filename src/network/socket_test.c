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


#include "encoding/cbor.h"
#include "utils/logger.h"
#include "network/messages.h"
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
    if (bytes_received == -1 || bytes_received == 0) {
        goto EXIT;
    }
    hdata->srv->thl->logf(
        hdata->srv->thl,
        0,
        LOG_LEVELS_INFO,
        "received message from client %s",
        buffer
    );
EXIT:
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

    multi_addr_t *tcp_addr = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9090");
    multi_addr_t *udp_addr = multi_address_new_from_string("/ip4/127.0.0.1/udp/9091");
    multi_addr_t *endpoint = multi_address_new_from_string("/ip4/127.0.0.1/udp/9091");
    config->addrs[0] = tcp_addr;
    config->addrs[1] = udp_addr;
    config->num_addrs = 2;

    SOCKET_OPTS opts[2] = {REUSEADDR, NOBLOCK};

    //   .num_threads = 6, .fn_tcp = example_task_func_tcp, .fn_udp = example_task_func_udp };
    socket_server_t *server = new_socket_server(thl, *config, opts, 2);
    assert(server != NULL);
    free_socket_server_config(config);
    thpool_add_work(server->thpool, start_socker_server_wrapper, server);

    socket_client_t *client = new_socket_client(thl, endpoint);
    assert(client != NULL);

    socket_client_sendto(client, client->peer_address, (unsigned char *)"hello world\n", strlen("hello world \n"));
    sleep(2);
    freeaddrinfo(client->peer_address);
    close(client->socket_number);
    free(client);
    signal_shutdown();
    sleep(5);
    free_socket_server(server);
    multi_address_free(endpoint);
    // free(config.addrs);
}


void test_cbor_message_t_encoding(void **state) {
    message_t *msg = calloc(sizeof(message_t), sizeof(message_t));
    assert(msg != NULL);
    msg->data = calloc(sizeof(unsigned char), 6);
    assert(msg->data != NULL);

    msg->data[0] = 'h';
    msg->data[1] = 'e';
    msg->data[2] = 'l';
    msg->data[3] = 'l';
    msg->data[4] = 'o';
    msg->data[5] = '\0';
    msg->len = 6;
    msg->type = MESSAGE_WANT_PEER_ID;

    cbor_encoded_data_t *cbdata = cbor_encode_message_t(msg);
    assert(cbdata != NULL);
    assert(cbdata->len == 10);

    message_t *ret_msg = cbor_decode_message_t(cbdata);
    assert(ret_msg != NULL);
    assert(ret_msg->len == 6);
    assert(ret_msg->type == MESSAGE_WANT_PEER_ID);
    assert(
        strcmp(
            (char *)ret_msg->data,
            (char *)msg->data
        ) == 0
    );
    assert(
        memcmp(
            ret_msg->data,
            msg->data,
            msg->len
        ) == 0
    );
    assert(
        memcmp(
            ret_msg->data,
            msg->data,
            ret_msg->len
        ) == 0
    );
    free_cbor_encoded_data(cbdata);
    free_message_t(msg);
    free_message_t(ret_msg);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cbor_message_t_encoding),
        cmocka_unit_test(test_new_socket_server)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}