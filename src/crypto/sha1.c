
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

#include "mbedtls/sha1.h"
#include "crypto/sha1.h"

/***
 * hash a string using SHA1
 * @warning doesnt appear to work
 * @param input the input string
 * @param input_length the length of the input string
 * @param output where to place the results, should be 20 bytes
 * @returns 1
 */
int libp2p_crypto_hashing_sha1(const unsigned char *input, size_t input_length,
                               unsigned char *output) {
    int rc = mbedtls_sha1_ret(input, input_length, output);
    if (rc != 0) {
        return 0;
    }
    return 1;
}

/**
 * Initialize a sha1 hmac process
 * @param ctx the context
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_hashing_sha1_init(mbedtls_sha1_context *ctx) {
    mbedtls_sha1_init(ctx);
    return 1;
}

/**
 * Update a sha1 hmac process
 * @param ctx the context
 * @param input the data to add
 * @param input_size the size of input
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_hashing_sha1_update(mbedtls_sha1_context *ctx,
                                      const unsigned char *input,
                                      size_t input_size) {
    mbedtls_sha1_update(ctx, input, input_size);
    return 1;
}

/**
 * finalize a sha1 hmac process
 * @param ctx the context
 * @param hash where to put the results (for sha1, should be 20 bytes long)
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_hashing_sha1_finish(mbedtls_sha1_context *ctx,
                                      unsigned char *hash) {
    mbedtls_sha1_finish(ctx, hash);
    return 1;
}

/**
 * Clean up allocated memory
 * @param ctx the context
 * @returns true(1) on success, otherwise false(0)
 */
int libp2p_crypto_hashing_sha1_free(mbedtls_sha1_context *ctx) {
    mbedtls_sha1_free(ctx);
    return 1;
}
