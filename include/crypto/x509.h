/*! @file x509.h
  * @brief provides x509 cert management tooling
*/

#pragma once

#include "rsa.h"

/**
 * @brief Turn an RsaPrivateKey struct into a DER formatted array of bytes
 * @param private_key the RsaPrivateKey struct
 * @param bytes an array of 1600 bytes to hold the DER formatted data
 * @returns true(1) on success
 */
int libp2p_encoding_x509_private_key_to_der(struct RsaPrivateKey *private_key,
                                            unsigned char *bytes[1600]);

/**
 * @brief Take a DER formatted char array and turn it into an RsaPrivateKey
 * @param der the DER formatted char array
 * @param der_length the number of bytes in the char array
 * @param private_key the struct to put the data in. Memory should have already
 * been allocated for the struct.
 * @returns true(1) on success
 */
int libp2p_encoding_x509_der_to_private_key(unsigned char *der, size_t der_length,
                                            struct RsaPrivateKey *private_key);