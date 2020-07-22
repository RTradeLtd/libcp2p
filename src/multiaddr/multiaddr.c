#include "multiaddr/multiaddr.h"

#include <string.h>
#include <sys/socket.h>

#include "multiaddr/protocols.h"
#include "multiaddr/protoutils.h"
#include "utils/varhexutils.h"
#include "utils/varint.h"

int strpos(char *haystack, char *needle) {
    char *p = strstr(haystack, needle);
    if (p) {
        return p - haystack;
    } else {
        return -1; // Not found = -1.
    }
}

/**
 * Construct a new MultiAddress struct
 * @returns an empty MultiAddress struct
 */
struct MultiAddress *multiaddress_new() {
    struct MultiAddress *out =
        (struct MultiAddress *)malloc(sizeof(struct MultiAddress));
    if (out != NULL) {
        out->bsize = 0;
        out->bytes = NULL;
        out->string = NULL;
    }
    return out;
}

/**
 * construct a new MultiAddress from bytes
 * @param byteaddress the byte array
 * @param size the size of the byte array
 * @returns a new MultiAddress struct filled in, or NULL on error
 * @warning this currently is failing tests
 */
struct MultiAddress *
multiaddress_new_from_bytes(const uint8_t *byteaddress,
                            int size) // Construct new address from bytes
{
    struct MultiAddress *out = multiaddress_new();
    if (out != NULL) {
        if (byteaddress != NULL) {
            out->bytes = malloc(size);
            if (out->bytes == NULL) {
                multiaddress_free(out);
                return NULL;
            }
            out->bsize = size;
            memcpy(out->bytes, byteaddress, size);
            if (!bytes_to_string(&out->string, byteaddress, size)) {
                multiaddress_free(out);
                return NULL;
            }
        } else {
            multiaddress_free(out);
            return NULL;
        }
    }
    return out;
}

struct MultiAddress *multiaddress_new_from_string(
    const char *straddress) // Construct new address from string
{
    struct MultiAddress *out = multiaddress_new();
    if (out != NULL) {
        out->string = malloc(strlen(straddress) + 1);
        if (out->string == NULL) {
            multiaddress_free(out);
            return NULL;
        }
        strcpy(out->string, straddress);

        if (string_to_bytes(&(out->bytes), &out->bsize, out->string,
                            strlen(out->string)) == 0) {
            multiaddress_free(out);
            return NULL;
        }
    }
    return out;
}

int multiaddress_is_ip(const struct MultiAddress *in) {
    // previously was this but -pedantic caused warn
    // if (in->bytes > 0) {
    if (in->bytes != 0) {
        int byte = in->bytes[0];

        if (byte == 4 || byte == 41)
            return 1;
    }
    return 0;
}
int multiaddress_is_ip4(const struct MultiAddress *in) {
    return in->bytes[0] == 4;
}

int multiaddress_is_ip6(const struct MultiAddress *in) {
    return in->bytes[0] == 41;
}

int multiaddress_get_ip_family(const struct MultiAddress *in) {
    if (in->bytes[0] == 4)
        return AF_INET;
    if (in->bytes[0] == 41)
        return AF_INET6;
    return 0;
}

/***
 * Pulls the textual representation of the IP address from a multihash
 * @param in the multihash to parse
 * @param ip where to put the ip address
 * @returns true(1) on success, otherwise 0
 */
int multiaddress_get_ip_address(const struct MultiAddress *in, char **ip) {
    // the incoming address is not what was expected
    if (strncmp(in->string, "/ip4/", 5) != 0 &&
        strncmp(in->string, "/ip6/", 5) != 0) {
        return 0;
    }
    if (strstr(in->string, "/tcp/") == NULL && strstr(in->string, "/udp/") == NULL) {
        return 0;
    }
    // ip
    char *str = malloc(strlen(in->string));
    if (str == NULL) {
        return 0;
    }
    strcpy(str, &in->string[5]); // gets rid of /ip4/
    char *pos = strchr(str, '/');
    pos[0] = 0;
    *ip = malloc(strlen(str) + 1);
    if (*ip == NULL) {
        return 0;
    }
    strcpy(*ip, str);
    free(str);
    return 1;
}

