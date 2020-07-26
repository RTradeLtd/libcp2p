#include "utils/logger.h"
#include "cli/command_line.h"
#include "multiaddr/multiaddr.h"
#include "network/socket_server.h"
#include "utils/colors.h"
#include "utils/logger.h"
#include <argtable3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COMMAND_VERSION_STRING
#define COMMAND_VERSION_STRING "0.0.1"
#endif

#pragma GCC diagnostic ignored "-Wunused-parameter"

void start_server_callback(int argc, char *argv[]);

struct arg_str *listen_address_tcp;
struct arg_str *listen_address_udp;
struct arg_str *pem_file_path;

void start_server_callback(int argc, char *argv[]) {
    socket_server_config_t *config = new_socket_server_config(2);
    if (config == NULL) {
        printf("failed to initialize config\n");
        return;
    }
    // note: no input validation
    multi_addr_t *tcp_addr =
        multi_address_new_from_string((char *)*listen_address_tcp->sval);
    if (tcp_addr == NULL) {
        free_socket_server_config(config);
        return;
    }
    printf("tcp addr: %s\n", tcp_addr->string);
    multi_addr_t *udp_addr =
        multi_address_new_from_string((char *)*listen_address_udp->sval);
    if (udp_addr == NULL) {
        free_socket_server_config(config);
        return;
    }
    printf("udp addr: %s\n", udp_addr->string);
    config->max_connections = 100;
    config->num_threads = 6;
    config->addrs[0] = tcp_addr;
    config->addrs[1] = udp_addr;
    config->fn_tcp = handle_inbound_rpc;
    config->fn_udp = handle_inbound_rpc;

    thread_logger *logger = new_thread_logger(true);
    if (logger == NULL) {
        free_socket_server_config(config);
        return;
    }
    socket_server_t *server = new_socket_server(logger, *config);
    if (server == NULL) {
        free_socket_server_config(config);
        clear_thread_logger(logger);
        return;
    }

    // free up config as it is no longer needed
    free_socket_server_config(config);

    // start the actual server
    start_socket_server(server);
    // free up the server resources
    free_socket_server(server);

    clear_thread_logger(logger);

    return;
}

// displays the help command
command_handler *new_socket_server_command();

command_handler *new_socket_server_command() {
    command_handler *handler = malloc(sizeof(command_handler));
    if (handler == NULL) {
        printf("failed to malloc comand_handler\n");
        return NULL;
    }
    handler->callback = start_server_callback;
    handler->name = "start-socket-server";
    return handler;
}

int main(int argc, char *argv[]) {
    // default arg setup
    setup_args(COMMAND_VERSION_STRING);
    // custom arg setup
    listen_address_udp = arg_strn(NULL, "listen-address-udp", "<multiaddr>", 0, 1,
                                  "multiaddress to accept udp connections on");
    listen_address_tcp = arg_strn(NULL, "listen-address-tcp", "<multiaddr>", 0, 1,
                                  "multiaddress to accept tcp connections on");
    pem_file_path = arg_strn(NULL, "pem-file-path", "<file-path>", 0, 1,
                             "path to PEM encoded private key");

    // declare artable
    void *argtable[] = {listen_address_tcp,
                        listen_address_udp,
                        pem_file_path,
                        help,
                        version,
                        file,
                        output,
                        command_to_run,
                        end};

    // prepare arguments
    int response = parse_args(argc, argv, argtable);
    switch (response) {
        case 0:
            break;
        case -1:
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            printf("parse_args failed\n");
            return response;
        case -2: // this means --help was invoked
            return 0;
    }
    // handle help if no other cli arguments were given (aka binary invoked with
    // ./some-binary)
    if (argc == 1) {
        print_help(argv[0], argtable);
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 0;
    }
    // construct the command object
    command_object *pcmd = new_command_object(argc, argv);
    if (pcmd == NULL) {
        printf("failed to get command_object");
        goto EXIT;
    }

    load_command(pcmd, new_socket_server_command());

    // END COMMAND INPUT PREPARATION
    int resp = execute(pcmd, (char *)*command_to_run->sval);
    if (resp != 0) {
        // TODO(bonedaddy): figure out if we should log this
        // printf("command run failed\n");
    }
EXIT:
    free_command_object(pcmd);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return resp;
}