#include <stdio.h>
#include "crypto/util.h"
#include "mbedtls/error.h"

/*! @brief prints the corresponding error to stdout
*/
void print_mbedtls_error(int errcode) {
    char buffer[1024];
    mbedtls_strerror(errcode, buffer, 1024);
    printf("mbedtls failure detected:\t%s\n", buffer);
}