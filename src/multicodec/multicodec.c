#include "multicodec/multicodec.h"
#include "multicodec/codecs.h"
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

int multicodec_encode(char *codec, char *inData, size_t inDataLength,
                      char *outData, size_t maxOutDataLength) {
    return 0;
}

int multicodec_decode(char *inData, size_t inDataLength, char *outData,
                      size_t maxOutDataLength) {
    return 0;
}

/*!
  * @brief used to check if the provided codec is a valid codec
  * @param codec the codec value to check
  * @returns Success: true
  * @returns Failure: false
*/
bool multicodec_is_valid(char *codec) {
    for (int i = 0; i < CODECS_COUNT; i++) {
        if (
            strcmp(codec, CODECS_LIST[i]) == 0
        ) {
            return true;
        }
    }
    return false;
}