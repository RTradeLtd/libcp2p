# Development Guidelines

* Code submitted in PRs must be formatted using `make format`
* All code must be tested with CMocka
* All code must be documented with doxygen compatible comments
* All code must be tested with valgrind

# writing tests

If starting a new test file, make sure the last part of the file name is `_test.c`, and that your actual tests have the following function signature:
```C
void test_your_test_name(void **state)
```
Use the following template to copy and paste when starting a new test file

```C
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
  write your tests here
*/

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(/* your test here */)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

# mbedtls

* [storing ecdsa context](https://forums.mbed.com/t/write-rsa-public-key-top-pem/4524/2)
* https://tls.mbed.org/discussions/generic/how-to-store-a-ecdsa-context