/*! @file logger.c
 * @brief a thread safe logger with optional printf style logging
 * @details allows writing color coded logs to stdout, with optional file output as
 * well. timestamps all logs, and provides optional printf style logging
 * @note logf_func has a bug where some format is respected and others are not,
 * consider the following from a `%s%s` format:
 *   - [error - Jul 06 10:01:07 PM] one<insert-tab-here>two
 *   - [warn - Jul 06 10:01:07 PM] one	two
 * @note warn, and info appear to not respect format, while debug and error do
 * @todo
 *  - buffer logs and use a dedicated thread for writing (avoid blocking locks)
 *  - handling system signals (exit, kill, etc...)
 */

#include "thirdparty/logger/logger.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*! @brief private function that returns a timestamp of format `Jul 06 10:12:20 PM`
 */
char *get_time_string();

thread_logger *new_thread_logger(bool with_debug) {
    thread_logger *thl = malloc(sizeof(thread_logger));
    if (thl == NULL) {
        printf("failed to malloc thread_logger\n");
        return NULL;
    }
    thl->lock = pthread_mutex_lock;
    thl->unlock = pthread_mutex_unlock;
    thl->log = log_func;
    thl->logf = logf_func;
    thl->debug = with_debug;
    pthread_mutex_init(&thl->mutex, NULL);
    return thl;
}

file_logger *new_file_logger(char *output_file, bool with_debug) {
    thread_logger *thl = new_thread_logger(with_debug);
    if (thl == NULL) {
        // dont printf log here since new_thread_logger handles that
        return NULL;
    }
    file_logger *fhl = malloc(sizeof(file_logger));
    if (fhl == NULL) {
        // free thl as it is not null
        free(thl);
        printf("failed to malloc file_logger\n");
        return NULL;
    }
    // append to file, create if not exist, sync write files
    // TODO(bonedaddy): try to use O_DSYNC for data integrity sync
    int file_descriptor =
        open(output_file, O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0640);
    // TODO(bonedaddy): should this just be `< 0` ? `open` shouldn't return 0 but im
    // unsure about removing the check for it
    if (file_descriptor <= 0) {
        // free thl as it is not null
        free(thl);
        // free fhl as it is not null
        free(fhl);
        printf("failed to run posix open function\n");
        return NULL;
    }
    fhl->file_descriptor = file_descriptor;
    fhl->thl = thl;
    return fhl;
}

int write_file_log(int file_descriptor, char *message) {
    // 2 for \n
    char *msg = calloc(1, strlen(message) + 2);
    if (msg == NULL) {
        printf("failed to calloc msg\n");
        return -1;
    }
    strcat(msg, message);
    strcat(msg, "\n");
    int response = write(file_descriptor, msg, strlen(msg));
    if (response == -1) {
        printf("failed to write file log message");
    } else {
        // this branch will be triggered if write doesnt fail
        // so overwrite the response to 0 as we want to return 0 to indicate
        // no error was received, and returning response directly would return the
        // number of bytes written
        response = 0;
    }
    free(msg);
    return response;
}

