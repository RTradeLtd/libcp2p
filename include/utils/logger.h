/*! @file logger.h
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

#pragma once

#include "colors.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

/*! @struct base struct used by the thread_logger
 */
struct thread_logger;

/*! @typedef specifies log_levels, typically used when determining function
 * invocation by log_fn
 */
typedef enum {
    /*! indicates the message we are logging is of type info (color green) */
    LOG_LEVELS_INFO,
    /*! indicates the message we are logging is of type warn (color yellow) */
    LOG_LEVELS_WARN,
    /*! indicates the message we are logging is of type error (color red) */
    LOG_LEVELS_ERROR,
    /*! indicates the message we are logging is of type debug (color soft red) */
    LOG_LEVELS_DEBUG
} LOG_LEVELS;

/*! @typedef signature of pthread_mutex_unlock and pthread_mutex_lock used by the
 * thread_logger
 * @param mx pointer to a pthread_mutex_t type
 */
typedef int (*mutex_fn)(pthread_mutex_t *mx);

/*! @typedef signature used by the thread_logger for log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
typedef void (*log_fn)(struct thread_logger *thl, int file_descriptor, char *message,
                       LOG_LEVELS level);
/*! @typedef signatured used by the thread_logger for printf style log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `%sFOO%sBAR`
 * @param ... values to supply to message
 */
typedef void (*log_fnf)(struct thread_logger *thl, int file_descriptor,
                        LOG_LEVELS level, char *message, ...);

/*! @typedef a thread safe logger
 * @brief guards all log calls with a mutex lock/unlock
 * recommended usage is to call thread_logger:log(instance_of_thread_logger,
 * char*_of_your_log_message, log_level) alternatively you can call the `*_log`
 * functions directly
 */
typedef struct thread_logger {
    // todo(bonedaddy): make atomic
    bool debug;
    pthread_mutex_t mutex;
    mutex_fn lock;
    mutex_fn unlock;
    log_fn log;
    log_fnf logf;
} thread_logger;

/*! @typedef a wrapper around thread_logger that enables file logging
 * @brief like thread_logger but also writes to a file
 * @todo
 *  - enable log rotation
 */
typedef struct file_logger {
    thread_logger *thl;
    int file_descriptor;
} file_logger;

/*! @brief returns a new thread safe logger
 * if with_debug is false, then all debug_log calls will be ignored
 * @param with_debug whether to enable debug logging, if false debug log calls will
 * be ignored
 */
thread_logger *new_thread_logger(bool with_debug);

/*! @brief returns a new file_logger
 * Calls new_thread_logger internally
 * @param output_file the file we will dump logs to. created if not exists and is
 * appended to
 */
file_logger *new_file_logger(char *output_file, bool with_debug);

/*! @brief free resources for the threaded logger
 * @param thl the thread_logger instance to free memory for
 */
void clear_thread_logger(thread_logger *thl);

/*! @brief free resources for the file ogger
 * @param fhl the file_logger instance to free memory for. also frees memory for the
 * embedded thread_logger and closes the open file
 */
void clear_file_logger(file_logger *fhl);

/*! @brief main function you should call, which will delegate to the appopriate *_log
 * function
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
void log_func(thread_logger *thl, int file_descriptor, char *message,
              LOG_LEVELS level);

/*! @brief like log_func but for formatted logs
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `<percent-sign>sFOO<percent-sign>sBAR`
 * @param ... values to supply to message
 */
void logf_func(thread_logger *thl, int file_descriptor, LOG_LEVELS level,
               char *message, ...);

/*! @brief logs a debug styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void debug_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs a warned styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void warn_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs an error styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void error_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs an info styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void info_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief used to write a log message to file
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
int write_file_log(int file_descriptor, char *message);