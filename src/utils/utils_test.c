#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <assert.h>
#include <pthread.h>
#include "utils/logger.h"
#include "utils/colors.h"
#include "utils/thread_pool.h"

void *test_thread_log(void *data) {
    thread_logger *thl = (thread_logger *)data;
    thl->log(thl, 0, "this is an info log", LOG_LEVELS_INFO);
    thl->logf(thl, 0, LOG_LEVELS_INFO, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is a warn log", LOG_LEVELS_WARN);
    thl->logf(thl, 0, LOG_LEVELS_WARN, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is an error log", LOG_LEVELS_ERROR);
    thl->logf(thl, 0, LOG_LEVELS_ERROR, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is a debug log", LOG_LEVELS_DEBUG);
    thl->logf(thl, 0, LOG_LEVELS_DEBUG, "%s\t%s", "one", "two");
    // commenting this out seems to get rid of memleaks reported by valgrind
    // pthread_exit(NULL);
    return NULL;
}

void *test_file_log(void *data) {
    file_logger *fhl = (file_logger *)data;
    fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is an info log", LOG_LEVELS_INFO);
    fhl->thl->logf(fhl->thl, fhl->file_descriptor, LOG_LEVELS_INFO, "%s\t%s", "one", "two");

    fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is a warn log", LOG_LEVELS_WARN);
    fhl->thl->logf(fhl->thl, fhl->file_descriptor, LOG_LEVELS_WARN, "%s\t%s", "one", "two");

    fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is an error log", LOG_LEVELS_ERROR);
    fhl->thl->logf(fhl->thl, fhl->file_descriptor, LOG_LEVELS_ERROR, "%s\t%s", "one", "two");
    
    fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is a debug log", LOG_LEVELS_DEBUG);
    fhl->thl->logf(fhl->thl, fhl->file_descriptor, LOG_LEVELS_DEBUG, "%s\t%s", "one", "two");
    // commenting this out seems to get rid of memleaks reported by valgrind
    // pthread_exit(NULL);
    return NULL;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_thread_logger(void **state) {
    bool args[2] = {false, true};
    for (int i = 0; i < 2; i++) {
        thread_logger *thl = new_thread_logger(args[i]);
        assert(thl != NULL);
        thl->log(thl, 0, "this is an info log", LOG_LEVELS_INFO);
        thl->log(thl, 0, "this is a warn log", LOG_LEVELS_WARN);
        thl->log(thl, 0, "this is an error log", LOG_LEVELS_ERROR);
        thl->log(thl, 0, "this is a debug log", LOG_LEVELS_DEBUG);
        pthread_t threads[4];
        pthread_attr_t attrs[4];
        for (int i = 0; i < 4; i++) {
            pthread_attr_init(&attrs[i]);
            pthread_create(&threads[i], &attrs[i], test_thread_log, thl);
        }
        for (int i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL);
            pthread_attr_destroy(&attrs[i]);
        }
        clear_thread_logger(thl);
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_file_logger(void **state) {
    bool args[2] = {false, true};
    for (int i = 0; i < 2; i++) {
        file_logger *fhl = new_file_logger("file_logger_test.log", args[i]);
        assert(fhl != NULL);
        fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is an info log", LOG_LEVELS_INFO);
        fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is a warn log", LOG_LEVELS_WARN);
        fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is an error log", LOG_LEVELS_ERROR);
        fhl->thl->log(fhl->thl, fhl->file_descriptor, "this is a debug log", LOG_LEVELS_DEBUG);
        pthread_t threads[4];
        pthread_attr_t attrs[4];
        for (int i = 0; i < 4; i++) {
            pthread_attr_init(&attrs[i]);
            pthread_create(&threads[i], &attrs[i], test_file_log, fhl);
        }
        for (int i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL);
            pthread_attr_destroy(&attrs[i]);
        }
        clear_file_logger(fhl);
    }
}

typedef struct args {
    COLORS test_color;
    char *want_ansi;
} args;
typedef struct test {
    args args;
    char *name;
} test;

void test_print_color(void **state);
void test_get_ansi_color_scheme(void **state);
void validate_test_args(test testdata);
// TODO(bonedaddy): test format_colored

void validate_test_args(test testdata) {
    //   printf("%s\n", format_colored(testdata.args.test_color, testdata.name));
    char *scheme = get_ansi_color_scheme(testdata.args.test_color);
    if (strcmp(scheme, testdata.args.want_ansi) == 0) {

        printf("%stest %s passed\n", scheme, testdata.name);
        return;
    }
    printf("test %s failed\n", testdata.name);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_get_ansi_color_scheme(void **state) {

    test tests[8] = {
        {
            .name = "red", 
            .args = {
                .test_color = COLORS_RED,
                .want_ansi = ANSI_COLOR_RED,
            },
        },
        {
            .name = "soft_red",
            .args = {
                .test_color = COLORS_SOFT_RED,
                .want_ansi = ANSI_COLOR_SOFT_RED,
            },
        },
        {
            .name = "green", 
            .args = {
                .test_color = COLORS_GREEN,
                .want_ansi = ANSI_COLOR_GREEN,
            },
        },
        {
            .name = "yellow", 
            .args = {
                .test_color = COLORS_YELLOW,
                .want_ansi = ANSI_COLOR_YELLOW,
            },
        },
        {
            .name = "blue", 
            .args = {
                .test_color = COLORS_BLUE,
                .want_ansi = ANSI_COLOR_BLUE,
            },
        },
        {
            .name = "magenta", 
            .args = {
                .test_color = COLORS_MAGENTA,
                .want_ansi = ANSI_COLOR_MAGENTA,
            },
        },
        {
            .name = "cyan", 
            .args = {
                .test_color = COLORS_CYAN,
                .want_ansi = ANSI_COLOR_CYAN,
            },
        },
        {
            .name = "reset", 
            .args = {
                .test_color = COLORS_RESET,
                .want_ansi = ANSI_COLOR_RESET,
            },
        },
    };
    for (int i = 0; i < 7; i++) {
        validate_test_args(tests[i]);
    }
}

void thread_pool_task_ex(void *arg) {
    printf("Thread #%u working on %lu\n", (int)pthread_self(), (uintptr_t)arg);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_thread_pool(void **state) {
    threadpool thpool = thpool_init(4);
    for (int i = 0; i < 40; i++) {
        int j = i;
        uintptr_t jj = j;
        thpool_add_work(thpool, thread_pool_task_ex, (void *)jj);
    }
    thpool_wait(thpool);
    thpool_destroy(thpool);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_thread_logger),
        cmocka_unit_test(test_file_logger),
        cmocka_unit_test(test_get_ansi_color_scheme),
        cmocka_unit_test(test_thread_pool)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}