void logf_func(thread_logger *thl, int file_descriptor, LOG_LEVELS level, char *file,
               int line, char *message, ...) {
    va_list args;
    va_start(args, message);
    char msg[sizeof(args) + (strlen(message) * 2)];
    memset(msg, 0, sizeof(msg));

    int response = vsnprintf(msg, sizeof(msg), message, args);
    if (response < 0) {
        free(msg);
        printf("failed to vsprintf\n");
        return;
    }
    log_func(thl, file_descriptor, msg, level, file, line);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void log_func(thread_logger *thl, int file_descriptor, char *message,
              LOG_LEVELS level, char *file, int line) {
    char *time_str = get_time_string();
    if (time_str == NULL) {
        // dont printf log as get_time_str does that
        return;
    }
    char location_info[strlen(file) + sizeof(line) + 4];
    memset(location_info, 0, sizeof(location_info));

    sprintf(location_info, " %s:%i", file, line);

    char
        date_msg[strlen(time_str) + strlen(message) + 2 + sizeof(location_info) + 6];
    memset(date_msg, 0, sizeof(date_msg));

    strcat(date_msg, time_str);
    strcat(date_msg, " -");
    strcat(date_msg, location_info);
    strcat(date_msg, "] ");
    strcat(date_msg, message);

    switch (level) {
        case LOG_LEVELS_INFO:
            info_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_WARN:
            warn_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_ERROR:
            error_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_DEBUG:
            debug_log(thl, file_descriptor, date_msg);
            break;
    }

    free(time_str);
}

void info_log(thread_logger *thl, int file_descriptor, char *message) {
    thl->lock(&thl->mutex);
    // 2 = 1 for null terminator, 1 for space after ]
    char *msg = calloc(1, strlen(message) + strlen("[info - ") + (size_t)2);
    if (msg == NULL) {
        printf("failed to calloc info_log msg");
        return;
    }
    strcat(msg, "[info - ");
    strcat(msg, message);
    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }
    print_colored(COLORS_GREEN, msg);
    thl->unlock(&thl->mutex);
    free(msg);
}

void warn_log(thread_logger *thl, int file_descriptor, char *message) {
    thl->lock(&thl->mutex);
    // 2 = 1 for null terminator, 1 for space after ]
    char *msg = calloc(1, strlen(message) + strlen("[warn - ") + (size_t)2);
    if (msg == NULL) {
        printf("failed to calloc warn_log msg");
        return;
    }
    strcat(msg, "[warn - ");
    strcat(msg, message);
    if (file_descriptor != 0) {
        // TODO(bonedaddy): decide if we want to copy
        // char *cpy = malloc(strlen(msg)+1);
        // strcpy(cpy, msg);
        write_file_log(file_descriptor, msg);
        // free(cpy);
    }
    print_colored(COLORS_YELLOW, msg);
    thl->unlock(&thl->mutex);
    free(msg);
}

void error_log(thread_logger *thl, int file_descriptor, char *message) {
    thl->lock(&thl->mutex);
    // 2 = 1 for null terminator, 1 for space after ]
    char *msg = calloc(1, strlen(message) + strlen("[error - ") + (size_t)2);
    if (msg == NULL) {
        printf("failed to calloc error_log msg");
        return;
    }
    strcat(msg, "[error - ");
    strcat(msg, message);
    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }
    print_colored(COLORS_RED, msg);
    thl->unlock(&thl->mutex);
    free(msg);
}

void debug_log(thread_logger *thl, int file_descriptor, char *message) {
    // unless debug enabled dont show
    if (thl->debug == false) {
        return;
    }

    thl->lock(&thl->mutex);
    // 2 = 1 for null terminator, 1 for space after ]
    char *msg = calloc(1, strlen(message) + strlen("[debug - ") + (size_t)2);
    if (msg == NULL) {
        printf("failed to calloc debug_log msg");
        return;
    }
    strcat(msg, "[debug - ");
    strcat(msg, message);
    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }
    print_colored(COLORS_SOFT_RED, msg);
    thl->unlock(&thl->mutex);
    free(msg);
}

void clear_thread_logger(thread_logger *thl) {
    free(thl);
}

void clear_file_logger(file_logger *fhl) {
    close(fhl->file_descriptor);
    clear_thread_logger(fhl->thl);
    free(fhl);
}

char *get_time_string() {
    char date[75];
    strftime(date, sizeof date, "%b %d %r", localtime(&(time_t){time(NULL)}));
    char *msg = calloc(1, sizeof(date) + 1);
    if (msg == NULL) {
        printf("failed to calloc get_time_string\n");
        return NULL;
    }
    strcat(msg, "");
    strcat(msg, date);
    return msg;
}