/*
 * protobuf.h
 *
 *  Created on: Dec 7, 2016
 *      Author: JohnJones
 */

#ifndef __PROTOBUF_H__
#define __PROTOBUF_H__

#include <stdio.h>
#include <stdint.h>

enum WireType {
	WIRETYPE_VARINT,
	WIRETYPE_64BIT,
	WIRETYPE_LENGTH_DELIMITED,
	WIRETYPE_START_GROUP,
	WIRETYPE_END_GROUP,
	WIRETYPE_32BIT
};

/***
 * Encode a length delimited field into the buffer
 * @param field_number the field number
 * @param wire_type the wire type
 * @param incoming the values
 * @param incoming_length the lenght of incoming
 * @param buffer the pointer to where to place the encoded value
 * @param max_buffer_length the buffer length remaining
 * @param bytes_written the number of bytes written
 * @returns true(1) on success
 */
int protobuf_encode_length_delimited(int field_number, enum WireType wire_type, const char* incoming, size_t incoming_length,
		unsigned char* buffer, size_t max_buffer_size, size_t* bytes_written);

int protobuf_decode_length_delimited(const unsigned char* buffer, size_t buffer_length, char** results, size_t *results_length, size_t* bytes_read);

/***
 * encode a string into the buffer
 * @param field_number the field number
 * @param incoming the string value
 * @param buffer the pointer to where to place the encoded value
 * @param max_buffer_length the buffer length remaining
 * @param bytes_written the number of bytes written
 * @returns true(1) on success
 */
int protobuf_encode_string(int field_number, enum WireType wire_type, const char* incoming, unsigned char* buffer,
		size_t max_buffer_length, size_t* bytes_written);

/**
 * Pull a string from the protobuf buffer
 * @param the buffer, positioned at the field size
 * @param buffer_length the buffer length
 * @param results the results (NOTE: will allocate memory)
 * @param bytes_read the number of bytes read
 * @returns true(1) on success
 */
int protobuf_decode_string(const unsigned char* buffer, size_t buffer_length, char** results, size_t* bytes_read);

/***
 * encode a varint into the buffer
 * @param field_number the field number
 * @param field_type the field type
 * @param incoming the value
 * @param buffer the pointer to where to place the encoded value
 * @param max_buffer_length the buffer length remaining
 * @param bytes_written the number of bytes written
 * @returns true(1) on success
 */
int protobuf_encode_varint(int field_number, enum WireType field_type, unsigned long long incoming, unsigned char* buffer,
		size_t max_buffer_length, size_t* bytes_written);

int protobuf_decode_varint(const unsigned char* buffer, size_t buffer_length, unsigned long long* results, size_t* bytes_read);

/***
 * retrieve field number and field type from current buffer at position 0
 * @param buffer the incoming buffer
 * @param buffer_length the length of the buffer
 * @param field_no the resultant field number
 * @param field_type the field type
 * @param bytes_read the number of bytes read from the buffer
 */
int protobuf_decode_field_and_type(const unsigned char* buffer, int buffer_length, int *field_no, enum WireType *field_type, size_t* bytes_read);


#endif /* PROTOBUF_H_ */
