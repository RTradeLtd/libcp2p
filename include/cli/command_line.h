
/*! @file command_line.h
  * @brief provides a basic CLI building tool
  * attribution note: modified version of commander see `deps/commander` for the license for that package
  * uses argtable3 for command line configuration and allows loading a list of commands to execute
  * using the `--conmmand` or `-c` CLI flag you can specify the command out of the list of commands that have been loaded
  * before calling execute you'll want to load the appropriate `argc` and `argv` values for the callback of the command you want to make
  * @note from https://github.com/bonedaddy/c-template
*/

// pragma once is an alternative to header guards which can be very verbose
#pragma once

#include <argtable3.h>
#include <stdbool.h>

#ifndef MAX_COMMANDS
#define MAX_COMMANDS 32
#endif

#ifndef MAX_COMMAND_ARGS
#define MAX_COMMAND_ARGS 32
#endif

// default command line arguments
struct arg_lit *help, *version;
struct arg_str *command_to_run;
struct arg_file *file, *output;
struct arg_end *end;

// command is the main object type
struct command;

// declares the command handler callback which takes in an instance of command
typedef void (*command_handler_callback)(int argc, char *argv[]);

/*! @struct an individual command to run
  * @brief callback is a function to be executed
*/
typedef struct {
  char *name; 
  command_handler_callback callback;
} command_handler;

/*! @struct the root command object
  * @brief contains the list of possible commands to execute
*/
typedef struct command {
  int command_count;
  int argc;
  char *argv[MAX_COMMAND_ARGS];
  command_handler *commands[MAX_COMMANDS];
} command_object;

/*! @brief returns the value of command_to_run
*/
char *get_run_command();
/*! @brief setups the default argtable arguments
*/
void setup_args(const char *version_string);
/*! @brief formats output
*/
void print_help(char *program_name, void *argtable[]);
/*! @brief parses arguments, and checks for any errors
*/
int parse_args(int argc, char *argv[], void *argtable[]);

/*! @brief checks to see if we have a command named according to run and executes it
*/
int execute(command_object *self, char *command_to_run);

/*! @brief loads command handler and makes it executable
*/
int load_command(command_object *self, command_handler *command);

/*! @brief intializes a new command_object to have commands loaded into
*/
command_object *new_command_object(int argc, char *argv[]);

/*! @brief frees memory allocated for the command_object and sets pointer to null
*/
void free_command_object(command_object *self);