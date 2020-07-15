#include "multicodec/multicodec.h"

#include <stdio.h>

int testEncode() {
    char inData[10];
    char outData[15];
    multicodec_encode("ascii", inData, 10, outData, 15);
    return 1;
}

int testit(const char *name, int (*func)(void)) {
    printf("Running function %s... ", name);
    int retVal = func();
    if (retVal == 0)
        printf("Success\n");
    else
        printf("Failed\n");
    return 0;
}

int main(int argc, char **argv) {
    testit("testEncode", testEncode);
    return 0;
}