char *multiaddress_get_ip_port_c(const struct MultiAddress *in) {
    char *ptr = strstr(in->string, "/tcp/");
    if (ptr == NULL) {
        ptr = strstr(in->string, "/udp/");
    }
    if (ptr == NULL) {
        return NULL;
    }
    ptr += 5;
    char *end_ptr = strstr(ptr, "/");
    if (end_ptr == NULL) {
        return ptr;
    }
    char str[end_ptr - ptr + 1];
    memcpy(str, ptr, end_ptr - ptr);
    str[end_ptr - ptr] = '\0';
    char *ret_str = calloc(sizeof(char), strlen(str));
    memcpy(ret_str, str, strlen(str));
    return ret_str;
}

/***
 * Pulls the IP port from a multiaddress
 * @param in the multiaddress
 * @param port where to put the port
 * @returns the port, or a negative number for an error
 */
int multiaddress_get_ip_port(const struct MultiAddress *in) {
    char *ptr = strstr(in->string, "/tcp/");
    if (ptr == NULL)
        ptr = strstr(in->string, "/udp/");
    if (ptr == NULL)
        return -1;
    ptr += 5;
    char *end_ptr = strstr(ptr, "/");
    if (end_ptr == NULL) {
        return atoi(ptr);
    }
    char str[end_ptr - ptr + 1];
    memcpy(str, ptr, end_ptr - ptr);
    str[end_ptr - ptr] = '\0';
    return atoi(str);
}

char *multiaddress_get_peer_id(const struct MultiAddress *in) {
    char *result = NULL;
    int str_len = 0;
    char *slash = NULL;
    char *ptr = NULL;

    ptr = strstr(in->string, "/ipfs/");
    if (ptr != NULL && ptr[6] != 0) {
        ptr += 6;
        str_len = strlen(ptr);
        slash = strchr(ptr, '/');
        if (slash != NULL) {
            str_len = slash - ptr;
        }
        if (str_len > 0) {
            result = malloc(str_len + 1);
            if (result != NULL) {
                memset(result, 0, str_len + 1);
                memcpy(result, ptr, str_len);
            }
        }
    }
    return result;
}

void multiaddress_free(struct MultiAddress *in) {
    if (in != NULL) {
        if (in->bytes != NULL)
            free(in->bytes);
        if (in->string != NULL)
            free(in->string);
        free(in);
        in = NULL;
    }
}

/**
 * Copy a multiaddress from one memory location to another
 * @param in the source
 * @returns the new struct MultiAddress or NULL if there was a problem (i.e. out
 * of memory)
 */
struct MultiAddress *multiaddress_copy(const struct MultiAddress *in) {
    struct MultiAddress *out = NULL;
    if (in != NULL) {
        out = (struct MultiAddress *)malloc(sizeof(struct MultiAddress));
        if (out != NULL) {
            if (in->bsize > 0) {
                out->bytes = malloc(in->bsize);
                if (out->bytes == NULL) {
                    free(out);
                    return NULL;
                }
                out->bsize = in->bsize;
                memcpy(out->bytes, in->bytes, out->bsize);
            } // bytes need to be copied
            if (in->string != NULL) {
                out->string = malloc(strlen(in->string) + 1);
                if (out->string == NULL) {
                    if (out->bsize > 0)
                        free(out->bytes);
                    free(out);
                    return NULL;
                }
                strcpy(out->string, in->string);
            } // string needs to be copied
        }     // structure allocated
    }         // good parameters
    return out;
}

/**
 * Put a string into the MultiAddress and recalculate the bytes
 * @param result the struct
 * @param string the new string
 */
