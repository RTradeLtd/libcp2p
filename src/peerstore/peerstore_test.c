#include "peerstore/peerstore.h"
#include "testutils/testutils.h"
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


void peerstore_test_insert_peer(void **state) {
  peerstore_t *pst = peerstore_new_assert(100);

  for (int i = 0; i < 99; i++) {
    ecdsa_private_key_t *priv_key = new_ecdsa_private_key();
    assert(priv_key != NULL);
    peer_id_t *pid = libp2p_crypto_ecdsa_keypair_peerid(priv_key);
    assert(pid != NULL);
    public_key_t *pub_key = libp2p_crypto_ecdsa_keypair_public(priv_key);
    assert(pub_key != NULL);

    bool ok = peerstore_insert_peer(pst, pid->data, pub_key->data, pid->len, pub_key->data_size);
    assert(ok == true);

    libp2p_crypto_ecdsa_free(priv_key);
    libp2p_peer_id_free(pid);
  }

  // todo
  peerstore_free_peerstore(pst);
}

/*
  write your tests here
*/

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(peerstore_test_new_peerstore),
        cmocka_unit_test(peerstore_test_resize_if_needed_not_needed),
        cmocka_unit_test(peerstore_test_insert_peer)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}