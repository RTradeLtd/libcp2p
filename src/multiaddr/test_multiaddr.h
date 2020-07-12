#pragma once
#pragma GCC diagnostic ignored "-Wanalyzer-possible-null-argument"
#include "multiaddr/multiaddr.h"
#include "multiaddr/varhexutils.h"

int test_new_like_libp2p() {
  int retVal = 0;
  char *ip = "10.211.55.2";
  int port = 4001;
  char str[strlen(ip) + 50];
  sprintf(str, "/ip4/%s/tcp/%d/", ip, port);
  struct MultiAddress *ma_string = multiaddress_new_from_string(str);
  // convert to binary
  struct MultiAddress *ma_binary =
      multiaddress_new_from_bytes(ma_string->bytes, ma_string->bsize);
  if (strcmp(ma_string->string, ma_binary->string) != 0) {
    fprintf(stderr, "%s does not equal %s\n", ma_string->string,
            ma_binary->string);
    goto exit;
  }

  retVal = 1;
exit:
  multiaddress_free(ma_string);
  multiaddress_free(ma_binary);
  return retVal;
}

int test_new_from_string() {
  struct MultiAddress *a =
      multiaddress_new_from_string("/ip4/127.0.0.1/tcp/8080/");
  printf("Number of Bytes: %lu, Bytes: ", a->bsize);
  for (int i = 0; i < (int)a->bsize; i++) {
    printf("%02x ", a->bytes[i]);
  }
  printf(" End of bytes\n");
  multiaddress_free(a);
  return 1;
}

int test_full() {
  char addrstr[100];
  strcpy(addrstr, "/ip4/192.168.1.1/");
  printf("INITIAL: %s\n", addrstr);
  struct MultiAddress *a;
  a = multiaddress_new_from_string(addrstr);
  unsigned char *tmp = Var_To_Hex((char *)a->bytes, a->bsize);
  printf("TEST BYTES: %s\n", tmp);
  free(tmp);

  // Remember, Decapsulation happens from right to left, never in reverse!

  printf("A STRING:%s\n", a->string);
  multiaddress_encapsulate(a, "/udp/3333/");
  printf("A STRING ENCAPSULATED:%s\n", a->string);
  tmp = Var_To_Hex((char *)a->bytes, a->bsize);
  printf("TEST BYTES: %s\n", tmp);
  free(tmp);
  multiaddress_decapsulate(a, "udp");
  printf("A STRING DECAPSULATED UDP:%s\n", a->string);
  tmp = Var_To_Hex((char *)a->bytes, a->bsize);
  printf("TEST BYTES: %s\n", tmp);
  free(tmp);
  multiaddress_encapsulate(a, "/udp/3333/");
  printf("A STRING ENCAPSULATED UDP: %s\n", a->string);
  multiaddress_encapsulate(
      a, "/ipfs/QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG");
  printf("A STRING ENCAPSULATED IPFS:%s\n", a->string);
  tmp = Var_To_Hex((char *)a->bytes, a->bsize);
  printf("TEST BYTES: %s\n", tmp);
  free(tmp);
  printf("TEST BYTE SIZE: %lu\n", a->bsize);

  struct MultiAddress *beta;
  beta = multiaddress_new_from_bytes(a->bytes, a->bsize);
  printf("B STRING: %s\n", beta->string);

  multiaddress_free(a);
  multiaddress_free(beta);
  return 1;
}

int test_hex_to_var() {
  size_t d;
  unsigned char *result = Hex_To_Var("04", &d);
  if (d != 1)
    return 0;
  if (result[0] != 4)
    return 0;

  if (result != NULL)
    free(result);
  return 1;
}

int test_int_to_hex() {
  int val = 2555351;
  char *result = Int_To_Hex(val);
  int retVal = Hex_To_Int(result);
  if (retVal != val)
    return 0;
  return 1;
}

