/***
 * Helper to derived protobuf objects
 */
#include <stdint.h>
#include <string.h>

#include "varint.h"
#include "protobuf.h"

int protobuf_encode_length_delimited(int field_number, enum WireType field_type, const char* incoming, size_t incoming_length,
		unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written)
{
	// push the field number and wire type together
	unsigned int field_no = field_number << 3;
	unsigned long long field = field_no | field_type;
	size_t bytes_processed = 0;
	*bytes_written = 0;
	// field type & number
	varint_encode(field, buffer, max_buffer_length, &bytes_processed);
	*bytes_written += bytes_processed;
	// field size
	varint_encode(incoming_length, &buffer[*bytes_written], max_buffer_length - *bytes_written, &bytes_processed);
	*bytes_written += bytes_processed;
	// field value
	if (incoming_length > 0) {
		memcpy(&buffer[*bytes_written], incoming, incoming_length);
		*bytes_written += incoming_length;
	}
	return 1;
}

int protobuf_decode_length_delimited(const unsigned char* buffer, size_t buffer_length, char** results, size_t *results_length, size_t* bytes_read) {
	size_t pos = 0;
	*bytes_read = 0;
	// grab the field size
	size_t field_size = varint_decode(&buffer[pos], buffer_length - pos, bytes_read);
	pos += *bytes_read;

	// allocate memory
	*results = malloc(sizeof(char) * field_size);
	if ((*results) == NULL)
		return 0;

	memset(*results, 0, field_size);

	// copy the bytes
	memcpy( *results, &buffer[pos], field_size);
	pos += field_size;

	// set return values
	*bytes_read = pos;
	*results_length = field_size;

	return 1;
}


/***
 * encode a string into the buffer
 * @param field_number the field number
 * @param field_type the field type
 * @param incoming the string value
 * @param buffer the pointer to where to place the encoded value
 * @param max_buffer_length the buffer length remaining
 * @param bytes_written the number of bytes written
 * @returns true(1) on success
 */
int protobuf_encode_string(int field_number, enum WireType field_type, const char* incoming, unsigned char* buffer,
		size_t max_buffer_length, size_t* bytes_written) {
	return protobuf_encode_length_delimited(field_number, field_type, incoming, strlen(incoming), buffer, max_buffer_length, bytes_written);
}

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
		size_t max_buffer_length, size_t* bytes_written) {
	*bytes_written = 0;
	// push the field number and wire type together
	unsigned int field_no = field_number << 3;
	unsigned long long field = field_no | field_type;
	size_t bytes_processed;
	// field type & number
	varint_encode(field, buffer, max_buffer_length, &bytes_processed);
	*bytes_written += bytes_processed;
	// field value
	varint_encode(incoming, &buffer[*bytes_written], max_buffer_length - *bytes_written, &bytes_processed);
	*bytes_written += bytes_processed;
	return 1;

}

int protobuf_decode_varint(const unsigned char* buffer, size_t buffer_length, unsigned long long* results, size_t* bytes_read) {
	*results = varint_decode(buffer, buffer_length, bytes_read);
	return 1;
}

/**
 * Pull a string from the protobuf buffer
 * @param the buffer, positioned at the field size
 * @param buffer_length the buffer length
 * @param results the results (NOTE: will allocate memory)
 * @param bytes_read the number of bytes read
 * @returns true(1) on success
 */
int protobuf_decode_string(const unsigned char* buffer, size_t buffer_length, char** results, size_t* bytes_read) {
	size_t pos = 0;
	*bytes_read = 0;
	// grab the field size
	int length = varint_decode(&buffer[pos], buffer_length - pos, bytes_read);
	pos += *bytes_read;
	*bytes_read += length;

	// allocate memory (if neccesary)
	if (length > 0) {
		*results = malloc(sizeof(char) * length + 1);
		if ((*results) == NULL)
			return 0;

		memset(*results, 0, length+1);

		// copy the string
		memcpy((*results), &buffer[pos], length);
		// don't forget the null
		(*results)[length] = 0;
	}
	return 1;
}

/***
 * retrieve field number and field type from current buffer at position 0
 * @param buffer the incoming buffer
 * @param buffer_length the length of the buffer
 * @param field_no the resultant field number
 * @param field_type the field type
 * @param bytes_read the number of bytes read from the buffer
 */
int protobuf_decode_field_and_type(const unsigned char* buffer, int buffer_length, int *field_no, enum WireType *field_type, size_t* bytes_read) {
	*bytes_read = 0;
	unsigned long long field = varint_decode(buffer, buffer_length, bytes_read);
	*field_no = field >> 3;
	*field_type = field - (*field_no << 3);
	return 1;
}



