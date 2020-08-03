#include "peerstore/peerstore.h"
#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
 * @brief frees up resources allocated for peer_t
 */
void peerstore_free_peer(peer_t *pr) {
    free(pr->peer_id);
    free(pr->public_key);
}

/*!
 * @brief frees up resources allocated for peerstore_t and all peers
 */
void peerstore_free_peerstore(peerstore_t *pst) {
    pthread_rwlock_wrlock(&pst->mutex);
    for (size_t i = 0; i < pst->num_peers; i++) {
        peerstore_free_peer(&pst->peers[i]);
    }
    free(pst->peers);
    pthread_rwlock_destroy(&pst->mutex);
    free(pst);
}

/*!
 * @brief check if we have a record for the given peer_id
 * @return Success: true
 * @return Failure: false
 */
bool peerstore_have_peer(peerstore_t *pst, unsigned char *peer_id) {
    bool have_peer = false;

    pthread_rwlock_rdlock(&pst->mutex);

    for (size_t i = 0; i < pst->num_peers; i++) {

        if (memcmp(pst->peers[i].peer_id, peer_id, pst->peers[i].peer_id_len) == 0) {
            have_peer = true;
            break;
        }
    }

    pthread_rwlock_unlock(&pst->mutex);

    return have_peer;
}

/*!
 * @brief used to insert a peer_id public_key pair into our peerstore
 * @details checks to see if we have a peer before inserting, and all inserts
 * @details are verified such that we ensure the given public_key matches the peer_id
 * @details however we should do this after we check if we have the peer to prevent
 * excessive work
 * @note if we already have the peer we report true and return early
 * @todo fix possible race condition with peerstore_have_peer check and the write
 * lock we claim here
 * @warning peer_id must be sha256 based
 * @todo add peer_id and public_key verification
 * @return Success (not addedd): true
 * @return Success(added): true
 * @return Failure: false
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool peerstore_insert_peer(peerstore_t *pst, unsigned char *peer_id,
                           unsigned char *public_key, size_t peer_id_len,
                           size_t public_key_len) {

    bool ok = false;

    // perform validation checksbefore we do any expensive computations and blockikng
    // locks

    /*! *@todo enable lru type cache to clean out old entries */
    if (pst->num_peers == pst->max_peers) {
        printf("too many peers stored\n");
        goto EXIT;
    }

    if (peerstore_have_peer(pst, peer_id) == true) {
        return true;
    }

    // validate the public key and peerid
    if (peerstore_validate_peer_id(peer_id, public_key, peer_id_len,
                                   public_key_len) == false) {
        goto EXIT;
    }

    pthread_rwlock_wrlock(&pst->mutex);

    bool resized = peerstore_resize_if_needed(pst);
    if (resized == false) {
        printf("failed to resize memory slot\n");
        goto EXIT;
    }

    peer_t pt = {.peer_id = peer_id,
                 .peer_id_len = peer_id_len,
                 .public_key = public_key,
                 .public_key_len = public_key_len};
    pst->peers[pst->num_peers] = pt;
    pst->num_peers += 1;

    ok = true;

EXIT:

    pthread_rwlock_unlock(&pst->mutex);

    return ok;
}

/*!
 * @brief performs a check to see if we need to allocate more memory
 * @details this is called after every insertion to the peerstore
 * @details so that the following call will have fresh memory to store data into
 */
bool peerstore_resize_if_needed(peerstore_t *pst) {

    if (pst->num_peers == pst->peers_size) {
        // realloc and increase available space by 2x
        pst->peers = realloc(pst->peers, (pst->peers_size * 2) * sizeof(peer_t));
        if (pst->peers == NULL) {
            return false;
        }
        pst->peers_size *= 2; // increase the size by 2
    }

    return true;
}

/*!
 * @brief returns the public key associated with the peer id
 * @details the contents of the peers public key (if found) are copied into output
 * @details if the public key length is greather than output length we false
 * @return Success: true
 * @return Failure: false
 */
bool peerstore_get_public_key(peerstore_t *pst, unsigned char *peer_id,
                              unsigned char *output, size_t output_len) {

    bool ok = false;

    pthread_rwlock_rdlock(&pst->mutex);

    for (size_t i = 0; i < pst->num_peers; i++) {
        if (memcmp(pst->peers[i].peer_id, peer_id, pst->peers[i].peer_id_len) == 0) {
            if (pst->peers[i].public_key_len > output_len) {
                goto EXIT;
            }
            memcpy(output, pst->peers[i].public_key, pst->peers[i].public_key_len);
            ok = true;
        }
    }

EXIT:

    pthread_rwlock_unlock(&pst->mutex);

    return ok;
}

/*!
 * @brief used to validate the peerID ensuring that the given public key matches the
 * given peerid
 * @param peer_id the actual peerid
 * @param public_key the public key in PEM format
 * @param peer_id_len the size of the peer_id
 * @param public_key_len the size of the public_key
 */
bool peerstore_validate_peer_id(unsigned char *peer_id, unsigned char *public_key,
                                size_t peer_id_len, size_t public_key_len) {


    unsigned char ret_peer_id[1024]; /*! @todo enable better length selection */
    memset(ret_peer_id, 0, 1024);
    size_t ret_peer_size = 1024;

    peer_id_t *pid = libp2p_new_peer_id_sha256(ret_peer_id, &ret_peer_size, public_key, public_key_len);
    if (pid == NULL) {
        printf("failed to hash public key\n");
        return false;
    }

    if (memcmp(peer_id, pid->data, peer_id_len) != 0) {
        printf("invalid peerid given\n");
        return false;
    }

    return true;
}

/*!
 * @brief returns a new instance of peerstore_t
 * @details is responsible for allocating memory and initializing the mutex
 * @details it initially allocates 2 slots of memory for the the peers memory
 * @details and will double this whenever the max is reached to prevent repeat
 * allocations
 */
peerstore_t *peerstore_new_peerstore(size_t max_peers) {
    peerstore_t *pst = calloc(1, sizeof(peerstore_t));
    if (pst == NULL) {
        return NULL;
    }
    pthread_rwlock_init(&pst->mutex, NULL);
    pst->peers = calloc(2, sizeof(peer_t));
    if (pst->peers == NULL) {
        free(pst);
        return NULL;
    }
    pst->max_peers = max_peers;
    pst->num_peers = 0;
    pst->peers_size = 2;
    return pst;
}