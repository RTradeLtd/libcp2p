// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file peerstore.c
 * @author Bonedaddy
 * @brief peerstore provides a logical grouping of all known peers
 * @details it provides access to an in-memory store of all known peers
 * @details peers are indexed by their peer identifier and contain the corresponding
 * public key
 * @note in the future this may also contain addressing information
 * @todo for secure connection negotiation, we need to figure out a way we can cache
 * the result
 * @todo of this negotiation to prevent having to continuously do it in between
 * sessions with a peer
 * @todo however we also need a way to "clear the cache" periodically to require
 * reauthentication for security
 */

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
 * @param pr an instance of peer_t
 * @note do not access pr after this function returns
 */
void peerstore_free_peer(peer_t *pr) {
    free(pr->peer_id);
    free(pr->public_key);
}

/*!
 * @brief frees up resources allocated for peerstore_t and all peers
 * @note in essence this "shuts down" the peerstore
 * @warning this deletes all information
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
 * @brief check if we have information for the given peer_id
 * @param pst the actual peerstore
 * @param peer_id the peer we are checking
 * @return Success: true
 * @return Failure: false
 */
bool peerstore_have_peer(peerstore_t *pst, unsigned char *peer_id) {
    bool have_peer = false;

    pthread_rwlock_rdlock(&pst->mutex);

    for (size_t i = 0; i < pst->num_peers; i++) {

        // check to see if the peer_id we are searching for matches this entry
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
bool peerstore_insert_peer(peerstore_t *pst, unsigned char *peer_id,
                           unsigned char *public_key, size_t peer_id_len,
                           size_t public_key_len, size_t num_addrs, multi_addr_t **addrs) {

    bool ok = false;

    // perform validation checks to prevent expensive computation if possible

    /*! @todo this isn't entirely race free, and it is possible for 2 requests to
     * occur in a row */

    /*! *@todo enable lru type cache to clean out old entries */
    if (pst->num_peers == pst->max_peers) {
        printf("too many peers stored\n");
        return ok;
    }

    /*! @todo dont insert  if we already have data for this peer */
    /*! @todo this is where the race condition lies */
    if (peerstore_have_peer(pst, peer_id) == true) {
        return ok;
    }

    /*! @note compute the peerID from the public key we are given */
    /*! @note we then match this with the peerID we are being given */
    /*! @note this ensures we dont enter any bad data into the peerstore */
    if (peerstore_validate_peer_id(peer_id, public_key, peer_id_len,
                                   public_key_len) == false) {
        return ok;
    }

    pthread_rwlock_wrlock(&pst->mutex);

    /*! @note this checks to see if we have enough room in the dynamic array */
    /*! @note if we dont have enough room then we will resize increase space by 2x */
    /*! @note if that isn't greater than the max peers size. if it is greater */
    /*! @note then we use the max peers as the limit */
    /*! @note returns true even if we dint need to resize because we have enough room
     */
    bool resized = peerstore_resize_if_needed(pst);
    if (resized == false) {
        printf("failed to resize memory slot\n");
        goto EXIT;
    }

    /*! @note right now im not sure if we should store this in the peerstore */
    /*! @note or use a pointer. i think like this would consume less memory */
    peer_t pt = {.peer_id_len = peer_id_len, .public_key_len = public_key_len, .num_addrs = num_addrs, .addrs = addrs};

    // allocate memory to hold peer id
    pt.peer_id = calloc(1, peer_id_len);
    if (peer_id == NULL) {
        goto EXIT;
    }

    // allocate memory to hold public key
    pt.public_key = calloc(1, public_key_len);
    if (pt.public_key == NULL) {
        free(pt.peer_id);
        goto EXIT;
    }

    // copy peerid and public key data into struct
    memcpy(pt.peer_id, peer_id, peer_id_len);
    memcpy(pt.public_key, public_key, public_key_len);

    // store data in the peerstore and increase count
    pst->peers[pst->num_peers] = pt;
    pst->num_peers += 1;

    ok = true;

EXIT:

    pthread_rwlock_unlock(&pst->mutex);

    return ok;
}

/*!
 * @brief performs a check to see if we need to allocate more memory
 * @warning caller must take care of locking
 * @details this is called after every insertion to the peerstore
 * @details so that the following call will have fresh memory to store data into
 * @return Success (enough size): true
 * @return Success (resized ok): true
 * @return Failure (resized failed): false
 */
bool peerstore_resize_if_needed(peerstore_t *pst) {

    if (pst->num_peers == pst->peers_size) {
        /*!
         * @note set the new peers size to twice the current
         * @note but if it ends up being more than the max peers we want
         * @note lets just use the max limit instead
         */
        size_t new_peers_size = pst->peers_size * 2;

        if (new_peers_size > pst->max_peers) {
            new_peers_size = pst->max_peers;
        }

        // realloc and increase available space by 2x (or max peers)
        pst->peers = realloc(pst->peers, new_peers_size * sizeof(peer_t));
        if (pst->peers == NULL) {
            return false;
        }

        pst->peers_size = new_peers_size;
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

            // copy the data to prevent accidental overwrites
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

    bool ok = false;
    size_t ret_peer_size = 1024;
    unsigned char ret_peer_id[1024]; /*! @todo enable better length selection */
    memset(ret_peer_id, 0, 1024);

    peer_id_t *pid = libp2p_new_peer_id_sha256(ret_peer_id, &ret_peer_size,
                                               public_key, public_key_len);
    if (pid == NULL) {
        return ok;
    }

    if (memcmp(peer_id, pid->data, peer_id_len) == 0) {
        ok = true;
    }

    libp2p_peer_id_free(pid);

    return ok;
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
    pst->peers = calloc(2, sizeof(peer_t));
    if (pst->peers == NULL) {
        free(pst);
        return NULL;
    }
    pthread_rwlock_init(&pst->mutex, NULL);
    pst->max_peers = max_peers;
    pst->num_peers = 0;
    pst->peers_size = 2;
    return pst;
}