int test_multiaddr_utils() {
  int retVal = 0;
  struct MultiAddress *addr =
      multiaddress_new_from_string("/ip4/127.0.0.1/tcp/4001/");
  if (!multiaddress_is_ip(addr)) {
    fprintf(stderr, "The address should be an IP\n");
    return 0;
  }
  char *ip = NULL;
  multiaddress_get_ip_address(addr, &ip);
  if (ip == NULL) {
    fprintf(stderr, "get_ip_address returned NULL\n");
    goto exit;
  }
  if (strcmp(ip, "127.0.0.1") != 0) {
    fprintf(stderr, "ip addresses are not equal\n");
    goto exit;
  }
  int port = multiaddress_get_ip_port(addr);
  if (port != 4001) {
    fprintf(stderr, "port incorrect. %d was returned instead of %d\n", port,
            4001);
    goto exit;
  }

  retVal = 1;
exit:
  if (ip != NULL)
    free(ip);
  if (addr != NULL)
    multiaddress_free(addr);
  return retVal;
}

int test_multiaddr_peer_id() {
  char *orig_address = "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
  char full_string[255];
  char *result = NULL;
  char *bytes = NULL;
  int retVal = 0, port = 0;
  struct MultiAddress *addr = NULL, *addr2 = NULL;

  sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

  addr = multiaddress_new_from_string(full_string);

  result = multiaddress_get_peer_id(addr);

  if (result == NULL || strcmp(result, orig_address) != 0)
    goto exit;

  free(result);
  result = NULL;

  // switch to bytes and back again to verify the peer id follows...

  // 1. display the original bytes
  result = (char *)Var_To_Hex((char *)addr->bytes, addr->bsize);
  fprintf(stderr, "Original Bytes: %s\n", result);
  free(result);
  result = NULL;

  // make a new MultiAddress from bytes
  bytes = malloc(addr->bsize);
  memcpy(bytes, addr->bytes, addr->bsize);
  addr2 = multiaddress_new_from_bytes((unsigned char *)bytes, addr->bsize);

  free(bytes);
  bytes = NULL;

  // 2. Display the resultant bytes
  result = (char *)Var_To_Hex((char *)addr2->bytes, addr2->bsize);
  fprintf(stderr, "New      Bytes: %s\n", result);
  free(result);
  result = NULL;

  if (strcmp(full_string, addr2->string) != 0) {
    fprintf(stderr, "Original string was %s but new string is %s\n",
            full_string, addr2->string);
    goto exit;
  }

  port = multiaddress_get_ip_port(addr2);
  if (port != 4001) {
    fprintf(stderr, "Original string had port 4001, but now reporting %d\n",
            port);
    goto exit;
  }

  result = multiaddress_get_peer_id(addr2);
  if (strcmp(result, orig_address) != 0) {
    fprintf(stderr, "New peer id %s does not match %s", result, orig_address);
    goto exit;
  }
  free(result);
  result = NULL;

  retVal = 1;
exit:
  if (addr != NULL)
    multiaddress_free(addr);
  if (addr2 != NULL)
    multiaddress_free(addr2);
  if (result != NULL)
    free(result);
  if (bytes != NULL)
    free(bytes);
  return retVal;
}

int test_multiaddr_get_peer_id() {
  const char *orig_address = "QmYwAPJzv5CZsnA625s3Xf2nemtYgPpHdWEz79ojWnPbdG";
  char full_string[255] = "";
  char *result = NULL;
  int retVal = 0;
  struct MultiAddress *addr = NULL;

  sprintf(full_string, "/ip4/127.0.0.1/tcp/4001/ipfs/%s/", orig_address);

  addr = multiaddress_new_from_string(full_string);

  result = multiaddress_get_peer_id(addr);

  if (result == NULL)
    goto exit;

  if (strcmp(orig_address, result) != 0)
    goto exit;

  retVal = 1;
exit:
  multiaddress_free(addr);
  free(result);
  result = NULL;
  return retVal;
}

int test_multiaddr_bytes() {
  int retVal = 0;
  char *orig_address = "/ip4/127.0.0.1/tcp/4001/";
  struct MultiAddress *orig = NULL, *result = NULL;

  orig = multiaddress_new_from_string(orig_address);

  result = multiaddress_new_from_bytes(orig->bytes, orig->bsize);

  if (strcmp(orig_address, result->string) != 0) {
    fprintf(stderr, "%s does not equal %s\n", orig_address, result->string);
    goto exit;
  }

  retVal = 1;
exit:
  if (orig != NULL)
    multiaddress_free(orig);
  if (result != NULL)
    multiaddress_free(result);
  return retVal;
}
