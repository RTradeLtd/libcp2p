
/*! @file command_line.c
 * @brief provides a basic CLI building tool
 * attribution note: modified version of commander see `deps/commander` for the
 * license for that package uses argtable3 for command line configuration and allows
 * loading a list of commands to execute using the `--conmmand` or `-c` CLI flag you
 * can specify the command out of the list of commands that have been loaded before
 * calling execute you'll want to load the appropriate `argc` and `argv` values for
 * the callback of the command you want to make
 * @note from https://github.com/bonedaddy/c-template
 */

#include "cli/command_line.h"
#include "thirdparty/argtable3/argtable3.h"
#include "utils/colors.h"
#include "utils/logger.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*! @brief intializes a new command_object to have commands loaded into
 */
command_object *new_command_object(int argc, char *argv[]) {
    // if too many arguments have been provided return a null pointer
    if (argc > MAX_COMMAND_ARGS) {
        print_colored(COLORS_RED, "too many command line arguments provided\n");
        return NULL;
    }
    // allocate memory equal to the size of command_object  combined with the size of
    // all provided arguments and typecast it to command_object * command_object
    // *pcobj = (command_object *)malloc(sizeof(command_object) + sizeof(*argv));
    // this prevents valgrind from reporting an error
    command_object *pcobj = calloc(sizeof(command_object), sizeof(*argv));
    if (pcobj == NULL) {
        printf("failed to malloc command_object\n");
        return NULL;
    }
    // set arg count
    pcobj->argc = argc;
    // set command_count
    pcobj->command_count = 0;
    // parse cli arguments provided by user  and assign to pcobj
    for (int i = 0; i < argc; i++) {
        pcobj->argv[i] = malloc(strlen(argv[i]) + 1);
        if (pcobj->argv[i] == NULL) {
            printf("failed to malloc argv space\n");
            return NULL;
        }
        strcpy(pcobj->argv[i], argv[i]);
    }
    return pcobj;
}

/*! @brief loads command handler and makes it executable
 */
int load_command(command_object *self, command_handler *command) {
    if (self == NULL) {
        return -1;
    }
    if (self->command_count >= MAX_COMMANDS) {
        print_colored(COLORS_RED, "maximum number of commands\n");
        return -1;
    }
    int n = self->command_count++;
    self->commands[n] = command;
    return 0;
}

/*! @brief checks to see if we have a command named according to run and executes it
 */
int execute(command_object *self, char *run) {
    for (int i = 0; i < self->command_count; i++) {
        if (strcmp(self->commands[i]->name, run) == 0) {
            self->commands[i]->callback(self->argc, self->argv);
            print_colored(COLORS_GREEN, "[info] command executed successfully\n");
            return 0;
        }
    }
    print_colored(COLORS_RED, "[error] failed to execute command\n");
    return -1;
}

/*! @brief frees memory allocated for the command_object and sets pointer to null
 * for some reason this is causing an address boundary error
 */
void free_command_object(command_object *self) {
    for (int i = 0; i < self->command_count; i++) {
        free(self->commands[i]);
    }
    for (int i = 0; i < self->argc; i++) {
        free(self->argv[i]);
    }
    free(self);
}

// validates argtable and parses command line arguments
int parse_args(int argc, char *argv[], void *argtable[]) {
    if (arg_nullcheck(argtable) != 0) {
        return -1;
    }
    int nerrors = arg_parse(argc, argv, argtable);
    // handle help before errors
    if (help->count > 0) {
        print_help(argv[0], argtable);
        return -2;
    }
    if (nerrors > 0) {
        arg_print_errors(stdout, end, "main");
        return -1;
    }
    return 0;
}

void print_help(char *program_name, void *argtable[]) {
    printf("Usage: %s", program_name);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, " %-25s %s\n");
}

// sets up default arguments, you will have to do this for user defined args
void setup_args(const char *version_string) {
    help = arg_litn(NULL, "help", 0, 1, "displays help menu");
    version = arg_litn(NULL, "version", 0, 1, version_string);
    file = arg_filen(NULL, "file", "<file>", 0, 10,
                     "file(s) to load data from max of 10");
    output = arg_filen(NULL, "output", "<file>", 0, 1, "file to output data too");
    command_to_run = arg_strn("c", "command", "<command>", 0, 1, "command to run");
    end = arg_end(20);
}

char *get_run_command() {
    char *run_command = malloc(strlen(*command_to_run->sval));
    if (run_command == NULL) {
        printf("failed to malloc run_command\n");
        return NULL;
    }
    strcpy(run_command, (char *)*command_to_run->sval);
    return run_command;
}