int multiaddress_encapsulate(struct MultiAddress *result, char *string) {
    if (result != NULL && string != NULL) {
        // remove the old values
        if (result->bytes != NULL)
            free(result->bytes);
        result->bytes = NULL;
        result->bsize = 0;
        char *exstr;
        if (string[0] == '/') {
            exstr = (char *)malloc(strlen(result->string) + 1);
        } else {
            exstr = (char *)malloc(strlen(result->string));
        }
        if (exstr == NULL) {
            return 1;
        }
        strcpy(exstr, result->string);
        free(result->string);
        // insert the new values
        result->string = malloc(strlen(string) + strlen(exstr) + 1);
        if (result->string == NULL) {
            multiaddress_free(result);
            return 0;
        }
        strcpy(result->string, exstr);
        free(exstr);
        if (string[0] == '/') {
            strcat(result->string, string + 1);
        } else {
            strcat(result->string, string);
        }
        if (string_to_bytes(&result->bytes, &result->bsize, result->string,
                            strlen(result->string) + 1) == 0) {
            multiaddress_free(result);
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

/**
 * Find scri and remove it from the resultant value
 * (ie /ip4/127.0.0.1/tcp/4001 becomes ip4/127.0.0.1 when you call
 * decapsulate(addr, "/tcp")
 * @param result the address to work with
 * @param srci the string to look for
 */
int multiaddress_decapsulate(struct MultiAddress *result, char *srci) {
    if (result != NULL && srci != NULL) {
        char *procstr = NULL;
        procstr = result->string;
        int i = 0;
        int sz = strlen(procstr);
        char *src = NULL;
        src = srci;
        // change slash to space
        for (i = 0; i < sz; i++) {
            if (procstr[i] == '/') {
                procstr[i] = ' ';
            }
        }
        int pos = -1;
        pos = strpos(procstr, src);
        if (pos != -1) {
            // fill rest with 0s
            for (i = pos; i < sz; i++) {
                procstr[i] = '\0';
            }
            // replace space with slash
            for (i = 0; i < sz; i++) {
                if (procstr[i] == ' ') {
                    procstr[i] = '/';
                }
            }
            // Bytes update
            if (result->bytes != NULL)
                free(result->bytes);
            result->bytes = NULL;
            result->bsize = 0;
            if (string_to_bytes(&result->bytes, &result->bsize, result->string,
                                strlen(result->string) + 1) == 0) {
                multiaddress_free(result);
                return 0;
            }
            return 1;
        } else {
            return 0;
        }
        return 0;
    } else {
        return 0;
    }
}

/**
 * Check to see how these two addresses compare
 * @param a side A
 * @param b side B
 * @returns <0 if B > A; >0 if A > B; 0 if A == B
 */
int multiaddress_compare(const struct MultiAddress *a,
                         const struct MultiAddress *b) {
    if (a == NULL && b == NULL)
        return 0;
    if (a == NULL && b != NULL)
        return -1;
    if (a != NULL && b == NULL)
        return 1;
    int total = b->bsize - a->bsize;
    if (total != 0)
        return total;
    for (size_t i = 0; i < b->bsize; i++) {
        total = b->bytes[i] - a->bytes[i];
        if (total != 0)
            return total;
    }
    return 0;
}

/**
 * Check to see how these two addresses compare, ignoring IP address,
 * only looking at the first /ipfs/ID hash
 * @param a side A
 * @param b side B
 * @returns <0 if B > A; >0 if A > B; 0 if A == B
 */
int multiaddress_compare_id(const struct MultiAddress *a,
                            const struct MultiAddress *b) {
    char *a_id = multiaddress_get_peer_id(a);
    char *b_id = multiaddress_get_peer_id(b);
    if (a_id == NULL && b_id == NULL)
        return 0;
    if (a_id == NULL && b_id != NULL)
        return -1;
    if (a_id != NULL && b_id == NULL)
        return 1;
    int retVal = strcmp(a_id, b_id);
    if (a_id != NULL)
        free(a_id);
    if (b_id != NULL)
        free(b_id);
    return retVal;
}
