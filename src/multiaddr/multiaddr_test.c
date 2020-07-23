#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "multiaddr/multiaddr.h"
#include "utils/varhexutils.h"

#define assert__(x) for ( ; !(x) ; assert(x) )

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_int_to_hex(void **state) {
    int val = 2555351;
    char *result = Int_To_Hex(val);
    assert(result != NULL);
    int retVal = Hex_To_Int(result); 
    assert(retVal == val);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_hex_to_var(void **state) {
    size_t d;
    unsigned char *result = Hex_To_Var("04", &d);
    assert(d == 1);
    assert(result != NULL);
    assert(result[0] == 4);
    free(result);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_new_from_string(void **state) {
    struct multi_address *a =
        multi_address_new_from_string("/ip4/127.0.0.1/tcp/8080/");
    assert(a != NULL);
    /* printf("Number of Bytes: %lu, Bytes: ", a->bsize);
    for (int i = 0; i < (int)a->bsize; i++) {
        printf("%02x ", a->bytes[i]);
    }
    printf(" End of bytes\n");
    */
    multi_address_free(a);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_new_like_libp2p(void **state) {
    int retVal = 0;
    char *ip = "10.211.55.2";
    int port = 4001;
    char str[strlen(ip) + 50];
    sprintf(str, "/ip4/%s/tcp/%d/", ip, port);
    struct multi_address *ma_string = multi_address_new_from_string(str);
    // convert to binary
    struct multi_address *ma_binary =
        multi_address_new_from_bytes(ma_string->bytes, ma_string->bsize);
    assert(
        strcmp(ma_string->string, ma_binary->string) == 0
    );
    multi_address_free(ma_string);
    multi_address_free(ma_binary);
}


#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_utils(void **state) {
    int retVal = 0;
    struct multi_address *addr =
        multi_address_new_from_string("/ip4/127.0.0.1/tcp/4001/");
    assert(multi_address_is_ip(addr));
    char ip[1024];
    multi_address_get_ip_address(addr, ip);
    assert(ip != NULL);
    assert(
        strcmp(ip, "127.0.0.1") == 0
    );
    int port = multi_address_get_ip_port(addr);
    assert(port == 4001);
    
    multi_address_free(addr);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_get_peer_id(void **state) {
    const char *orig_address = "CIQKAFXKNLNOTSYX4XXER2AXMVZHINJF7AGJPYNCV2D4F7GPEJEW3TA=";
    char full_string[255] = "";
    char *result = NULL;
    int retVal = 0;
    struct multi_address *addr = NULL;

    sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

    addr = multi_address_new_from_string(full_string);
    assert(addr != NULL);
    result = multi_address_get_peer_id(addr);
    assert(result != NULL);
    int rc = strcmp(orig_address, result);
    assert(rc == 0);
    multi_address_free(addr);
    free(result);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_peer_id(void **state) {
    char *orig_address = "CIQKAFXKNLNOTSYX4XXER2AXMVZHINJF7AGJPYNCV2D4F7GPEJEW3TA=";
    char full_string[255];
    char *result = NULL;
    char *bytes = NULL;
    int retVal = 0, port = 0;
    struct multi_address *addr = NULL, *addr2 = NULL;
    sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

    addr = multi_address_new_from_string(full_string);
    result = multi_address_get_peer_id(addr);
    assert__(result) {
        printf("result is not NULL\n"); 
    };
    assert__(strcmp(result, orig_address) == 0) {
        printf("Original string was %s but new string is %s\n", result, orig_address);
    }
    free(result);

    // switch to bytes and back again to verify the peer id follows...

    // 1. display the original bytes
    result = Var_To_Hex(addr->bytes, addr->bsize);
    // fprintf(stderr, "Original Bytes: %s\n", result);
    free(result);

    // make a new multi_address from bytes
    bytes = malloc(addr->bsize);
    memcpy(bytes, addr->bytes, addr->bsize);
    // TODO(bonedaddy): this is bugged and doesnt return the correct multi_address
    // interestingly enough the new bytes matches the old bytes???
    addr2 = multi_address_new_from_bytes(bytes, addr->bsize);
    assert(addr2 != NULL);
    free(bytes);
    
    // 2. Display the resultant bytes
    result = Var_To_Hex(addr2->bytes, addr2->bsize);
    fprintf(stderr, "New      Bytes: %s\n", result);
    free(result);

    assert__(strcmp(full_string, addr2->string) == 0) {
        printf("Original string was %s but new string is %s\n", full_string, addr2->string);
    }

    port = multi_address_get_ip_port(addr2);
    assert__(port == 4001) {
        printf("Original port was 4001 but new port is %i\n", port);
    }

    result = multi_address_get_peer_id(addr2);
    assert__(strcmp(result, orig_address) == 0) {
        printf("New peer id %s does not match %s\n", result, orig_address);
    }
    free(result);
    multi_address_free(addr2);
    multi_address_free(addr);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_bytes(void **state) {
    int retVal = 0;
    char *orig_address = "/ip4/127.0.0.1/tcp/4001/";
    struct multi_address *orig = NULL, *result = NULL;

    orig = multi_address_new_from_string(orig_address);

    result = multi_address_new_from_bytes(orig->bytes, orig->bsize);

    assert(result != NULL);
    assert(
        strcmp(orig_address, result->string) == 0
    );
    multi_address_free(orig);
    multi_address_free(result);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_full(void **state) {
    char addrstr[100];
    strcpy(addrstr, "/ip4/192.168.1.1/");
    // printf("INITIAL: %s\n", addrstr);
    struct multi_address *a;
    a = multi_address_new_from_string(addrstr);
    unsigned char *tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);

    // Remember, Decapsulation happens from right to left, never in reverse!

    //  printf("A STRING:%s\n", a->string);
    multi_address_encapsulate(a, "/udp/3333/");
    //  printf("A STRING ENCAPSULATED:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    multi_address_decapsulate(a, "udp");
    //  printf("A STRING DECAPSULATED UDP:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    multi_address_encapsulate(a, "/udp/3333/");
    //  printf("A STRING ENCAPSULATED UDP: %s\n", a->string);
    multi_address_encapsulate(
        a, "/ipfs/CIQKAFXKNLNOTSYX4XXER2AXMVZHINJF7AGJPYNCV2D4F7GPEJEW3TA=");
    //  printf("A STRING ENCAPSULATED IPFS:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    //  printf("TEST BYTE SIZE: %lu\n", a->bsize);

    struct multi_address *beta;
    beta = multi_address_new_from_bytes(a->bytes, a->bsize);
    // printf("B STRING: %s\n", beta->string);

    multi_address_free(a);
    multi_address_free(beta);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multiaddr_peer_id),
        cmocka_unit_test(test_multiaddr_get_peer_id),
        cmocka_unit_test(test_new_like_libp2p),
        cmocka_unit_test(test_new_from_string),
        cmocka_unit_test(test_hex_to_var),
        cmocka_unit_test(test_int_to_hex),
        cmocka_unit_test(test_multiaddr_utils),
        cmocka_unit_test(test_multiaddr_bytes),
        cmocka_unit_test(test_full)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
