/*! @file colors.h
  * @brief macros and utilities for printing color to stdout
  * from https://www.quora.com/How-do-I-print-a-colored-output-in-C
  * Pass a `COLORS_x` enum value into `print_colored` and the color will be printed on stdout
  * Note that if you want to disable printing of that color you'll have to send the `COLORS_RESET` enum value through
  * to make non-bold change 0->1 (0;31m red) vs (1;31m bold red)
*/

#pragma once

#include <stdbool.h>

#define ANSI_COLOR_RED "\x1b[1;31m"
#define ANSI_COLOR_SOFT_RED "\x1b[1;38;5;210m"
#define ANSI_COLOR_GREEN "\x1b[1;32m"
#define ANSI_COLOR_YELLOW "\x1b[1;33m"
#define ANSI_COLOR_BLUE "\x1b[1;34m"
#define ANSI_COLOR_MAGENTA "\x1b[1;35m"
#define ANSI_COLOR_CYAN "\x1b[1;36m"
#define ANSI_COLOR_RESET "\x1b[1;0m"

/*! @brief allows short-handed references to ANSI color schemes, and enables easier color selection
  * anytime you want to extend the available colors with an additional enum, add a switch case in get_ansi_color_scheme
*/
typedef enum {
  COLORS_RED, COLORS_SOFT_RED, COLORS_GREEN, COLORS_YELLOW, COLORS_BLUE, COLORS_MAGENTA, COLORS_CYAN, COLORS_RESET
} COLORS;

/*! @brief returns a `char *` with the message formatted with ansi colors
*/
char *format_colored(COLORS color, char *message);

/*! @brief returns an ansi color string to be used with printf
*/
char *get_ansi_color_scheme(COLORS color);

/*! @brief prints message to stdout with the given color
*/
void print_colored(COLORS color, char *message);

/*! @brief writes a message to fh with the given color
  * For "sync writes" and to always flush logs to disk immediately set do_flush to true
  * returns 0 if no error, returns 1 if error
*/
int write_colored(COLORS color, int file_descriptor, char *message);