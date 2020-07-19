/**
 * An implementation of multicodec
 * See https://github.com/multiformats/multicodec
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define MULTICODEC_RAW_BINARY 0x052f6269632f
#define MULTICODEC_ASCII_BASE_2 0x042f62322f

/**
 * Encode a char array using the specified encoding
 * @param codecName a standard name of the codec
 * @param inData the incoming data to be encoded
 * @param inDataLength the length of the inData
 * @param outData a place to store the outgoing data that is at least
 * maxOutDataLength in size
 * @param maxOutDataLength the max size for the output
 * @returns an error code or 0 if all is well
 */
int multicodec_encode(char *codec, char *inData, size_t inDataLength,
                      char *outData, size_t maxOutDataLength);

/**
 * Decode a multicodec encoded data array
 * @param inData the data to be decoded
 * @param inDataLength the length of the data to be encoded
 * @param outData the array to put the results in
 * @param maxOutDataLength the maximum size of the data
 * @returns an error code or 0 if all is well
 */
int multicodec_decode(char *inData, size_t inDataLength, char *outData,
                      size_t maxOutDataLength);


/*!
  * @brief used to check if the provided codec is a valid codec
  * @param codec the codec value to check
  * @returns Success: true
  * @returns Failure: false
*/
bool multicodec_is_valid(char *codec);