#include "multiaddr/multiaddr.h"
#include "multiaddr/varhexutils.h"
#include <assert.h>
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define assert__(x) for (; !(x); assert(x))

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
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_new_from_string(void **state) {
    struct MultiAddress *a =
        multiaddress_new_from_string("/ip4/127.0.0.1/tcp/8080/");
    assert(a != NULL);
    /* printf("Number of Bytes: %lu, Bytes: ", a->bsize);
    for (int i = 0; i < (int)a->bsize; i++) {
        printf("%02x ", a->bytes[i]);
    }
    printf(" End of bytes\n");
    */
    multiaddress_free(a);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_new_like_libp2p(void **state) {
    int retVal = 0;
    char *ip = "10.211.55.2";
    int port = 4001;
    char str[strlen(ip) + 50];
    sprintf(str, "/ip4/%s/tcp/%d/", ip, port);
    struct MultiAddress *ma_string = multiaddress_new_from_string(str);
    // convert to binary
    struct MultiAddress *ma_binary =
        multiaddress_new_from_bytes(ma_string->bytes, ma_string->bsize);
    assert(strcmp(ma_string->string, ma_binary->string) == 0);
    multiaddress_free(ma_string);
    multiaddress_free(ma_binary);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_utils(void **state) {
    int retVal = 0;
    struct MultiAddress *addr =
        multiaddress_new_from_string("/ip4/127.0.0.1/tcp/4001/");
    assert(multiaddress_is_ip(addr));
    char *ip = NULL;
    multiaddress_get_ip_address(addr, &ip);
    assert(ip != NULL);
    assert(strcmp(ip, "127.0.0.1") == 0);
    int port = multiaddress_get_ip_port(addr);
    assert(port == 4001);

    free(ip);
    multiaddress_free(addr);
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
    assert__(result) { printf("result is not NULL\n"); };
    assert__(strcmp(result, orig_address) == 0) {
        printf("Original string was %s but new string is %s\n", result,
               orig_address);
    }
    free(result);

    // switch to bytes and back again to verify the peer id follows...

    // 1. display the original bytes
    result = Var_To_Hex(addr->bytes, addr->bsize);
    // fprintf(stderr, "Original Bytes: %s\n", result);
    free(result);

    /* TODO(bonedaddy): fix as this has an error
     // make a new MultiAddress from bytes
     bytes = malloc(addr->bsize);
     memcpy(bytes, addr->bytes, addr->bsize);
     // TODO(bonedaddy): this is bugged and doesnt return the correct
     multiaddress
     // interestingly enough the new bytes matches the old bytes???
     addr2 = multiaddress_new_from_bytes(bytes, addr->bsize);
     assert(addr2 != NULL);
     free(bytes);

     // 2. Display the resultant bytes
     result = Var_To_Hex(addr2->bytes, addr2->bsize);
     fprintf(stderr, "New      Bytes: %s\n", result);
     free(result);

     assert__(strcmp(full_string, addr2->string) == 0) {
         printf("Original string was %s but new string is %s\n", full_string,
     addr2->string);
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
     */
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_multiaddr_bytes(void **state) {
    int retVal = 0;
    char *orig_address = "/ip4/127.0.0.1/tcp/4001/";
    struct MultiAddress *orig = NULL, *result = NULL;

    orig = multiaddress_new_from_string(orig_address);

    result = multiaddress_new_from_bytes(orig->bytes, orig->bsize);

    assert(result != NULL);
    assert(strcmp(orig_address, result->string) == 0);
    multiaddress_free(orig);
    multiaddress_free(result);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_full(void **state) {
    char addrstr[100];
    strcpy(addrstr, "/ip4/192.168.1.1/");
    // printf("INITIAL: %s\n", addrstr);
    struct MultiAddress *a;
    a = multiaddress_new_from_string(addrstr);
    unsigned char *tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);

    // Remember, Decapsulation happens from right to left, never in reverse!

    //  printf("A STRING:%s\n", a->string);
    multiaddress_encapsulate(a, "/udp/3333/");
    //  printf("A STRING ENCAPSULATED:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    multiaddress_decapsulate(a, "udp");
    //  printf("A STRING DECAPSULATED UDP:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    multiaddress_encapsulate(a, "/udp/3333/");
    //  printf("A STRING ENCAPSULATED UDP: %s\n", a->string);
    multiaddress_encapsulate(
        a, "/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG");
    //  printf("A STRING ENCAPSULATED IPFS:%s\n", a->string);
    tmp = Var_To_Hex((char *)a->bytes, a->bsize);
    //  printf("TEST BYTES: %s\n", tmp);
    free(tmp);
    //  printf("TEST BYTE SIZE: %lu\n", a->bsize);

    struct MultiAddress *beta;
    beta = multiaddress_new_from_bytes(a->bytes, a->bsize);
    // printf("B STRING: %s\n", beta->string);

    multiaddress_free(a);
    multiaddress_free(beta);
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
        cmocka_unit_test(test_full)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
