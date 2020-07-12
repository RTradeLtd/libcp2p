#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobuf.h"
#include "varint.h"
#include "Test2_protobuf.h"

enum WireType Test2_message_fields[] = { WIRETYPE_LENGTH_DELIMITED };

/**
 * Allocate resources for this structure
 * @param test2 the structure
 * @returns true(1) on success
 */
int Test2_new(struct Test2** test2) {
	*test2 = (struct Test2*)malloc(sizeof(struct Test2));
	if (*test2 == NULL)
		return 0;
	(*test2)->a = NULL;

	return 1;
}

/**
 * Free resources
 * @param test2 the struct
 * @returns true(1) on success
 */
int Test2_free(struct Test2* test2) {
	if (test2->a != NULL)
		free(test2->a);
	free(test2);
	return 1;
}

/***
 * Encode struct into protobuf stream
 * @param incoming the struct
 * @param buffer the place to put the stream
 * @param max_buffer_length the length of the buffer
 * @param bytes_written the number of bytes used in encoding the stream
 * @returns true(1) on success
 */
int Test2_protobuf_encode(struct Test2* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written) {
	if (protobuf_encode_string(1, Test2_message_fields[0], incoming->a, buffer, max_buffer_length, bytes_written) == 0)
		return 0;
	return 1;
}

/***
 * Decode a buffer into a struct
 * @param buffer the incoming buffer
 * @param buffer_length the length of the buffer
 * @param output the resultant struct
 * @returns true(1) on success
 */
int Test2_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test2** output) {
	int pos = 0;
	if (Test2_new(output) == 0)
		return 0;

	while (pos < buffer_length) { // loop through buffer
		size_t bytes_read;
		int field_no;
		enum WireType field_type;
		protobuf_decode_field_and_type(&buffer[pos], buffer_length - pos, &field_no, &field_type, &bytes_read);
		pos += bytes_read;
		bytes_read = 0;
		switch(field_no) {
			case 1:
				protobuf_decode_string(&buffer[pos], buffer_length - pos, &((*output)->a), &bytes_read);
				break;
		}
		pos += bytes_read;

	}
	return 1;
}

