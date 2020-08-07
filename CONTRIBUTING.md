# Contribution Guidelines

The following is a short summary on development guidelines:

* Code submitted in PRs must be formatted using `make format`
* All code must be tested with CMocka
* All code must be documented with doxygen compatible comments
* All code must be tested with valgrind

# style guides

when it comes to programming everyone has their own style of writing code, and in languages like C which can accept almost any style of code, it can lead to large code bases looking like they were written by different people. Ultimately this makes it harder to analyze the codebase as a whole.

As such we enforce very strict styling rules within the codebase, and all code must be formatted using `clang-format`. In addition there are a few other rules that apply to style of code that aren't caught by `clang-format`. Adhering to these style guidelines is **not** optional, and all code that is submitted via PRs **must** follow these guidelines. Refusal to follow the guidelines will result in PRs not being accepted. This may seem a bit strict to some people, but it is necessary in ensuring that the codebase is maintainable, and easy to read.

## comments

*Every single function, typedef, struct, or global variable* **must** be commented using valid doxygen comments. For functions the comments must clearly explain the purpose of the function, the arguments of the function, and the return values, as well as any other notes. Memory allocation and management of memory that is required for function calls must also be documented via `@warning` tags. For typedefs, structs, and global variables the comments must cleary explain the purpose of them.

## braces

You must always have the opening brace on the same line of the expression. 

For example the following is acceptable

```C
if (abc == true) {
    /* do stuff */
}
```

Whereas the following is not acceptable:

```C
if (abc == true)
{
  /* do stuff */
}
```

## memory allocations

When it comes to managing memory, there is no strict rule on whether or not you allocate the memory for an object in a function call, or do it outside of a function call providing the memory as an argument. Either of these two patterns is acceptable, however you must document the function via a `@warning` tag that indicates how the memory is supposed to be managed.


# writing tests

> NOTE: all tests should be placed into the `tests` directory in the project root

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
