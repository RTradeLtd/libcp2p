#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "multiaddr/multiaddr.h"
#include "multiaddr/varhexutils.h"

#define assert__(x) for ( ; !(x) ; assert(x) )

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_peer_id(void **state) {
    char *orig_address = "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
    char full_string[255];
    char *result = NULL;
    char *bytes = NULL;
    int retVal = 0, port = 0;
    struct MultiAddress *addr = NULL, *addr2 = NULL;
    sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

    addr = multiaddress_new_from_string(full_string);
    result = multiaddress_get_peer_id(addr);
    assert__(result) {
        printf("result is not NULL\n"); 
    };
    assert__(strcmp(result, orig_address) == 0) {
        printf("Original string was %s but new string is %s\n", result, orig_address);
    }
    free(result);

    // switch to bytes and back again to verify the peer id follows...

    // 1. display the original bytes
    result = Var_To_Hex((char *)addr->bytes, addr->bsize);
    fprintf(stderr, "Original Bytes: %s\n", result);
    free(result);

    // make a new MultiAddress from bytes
    bytes = malloc(addr->bsize);
    memcpy(bytes, addr->bytes, addr->bsize);
    // TODO(bonedaddy): this is bugged and doesnt return the correct multiaddress
    addr2 = multiaddress_new_from_bytes((unsigned char *)bytes, addr->bsize);
    assert(addr2 != NULL);
    free(bytes);

    // 2. Display the resultant bytes
    result = Var_To_Hex((char *)addr2->bytes, addr2->bsize);
    fprintf(stderr, "New      Bytes: %s\n", result);
    free(result);

    assert__(strcmp(full_string, addr2->string) == 0) {
        printf("Original string was %s but new string is %s\n", full_string, addr2->string);
    }

    port = multiaddress_get_ip_port(addr2);
    assert__(port == 4001) {
        printf("Original port was 4001 but new port is %i\n", port);
    }

    result = multiaddress_get_peer_id(addr2);
    assert__(strcmp(result, orig_address) == 0) {
        printf("New peer id %s does not match %s\n", result, orig_address);
    }
 
    free(result);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_get_peer_id(void **state) {
    const char *orig_address = "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
    char full_string[255] = "";
    char *result = NULL;
    int retVal = 0;
    struct MultiAddress *addr = NULL;

    sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

    addr = multiaddress_new_from_string(full_string);
    result = multiaddress_get_peer_id(addr);
    assert(result != NULL);
    int rc = strcmp(orig_address, result);
    assert(rc == 0);
    multiaddress_free(addr);
    free(result);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multiaddr_peer_id),
        cmocka_unit_test(test_multiaddr_get_peer_id),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
