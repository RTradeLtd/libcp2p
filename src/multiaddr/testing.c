#include <stdio.h>

#include "test_multiaddr.h"

const char *names[] = {"test_new_from_string",
                       "test_full",
                       "test_hex_to_var",
                       "test_int_to_hex",
                       "test_multiaddr_utils",
                       "test_multiaddr_peer_id",
                       "test_multiaddr_get_peer_id",
                       "test_multiaddr_bytes",
                       "test_new_like_libp2p"};

int (*funcs[])(void) = {test_new_from_string,
                        test_full,
                        test_hex_to_var,
                        test_int_to_hex,
                        test_multiaddr_utils,
                        test_multiaddr_peer_id,
                        test_multiaddr_get_peer_id,
                        test_multiaddr_bytes,
                        test_new_like_libp2p};

int testit(const char *name, int (*func)(void)) {
  printf("Testing %s...\n", name);
  int retVal = func();
  if (retVal)
    printf("%s success!\n", name);
  else
    printf("** Uh oh! %s failed.**\n", name);
  return retVal;
}

int main(int argc, char **argv) {
  int counter = 0;
  int tests_ran = 0;
  char *test_wanted;
  int only_one = 0;
  if (argc > 1) {
    only_one = 1;
    if (argv[1][0] == '\'') { // some shells put quotes around arguments
      argv[1][strlen(argv[1]) - 1] = 0;
      test_wanted = &(argv[1][1]);
    } else
      test_wanted = argv[1];
  }
  int array_length = sizeof(funcs) / sizeof(funcs[0]);
  int array2_length = sizeof(names) / sizeof(names[0]);
  if (array_length != array2_length) {
    printf("Test arrays are not of the same length. Funcs: %d, Names: %d\n",
           array_length, array2_length);
  }
  for (int i = 0; i < array_length; i++) {
    if (only_one) {
      const char *currName = names[i];
      if (strcmp(currName, test_wanted) == 0) {
        tests_ran++;
        counter += testit(names[i], funcs[i]);
      }
    } else if (!only_one) {
      tests_ran++;
      counter += testit(names[i], funcs[i]);
    }
  }

  if (tests_ran == 0)
    printf("***** No tests found *****\n");
  else {
    if (tests_ran - counter > 0) {
      printf("***** There were %d failed test(s) (%d successful) *****\n",
             tests_ran - counter, counter);
    } else {
      printf("All %d tests passed\n", tests_ran);
    }
  }
  return 1;
}
