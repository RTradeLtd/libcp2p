#include "utils/colors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *get_ansi_color_scheme(COLORS color) {
    switch (color) {
        case COLORS_RED:
            return ANSI_COLOR_RED;
        case COLORS_SOFT_RED:
            return ANSI_COLOR_SOFT_RED;
        case COLORS_GREEN:
            return ANSI_COLOR_GREEN;
        case COLORS_YELLOW:
            return ANSI_COLOR_YELLOW;
        case COLORS_BLUE:
            return ANSI_COLOR_BLUE;
        case COLORS_MAGENTA:
            return ANSI_COLOR_MAGENTA;
        case COLORS_CYAN:
            return ANSI_COLOR_CYAN;
        case COLORS_RESET:
            return ANSI_COLOR_RESET;
        default:
            return NULL;
    }
}

char *format_colored(COLORS color, char *message) {
    char *pcolor = get_ansi_color_scheme(color);
    if (pcolor == NULL) {
        return NULL;
    }
    char *formatted = malloc(sizeof(message) + sizeof(pcolor));
    if (formatted == NULL) {
        printf("failed to format colored string\n");
        return NULL;
    }
    strcat(formatted, pcolor);
    strcat(formatted, message);
    return formatted;
}

void print_colored(COLORS color, char *message) {
    printf("%s%s%s\n", get_ansi_color_scheme(color), message, ANSI_COLOR_RESET);
}

int write_colored(COLORS color, int file_descriptor, char *message) {
    char *pcolor = get_ansi_color_scheme(color);
    if (pcolor == NULL) {
        return -1;
    }
    char *reset = get_ansi_color_scheme(COLORS_RESET);
    if (reset == NULL) {
        return -1;
    }
    // 2 for \n
    char *write_message =
        calloc(sizeof(char), strlen(pcolor) + strlen(reset) + strlen(message) + 2);
    if (write_message == NULL) {
        printf("failed to calloc write_message\n");
        return -1;
    }
    strcat(write_message, pcolor);
    strcat(write_message, message);
    strcat(write_message, reset);
    strcat(write_message, "\n");
    int response = write(file_descriptor, write_message, strlen(write_message));
    free(write_message);
    if (response == -1) {
        printf("failed to write colored message\n");
        return response;
    }
    return 0;
}