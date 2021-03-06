/*! @file protoutils.h
 * @brief utility functions for working with multiaddrs and protocols
 */
#include <stddef.h>
#include <stdint.h>

#include "protocols.h"

#pragma once

// #ifndef PROTOUTILS
// #define PROTOUTILS

//////////////////////////////////////////////////////////
char ASCII2bits(char ch);

void hex2bin(char *dst, char *src, int len);

char bits2ASCII(char b);

void bin2hex(char *dst, char *src, int len);

//////////////////////////////////////////////////////////
// IPv4 VALIDATOR
#define DELIM "."

/* return 1 if string contain only digits, else return 0 */
int valid_digit(char *ip_str);

/* return 1 if IP string is valid, else return 0 */
int is_valid_ipv4(char *ip_str);

//////////////IPv6 Validator
#define MAX_HEX_NUMBER_COUNT 8

int ishexdigit(char ch);

int is_valid_ipv6(char *str);

uint64_t ip2int(const char *ipconvertint);

char *int2ip(int inputintip);

/**
 * @brief Unserialize the bytes into a string
 * @param results where to put the resultant string
 * @param bytes the bytes to unserialize
 * @param bytes_size the length of the bytes array
 */
int bytes_to_string(char *results, const uint8_t *bytes, int bytes_size);

/**
 * @brief Convert an address string to a byte representation
 * @param protocol the protocol to use
 * @param incoming the byte array
 * @param incoming_size the size of the byte array
 * @param results the results
 * @param results_size the size of the results
 * @returns Success: 0
 * @returns Failure: 1
 */
int address_string_to_bytes(struct Protocol *protocol, const char *incoming,
                            size_t incoming_size, char *results, int *results_size);

int string_to_bytes(uint8_t **finalbytes, size_t *realbbsize, const char *strx,
                    size_t strsize);

// #endif
