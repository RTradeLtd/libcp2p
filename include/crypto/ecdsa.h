
#include "mbedtls/ecdsa.h"

struct EcdsaPrivateKey { char *hello; };

/*! @brief used to generate an ECDSA keypair
  * @param private_key the place to store the private key
  * @returns Fail: 0
  * @returns Success: 1
*/
int libp2p_crypto_ecdsa_generation_keypair(struct EcdsaPrivateKey *private_key);
