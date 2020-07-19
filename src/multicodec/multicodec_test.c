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
// limitations under the Licens

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "multicodec/codecs.h"
#include "multicodec/multicodec.h"
#include <stdbool.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

void test_valide_codecs(void **state) {
    for (int i = 0; i < CODECS_COUNT; i++) {
        bool is_valid = multicodec_is_valid((char *)CODECS_LIST[i]);
        assert(is_valid == true);
    }
    // test a random string to ensure its not a codec
    bool is_valid = multicodec_is_valid("this is not a real codec");
    assert(is_valid == false);
}

void test_multicodec_encode_decode(void **state) {
    for (int i = 0; i < CODECS_COUNT; i++) {
        const char *codec = CODECS_LIST[i];
        char *in_data = "hello world";
        size_t in_data_len = 12;
        char *out_data = calloc(
            sizeof(char),
            in_data_len + strlen(codec)
        );
        size_t max_out_data_len = sizeof(char) * (in_data_len + strlen(codec));
        char want_out[max_out_data_len];
        memset(want_out, 0, max_out_data_len);
        sprintf(want_out, "%s%s", codec, in_data);

        multicodec_encoded_t *encoded = multicodec_encode(
            (char *)codec,
            in_data,
            in_data_len,
            out_data,
            max_out_data_len
        );
        assert(encoded != NULL);
        assert(
            strcmp(
                encoded->data,
                want_out
            ) == 0
        );

        char *decoded_out = calloc(sizeof(char), 12); 
        int rc = multicodec_decode(
            encoded,
            decoded_out,
            12
        );
        assert(rc == 0);
        assert(
            strcmp(
                decoded_out,
                in_data
            ) == 0
        );
        multicodec_free_encoded(encoded);
        free(decoded_out);
    }
    multicodec_encoded_t *enc = multicodec_encode("not a real codec", NULL, 0, NULL, 0);
    assert(enc == NULL);
    multicodec_encoded_t test_enc = {.codec = "not a real codec"};
    int rc = multicodec_decode(&test_enc, NULL, 0);
    assert(rc == 1);

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_valide_codecs),
        cmocka_unit_test(test_multicodec_encode_decode)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}