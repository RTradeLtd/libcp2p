#include "peerstore/peerstore.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*!
  * @brief returns the peerstore triggering and assertion failure if creation fails
*/
peerstore_t *peerstore_new_assert(size_t max_peers); 


/*!
  * @brief returns the peerstore triggering and assertion failure if creation fails
*/
peerstore_t *peerstore_new_assert(size_t max_peers) {
  peerstore_t *pst = peerstore_new_peerstore(max_peers);
  assert(pst != NULL);
  return pst;
}

void peerstore_test_new_peerstore(void **state) {
  peerstore_t *pst = peerstore_new_assert(100);
  assert(pst != NULL);
  assert(pst->max_peers = 100);
  assert(pst->peers_size = 2);
  assert(pst->num_peers == 0);

  peerstore_free_peerstore(pst);
}


/*!
  * @brief this doesnt actually test reszigin as that is covered by peer insertion tests
*/
void peerstore_test_resize_if_needed_not_needed(void **state) {
  peerstore_t *pst = peerstore_new_assert(100);
  bool ok = peerstore_resize_if_needed(pst);
  assert(ok == true);
  peerstore_free_peerstore(pst);
}

/*
  write your tests here
*/

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(peerstore_test_new_peerstore),
        cmocka_unit_test(peerstore_test_resize_if_needed_not_needed)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}