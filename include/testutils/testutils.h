#pragma once

#include "crypto/ecdsa.h"

/*!
 * @brief returns a new PEM formatted ECDSA private key
 * @details no need to check returned value for NULL PTR as we use assert internally
 */
unsigned char *new_ecdsa_private_key_pem(void);
ecdsa_private_key_t *new_ecdsa_private_key(void);