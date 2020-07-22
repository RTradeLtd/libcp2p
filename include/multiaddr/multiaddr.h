#ifndef MULTIADDR
#define MULTIADDR
#include <string.h>

#include "protocols.h"
#include "protoutils.h"
#include "utils/varhexutils.h"
#include "utils/varint.h"

/**
 * Normally, addresses have been represented using string addresses, like:

        tcp4://127.0.0.1:1234
        udp4://10.20.30.40:5060
        ws://1.2.3.4:5678
        tcp6://[1fff:0:a88:85a3::ac1f]:8001
        This isn't optimal. Instead, addresses should be formatted so:

        Binary format:

        (varint proto><n byte addr>)+
        <1 byte ipv4 code><4 byte ipv4 addr><1 byte udp code><2 byte udp port>
        <1 byte ipv6 code><16 byte ipv6 addr><1 byte tcp code><2 byte tcp port>

        String format:

        (/<addr str code>/<addr str rep>)+
        /ip4/<ipv4 str addr>/udp/<udp int port>
        /ip6/<ipv6 str addr>/tcp/<tcp int port>
 */

typedef struct MultiAddress {
    // A MultiAddress represented as a string
    char *string;
    // A MultiAddress represented as an array of bytes
    //<varint proto><n byte addr><1 byte protocol code><4 byte ipv4 address
    // or 16
    // byte ipv6 address><1 byte tcp/udp code><2 byte port>
    uint8_t *bytes;
    size_t bsize;
} multi_addr_t;

int strpos(char *haystack, char *needle);

struct MultiAddress *
multiaddress_new_from_bytes(const uint8_t *byteaddress,
                            int size); // Construct new address from bytes

struct MultiAddress *multiaddress_new_from_string(
    const char *straddress); // Construct new address from string

void multiaddress_free(struct MultiAddress *in);

struct MultiAddress *multiaddress_copy(const struct MultiAddress *source);

// helpers to parse the MultiAddress struct

int multiaddress_encapsulate(struct MultiAddress *result, char *string);

int multiaddress_decapsulate(struct MultiAddress *result, char *srci);

int multiaddress_is_ip(const struct MultiAddress *in);

int multiaddress_is_ip4(const struct MultiAddress *in);

int multiaddress_is_ip6(const struct MultiAddress *in);

int multiaddress_get_ip_family(const struct MultiAddress *in);

/***
 * Pulls the textual representation of the IP address from a multihash
 * @param in the multihash to parse
 * @param ip where to put the ip address
 * @returns true(1) on success, otherwise 0
 */
int multiaddress_get_ip_address(const struct MultiAddress *in, char **ip);

/***
 * Pulls the IP port from a multiaddress
 * @param in the multiaddress
 * @param port where to put the port
 * @returns the port, or a negative number for an error
 */
int multiaddress_get_ip_port(const struct MultiAddress *in);

char *multiaddress_get_ip_port_c(const struct MultiAddress *in);

/**
 * Pulls the peer ID out of a multiaddress struct
 * @param in the MultiAddress
 * @returns the peer id string, or NULL NOTE: This allocates memory that needs
 * to be freed
 */
char *multiaddress_get_peer_id(const struct MultiAddress *in);

/**
 * Check to see how these two addresses compare
 * @param a side A
 * @param b side B
 * @returns <0 if B > A; >0 if A > B; 0 if A == B
 */
int multiaddress_compare(const struct MultiAddress *a, const struct MultiAddress *b);

/**
 * Check to see how these two addresses compare, ignoring IP address, only
 * looking at ID hash
 * @param a side A
 * @param b side B
 * @returns <0 if B > A; >0 if A > B; 0 if A == B
 */
int multiaddress_compare_id(const struct MultiAddress *a,
                            const struct MultiAddress *b);

#endif
