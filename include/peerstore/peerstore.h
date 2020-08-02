#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct peer {
    size_t peer_id_len;
    size_t public_key_len;
    unsigned char *peer_id;
    unsigned char *public_key;
} peer_t;

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
    pthread_rwlock_t mutex;
    peer_t *peers; /*! *@todo should we make this as **peers type */
} peerstore_t;

/*!
 * @brief frees up resources allocated for peer_t
 */
void peerstore_free_peer(peer_t *pr);

/*!
 * @brief frees up resources allocated for peerstore_t and all peers
 */
void peerstore_free_peerstore(peerstore_t *pst);

/*!
 * @brief check if we have a record for the given peer_id
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
 * @brief returns a new instance of peerstore_t
 * @details is responsible for allocating memory and initializing the mutex
 * @details it initially allocates 2 slots of memory for the the peers memory
 * @details and will double this whenever the max is reached to prevent repeat
 * allocations
 */
peerstore_t *peerstore_new_peerstore(size_t max_peers);