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
#include "thirdparty/logger/logger.h"
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
#pragma GCC diagnostic ignored "-Wunused-variable"


void start_socker_server_wrapper(void *data) {
    socket_server_t *server = (socket_server_t *)data;
    start_socket_server(server);
}

/*!
  * @brief in this we reuse the thread pool to start the socket server listening process, but you will likely want to do this from your main thread
*/
void test_new_socket_server(void **state) {

    // setup the first server
    thread_logger *thl1 = new_thread_logger(true);
    socket_server_config_t *config1 = new_socket_server_config(2);
    config1->max_connections = 100;
    config1->num_threads = 6;
    config1->fn_tcp = handle_inbound_rpc;
    config1->fn_udp = handle_inbound_rpc;

    multi_addr_t *tcp_addr1 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9090");
    multi_addr_t *udp_addr1 = multi_address_new_from_string("/ip4/127.0.0.1/udp/9091");
    multi_addr_t *endpoint1 = multi_address_new_from_string("/ip4/127.0.0.1/udp/9091");
    config1->addrs[0] = tcp_addr1;
    config1->addrs[1] = udp_addr1;
    config1->num_addrs = 2;

    SOCKET_OPTS opts[2] = {REUSEADDR, NOBLOCK};

    socket_server_t *server1 = new_socket_server(thl1, config1, opts, 2);
    assert(server1 != NULL);
    free_socket_server_config(config1);
    thpool_add_work(server1->thpool, start_socker_server_wrapper, server1);

    // setup the second server
    thread_logger *thl2 = new_thread_logger(true);
    socket_server_config_t *config2 = new_socket_server_config(2);
    config2->max_connections = 100;
    config2->num_threads = 6;
    config2->fn_tcp = handle_inbound_rpc;
    config2->fn_udp = handle_inbound_rpc;

    multi_addr_t *tcp_addr2 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9092");
    multi_addr_t *udp_addr2 = multi_address_new_from_string("/ip4/127.0.0.1/udp/9093");
    multi_addr_t *endpoint2 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9092");
    config2->addrs[0] = tcp_addr2;
    config2->addrs[1] = udp_addr2;
    config2->num_addrs = 2;

    socket_server_t *server2 = new_socket_server(thl2, config2, opts, 2);
    assert(server2 != NULL);
    free_socket_server_config(config2);
    thpool_add_work(server2->thpool, start_socker_server_wrapper, server2);

    sleep(2);


    message_t *msg = calloc(1, sizeof(message_t));
    msg->type = MESSAGE_START_ECDH;
    msg->data = calloc(1, 2);
    assert(msg->data != NULL);
    msg->data[0] = 'o';
    msg->data[1] = 'k';
    msg->len = 2;

    cbor_encoded_data_t *msg_encoded = cbor_encode_message_t(msg);
    assert(msg_encoded != NULL);

    size_t cbor_len = get_encoded_send_buffer_len(msg_encoded);
    unsigned char send_buffer[cbor_len];
    memset(send_buffer, 0, cbor_len);

    int rc = get_encoded_send_buffer(msg_encoded, send_buffer, cbor_len);
    assert(rc == 0);



    rc = socket_server_sendto(server1, endpoint2, send_buffer, cbor_len);
    if (rc != 0) {
        printf("failed to send from server1 to server2: %s\n", strerror(errno));
    }





    // this will trigger shutdown of both servers
    signal_shutdown();
    sleep(5);
    free_socket_server(server1);
    multi_address_free(endpoint1);
    // free(config.addrs);
}


void test_cbor_message_t_encoding(void **state) {
    message_t *msg = calloc(1, sizeof(message_t));
    assert(msg != NULL);
    msg->data = calloc(1, 6);
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