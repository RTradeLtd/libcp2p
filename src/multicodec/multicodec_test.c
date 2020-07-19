#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "multicodec/codecs.h"
#include "multicodec/multicodec.h"
#include <stdbool.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

void test_valide_codecs(void **state) {
    for (int i = 0; i < CODECS_COUNT; i++) {
        bool is_valid = multicodec_is_valid((char *)CODECS_LIST[i]);
        assert(is_valid == true);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_valide_codecs)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}