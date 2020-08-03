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
 * @file peerstore.h
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

#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*!
 * @struct peer
 * @typedef peer_t
 * @brief contains identification information about a single peer
 * @details bundles together a peers identifier (hash of public key) and the public
 * key itself
 */
typedef struct peer {
    size_t peer_id_len;        /*! @note the size of the peer id */
    size_t public_key_len;     /*! @note the size of the public key */
    unsigned char *peer_id;    /*! @note the actual peer id data */
    unsigned char *public_key; /*! @note the actual public key data */
} peer_t;

/*!
 * @struct peerstore
 * @typedef peerstore_t
 * @brief bundles together all known peer information
 * @note if accessing any internal members please use the read-write lock accordingly
 * @warning do not modify any information in the peer records directly
 * @warning all infoirmation must first be copied out to prevent accidental
 * overwrites
 */
typedef struct peerstore {
    /*!
     * @brief the maximum number of allowed peers
     */
    size_t max_peers;
    /*!
     * @brief the current number of stores peers
     */
    size_t num_peers;
    /*!
     * @brief identifies the current size of the peers member
     */
    size_t peers_size;
    /*!
     * @brief read-write lock ensuring fine-grained synchronization
     */
    pthread_rwlock_t mutex;
    /*!
     * @brief dynamic array/memory containing all known peers
     * @note should we do a poitner to poitner of peer_t ?? (aka: peer_t **peers)
     */
    peer_t *peers; /*! *@todo should we make this as **peers type */
} peerstore_t;

/*!
 * @brief frees up resources allocated for peer_t
 * @param pr an instance of peer_t
 * @note do not access pr after this function returns
 */
void peerstore_free_peer(peer_t *pr);

/*!
 * @brief frees up resources allocated for peerstore_t and all peers
 * @note in essence this "shuts down" the peerstore
 * @warning this deletes all information
 */
void peerstore_free_peerstore(peerstore_t *pst);

/*!
 * @brief check if we have information for the given peer_id
 * @param pst the actual peerstore
 * @param peer_id the peer we are checking
 * @return Success: true
 * @return Failure: false
 */
bool peerstore_have_peer(peerstore_t *pst, unsigned char *peer_id);

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
                           size_t public_key_len);

/*!
 * @brief performs a check to see if we need to allocate more memory
 * @warning caller must take care of locking
 * @details this is called after every insertion to the peerstore
 * @details so that the following call will have fresh memory to store data into
 * @return Success (enough size): true
 * @return Success (resized ok): true
 * @return Failure (resized failed): false
 */
bool peerstore_resize_if_needed(peerstore_t *pst);

/*!
 * @brief returns the public key associated with the peer id
 * @details the contents of the peers public key (if found) are copied into output
 * @details if the public key length is greather than output length we false
 * @return Success: true
 * @return Failure: false
 */
bool peerstore_get_public_key(peerstore_t *pst, unsigned char *peer_id,
                              unsigned char *output, size_t output_len);

/*!
 * @brief used to validate the peerID ensuring that the given public key matches the
 * given peerid
 * @param peer_id the actual peerid
 * @param public_key the public key in PEM format
 * @param peer_id_len the size of the peer_id
 * @param public_key_len the size of the public_key
 */
bool peerstore_validate_peer_id(unsigned char *peer_id, unsigned char *public_key,
                                size_t peer_id_len, size_t public_key_len);

/*!
 * @brief returns a new instance of peerstore_t
 * @details is responsible for allocating memory and initializing the mutex
 * @details it initially allocates 2 slots of memory for the the peers memory
 * @details and will double this whenever the max is reached to prevent repeat
 * allocations
 */
peerstore_t *peerstore_new_peerstore(size_t max_peers);