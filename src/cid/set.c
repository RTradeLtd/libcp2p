#include "cid/cid.h"
#include <stdlib.h>
#include <string.h>

struct CidSet *ipfs_cid_set_new() {
    return calloc(1, sizeof(struct CidSet));
}

void ipfs_cid_set_destroy(struct CidSet **set) {
    struct CidSet *prev;

    if (set) {
        while (*set) {
            prev = *set;
            *set = (*set)->next;
            if (prev->cid) {
                free(prev->cid);
            }
            free(prev);
        }
    }
}

int ipfs_cid_set_add(struct CidSet *set, struct Cid *cid, int visit) {
    if (!set || !cid) {
        // return ErrInvalidParam;
        return -1;
    }
    if (!set->cid) { // First item.
        set->cid = malloc(sizeof(struct Cid));
        if (!set->cid) {
            // return ErrAllocFailed;
            return -1;
        }
        memcpy(set->cid, cid, sizeof(struct Cid));
        set->cid->hash = calloc(1, cid->hash_length);
        if (!set->cid->hash) {
            free(set->cid);
            // return ErrAllocFailed;
            return -1;
        }
        memcpy(set->cid->hash, cid->hash, cid->hash_length);
        return 0;
    }
    for (;;) {
        if ((set->cid->hash_length == cid->hash_length) &&
            (memcmp(set->cid->hash, cid->hash, cid->hash_length) == 0)) {
            // Already added.
            if (!visit) {
                // update with new cid.
                free(set->cid->hash);
                memcpy(set->cid, cid, sizeof(struct Cid));
                set->cid->hash = calloc(1, cid->hash_length);
                if (!set->cid->hash) {
                    return -1;
                }
                memcpy(set->cid->hash, cid->hash, cid->hash_length);
            }
            return 0;
        }
        if (!set->next) {
            set->next = ipfs_cid_set_new();
            if (!set->next) {
                return -1;
            }
            set = set->next;
            set->cid = malloc(sizeof(struct Cid));
            if (!set->cid) {
                return -1;
            }
            memcpy(set->cid, cid, sizeof(struct Cid));
            set->cid->hash = calloc(1, cid->hash_length);
            if (!set->cid->hash) {
                return -1;
            }
            memcpy(set->cid->hash, cid->hash, cid->hash_length);
            return 0;
        }
        set = set->next;
    }
    // this should never get hit
    return 0;
}

int ipfs_cid_set_has(struct CidSet *set, struct Cid *cid) {
    if (!set || !cid || !set->cid) {
        return 0;
    }
    for (;;) {
        if ((set->cid->hash_length == cid->hash_length) &&
            (memcmp(set->cid->hash, cid->hash, cid->hash_length) == 0)) {
            return 1; // has
        }
        if (!set->next) {
            return 0; // end without found.
        }
        set = set->next;
    }
    return 0;
}

int ipfs_cid_set_remove(struct CidSet *set, struct Cid *cid) {
    struct CidSet *prev = set;

    if (!set || !cid || !set->cid) {
        return 0;
    }
    for (;;) {
        if ((set->cid->hash_length == cid->hash_length) &&
            (memcmp(set->cid->hash, cid->hash, cid->hash_length) == 0)) {
            free(set->cid);
            if (prev == set) { // first item
                set = set->next;
                if (!set) {
                    prev->cid = NULL;
                    return 1;
                }
                prev->cid = set->cid;
            } else {
                prev->next = set->next;
            }
            free(set);
            return 1; // removed
        }
        if (!set->next) {
            return 0; // end without found.
        }
        prev = set;
        set = set->next;
    }
    return 0;
}

int ipfs_cid_set_len(struct CidSet *set) {
    int len;

    if (!set || !set->cid) {
        return 0;
    }

    for (len = 0; set; len++, set = set->next)
        ;

    return len;
}

unsigned char **ipfs_cid_set_keys(struct CidSet *set) {
    int i, len = ipfs_cid_set_len(set);
    unsigned char **ret;

    ret = calloc(len + 1, sizeof(char *));
    if (ret) {
        for (i = 0; i < len; len++) {
            if (set && set->cid && set->cid->hash) {
                ret[i] = calloc(1, set->cid->hash_length + 1);
                if (ret[i]) {
                    memcpy(ret[i], set->cid->hash, set->cid->hash_length);
                }
            }
            set = set->next;
        }
    }
    return ret;
}

int ipfs_cid_set_foreach(struct CidSet *set, int (*func)(struct Cid *)) {
    int err = 0;

    while (set) {
        if (set->cid) {
            err = func(set->cid);
            if (err) {
                return err;
            }
        }
        set = set->next;
    }

    return err;
}
