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

/*! @file multicodec.h
 * @author Bonedaddy
 * @brief an implemention of multicodec
 * @details enables self describing data encoding methods, for the actual codec
 * definitions see codecs.h
 * @note for more information on multicodecs see
 * https://github.com/multiformats/multicodec
 */

#pragma once

#include <stdbool.h>
#include <stdlib.h>

#define MULTICODEC_RAW_BINARY 0x052f6269632f
#define MULTICODEC_ASCII_BASE_2 0x042f62322f

/*!
 * @struct multicodec_encoded
 * @typedef multicodec_encoded_t
 * @brief holds a multicodec encoded data primarily used for easy multicodec decoded
 */
typedef struct multicodec_encoded {
    char *codec; /*! @brief the codec used for encoding */
    char *data;  /*! @brief the encoded data prefixed with codec */
} multicodec_encoded_t;

/*! @brief free up resources allocated for multicodec_encoded_t
 * @param encoded the isntance of multicodec_encoded_t we are freeing
 */
void multicodec_free_encoded(multicodec_encoded_t *encoded);

/**
 * @brief Encode a char array using the specified encoding
 * @param codecName a standard name of the codec
 * @param inData the incoming data to be encoded
 * @param inDataLength the length of the inData
 * @param outData a place to store the outgoing data that is at least
 * maxOutDataLength in size
 * @param maxOutDataLength the max size for the output
 * @returns Success: pointer to an instance of multicodec_encoded_t
 * @returns Failure: NULL pointer
 */
multicodec_encoded_t *multicodec_encode(char *codec, char *inData,
                                        size_t inDataLength, char *outData,
                                        size_t maxOutDataLength);

/**
 * @brief Decode a multicodec encoded data array
 * @param encoded an instance of multicodec_encoded_t holding the encoded data and
 * codec used
 * @param outData the array to put the results in
 * @param maxOutDataLength the maximum size of the data
 * @returns an error code or 0 if all is well
 */
int multicodec_decode(multicodec_encoded_t *encoded, char *outData,
                      size_t maxOutDataLength);

/*!
 * @brief used to check if the provided codec is a valid codec
 * @param codec the codec value to check
 * @returns Success: true
 * @returns Failure: false
 */
bool multicodec_is_valid(char *codec);