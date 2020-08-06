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
#include "testutils/testutils.h"
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

void start_socker_server_wrapper(void *data) {
    socket_server_t *server = (socket_server_t *)data;
    start_socket_server(server);
}

/*!
  * @brief in this we reuse the thread pool to start the socket server listening process, but you will likely want to do this from your main thread
  * @warning this test is currently leaking about 24 bytes likely because we are using two thread pools which rely on some sort of global stuff
*/
void test_new_socket_server(void **state) {
    ecdsa_private_key_t *server1_pk = new_ecdsa_private_key();
    ecdsa_private_key_t *server2_pk = new_ecdsa_private_key();

    int rc = libp2p_crypto_ecdsa_private_key_save(server1_pk, "server1.pem");
    assert(rc == 0);
    rc = libp2p_crypto_ecdsa_private_key_save(server2_pk, "server2.pem");
    assert(rc == 0);

    // start server 1
    thread_logger *thl1 = new_thread_logger(true);
    socket_server_config_t *config1 = new_socket_server_config(1);
    config1->max_connections = 100;
    config1->num_threads = 6;
    config1->recv_timeout_sec = 3;
    config1->max_peers = 90;
    config1->fn_tcp = handle_inbound_rpc;
    config1->private_key_path = "server1.pem";

    multi_addr_t *tcp_addr1 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9090");
    multi_addr_t *endpoint1 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9090");
    config1->addrs[0] = tcp_addr1;
    config1->num_addrs = 1;

    SOCKET_OPTS opts[2] = {REUSEADDR, NOBLOCK};

    //   .num_threads = 6, .fn_tcp = example_task_func_tcp, .fn_udp = example_task_func_udp };
    socket_server_t *server1 = new_socket_server(thl1, config1, opts, 2);
    assert(server1 != NULL);
    assert(server1->pstore->max_peers == 90);
    free_socket_server_config(config1);

    thpool_add_work(server1->thpool, start_socker_server_wrapper, server1);

    // start server2
    thread_logger *thl2 = new_thread_logger(true);
    socket_server_config_t *config2 = new_socket_server_config(1);
    config2->max_connections = 100;
    config2->num_threads = 6;
    config2->recv_timeout_sec = 3;
    config2->max_peers = 0; // use to trigger default max peers handling
    config2->fn_tcp = handle_inbound_rpc;
    config2->private_key_path = "server2.pem";
    
    multi_addr_t *tcp_addr2 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9091");
    multi_addr_t *endpoint2 = multi_address_new_from_string("/ip4/127.0.0.1/tcp/9091");
    config2->addrs[0] = tcp_addr2;
    config2->num_addrs = 1;

    //   .num_threads = 6, .fn_tcp = example_task_func_tcp, .fn_udp = example_task_func_udp };
    socket_server_t *server2 = new_socket_server(thl2, config2, opts, 2);
    assert(server2 != NULL);
    
    // verify max peers
    assert(server2->pstore->max_peers == 100);
    
    free_socket_server_config(config2);

    thpool_add_work(server2->thpool, start_socker_server_wrapper, server2);

    // give time for the last server to start
    sleep(1);

    message_hello_t *send_msg_hello = new_server_message_hello_t(server2);
    assert(send_msg_hello != NULL);

    message_t *send_msg = message_hello_t_to_message_t(send_msg_hello, true);
    assert(send_msg != NULL);

    rc = socket_server_send(server2, endpoint1, send_msg);
    assert(rc == 0);


    sleep(10);
    signal_shutdown();

    multi_address_free(endpoint1);
    multi_address_free(endpoint2);
    free_message_t(send_msg);
    free_message_hello_t(send_msg_hello);
    libp2p_crypto_ecdsa_free(server1_pk);
    libp2p_crypto_ecdsa_free(server2_pk);

    sleep(1);

    free_socket_server(server1);
    free_socket_server(server2);
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


void test_cbor_message_hello_t_encoding(void **state) {
    message_hello_t *msg_hello = calloc(1, sizeof(message_hello_t));
    assert(msg_hello != NULL);

    msg_hello->peer_id = calloc(1, 5);
    assert(msg_hello->peer_id != NULL);
    
    msg_hello->public_key = calloc(1, 5);
    assert(msg_hello->public_key != NULL);

    msg_hello->peer_id[0] = 'h';
    msg_hello->peer_id[1] = 'e';
    msg_hello->peer_id[2] = 'l';
    msg_hello->peer_id[3] = 'l';
    msg_hello->peer_id[4] = 'o';
    msg_hello->peer_id_len = 5;
    msg_hello->public_key[0] = 'w';
    msg_hello->public_key[1] = 'o';
    msg_hello->public_key[2] = 'r';
    msg_hello->public_key[3] = 'l';
    msg_hello->public_key[4] = 'd';
    msg_hello->public_key_len = 5;

    cbor_encoded_data_t *encoded_msg_hello = cbor_encode_hello_t(msg_hello);
    assert(encoded_msg_hello != NULL);

    message_hello_t *decoded_msg_hello = cbor_decode_hello_t(encoded_msg_hello);
    assert(decoded_msg_hello != NULL);

    // compare member values, but do it twice
    // using the length from each of the instances
    // to ensure consistency

    assert(
        memcmp(
            decoded_msg_hello->peer_id,
            msg_hello->peer_id,
            decoded_msg_hello->peer_id_len
        ) == 0
    );
    assert(
        memcmp(
            decoded_msg_hello->peer_id,
            msg_hello->peer_id,
            msg_hello->peer_id_len
        ) == 0
    );

    assert(
        memcmp(
            decoded_msg_hello->public_key,
            msg_hello->public_key,
            decoded_msg_hello->public_key_len
        ) == 0
    );
    assert(
        memcmp(
            decoded_msg_hello->public_key,
            msg_hello->public_key,
            msg_hello->public_key_len
        ) == 0
    );

    free_cbor_encoded_data(encoded_msg_hello);
    free_message_hello_t(msg_hello);
    free_message_hello_t(decoded_msg_hello);
    
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cbor_message_t_encoding),
        cmocka_unit_test(test_cbor_message_hello_t_encoding),
        cmocka_unit_test(test_new_socket_server)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}