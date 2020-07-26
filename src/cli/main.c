#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <argtable3.h>
#include "utils/colors.h"
#include "cli/command_line.h"
#include "utils/logger.h"


#ifndef COMMAND_VERSION_STRING
#define COMMAND_VERSION_STRING "0.0.1"
#endif

int main(int argc, char *argv[]) {
  setup_args(COMMAND_VERSION_STRING);
  void *argtable[] = {help, version, file, output, command_to_run, end};
  int response = parse_args(argc, argv, argtable);
  switch (response) {
    case 0:
      break;
    case -1:
      printf("parse_args failed\n");
      return response;
    case -2: // this means --help was invoked
      return 0;
  }
  // handle help if no other cli arguments were given (aka binary invoked with ./some-binary)
  if (argc == 1) {
    print_help(argv[0], argtable);
    return 0;
  }
  // construct the command object
  command_object *pcmd = new_command_object(argc, argv);
  if (pcmd == NULL) {
    printf("failed to get command_object");
    return -1;
  }
  // END COMMAND INPUT PREPARATION
  int resp = execute(pcmd, "helo");
  if (resp != 0) {
    // TODO(bonedaddy): figure out if we should log this
    // printf("command run failed\n");
  }
  free_command_object(pcmd);
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return resp;
}