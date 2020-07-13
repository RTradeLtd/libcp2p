#ifndef base58_h
#define base58_h

#include <stddef.h>
/**
 * convert a base58 encoded string into a binary array
 * @param base58 the base58 encoded string
 * @param base58_size the size of the encoded string
 * @param binary_data the results buffer
 * @param binary_data_size the size of the results buffer
 * @returns true(1) on success
 */
int libp2p_crypto_encoding_base58_decode(const char* b58, size_t base58_size, unsigned char** bin, size_t* binszp);

/**
 * encode an array of bytes into a base58 string
 * @param binary_data the data to be encoded
 * @param binary_data_size the size of the data to be encoded
 * @param base58 the results buffer
 * @param base58_size the size of the results buffer
 * @returns true(1) on success
 */
int libp2p_crypto_encoding_base58_encode(const unsigned char* binary_data, size_t binary_data_size, unsigned char** base58, size_t* base58_size);

/***
 * calculate the size of the results based on an incoming string
 * @param decoded_length the length of the string to be encoded
 * @returns the size in bytes had the string been encoded
 */
size_t libp2p_crypto_encoding_base58_encode_size(size_t decoded_length);

/**
 * calculate the max length in bytes of an encoding of n source bytes
 * @param encoded_length the length of the string to be decoded
 * @returns the maximum size in bytes had the string been decoded
 */
size_t libp2p_crypto_encoding_base58_decode_size(size_t encoded_length);


#endif /* base58_h */
