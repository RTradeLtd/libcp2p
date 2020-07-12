#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobuf.h"
#include "varint.h"
#include "Test3_protobuf.h"

enum WireType Test3_message_fields[] = { WIRETYPE_LENGTH_DELIMITED, WIRETYPE_VARINT, WIRETYPE_LENGTH_DELIMITED };

int Test3_new(struct Test3** test3) {
	*test3 = (struct Test3*)malloc(sizeof(struct Test3));
	if (*test3 == NULL)
		return 0;
	(*test3)->a_string = NULL;
	(*test3)->an_int = 0;
	(*test3)->test2 = NULL;
	return 1;
}

int Test3_free(struct Test3* test3) {
	if (test3->a_string != NULL)
		free(test3->a_string);
	if (test3->test2 != NULL)
		Test2_free(test3->test2);
	free(test3);
	return 1;
}

int Test3_protobuf_encode(struct Test3* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written) {
	size_t bytes_used;
	*bytes_written = 0;
	protobuf_encode_string(1, Test3_message_fields[0], incoming->a_string, buffer, max_buffer_length, &bytes_used);
	*bytes_written += bytes_used;
	protobuf_encode_varint(2, Test3_message_fields[1], incoming->an_int, &buffer[*bytes_written], max_buffer_length - *bytes_written, &bytes_used);
	*bytes_written += bytes_used;
	// write Test2 to safe area
	int pos = *bytes_written + 11;
	size_t test2_size = 0;
	int retVal = Test2_protobuf_encode(incoming->test2, &buffer[pos], max_buffer_length - pos, &test2_size);
	protobuf_encode_varint(3, Test3_message_fields[2], test2_size, &buffer[*bytes_written], max_buffer_length - *bytes_written, &bytes_used);
	*bytes_written += bytes_used;
	// now move the encoded Test2 to the correct area
	memcpy(&buffer[*bytes_written], &buffer[pos], test2_size);
	*bytes_written += test2_size;
	return retVal;
}

int Test3_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test3** output) {
	size_t pos = 0;
	if (Test3_new(output) == 0)
		return 0;

	while (pos < buffer_length) { // loop through buffer
		size_t bytes_read = 0;
		int field_no;
		enum WireType field_type;
		if (protobuf_decode_field_and_type(&buffer[pos], buffer_length, &field_no, &field_type, &bytes_read) == 0) {
			Test3_free(*output);
			return 0;
		}
		pos += bytes_read;
		switch (field_no) {
			case(1):
				protobuf_decode_string(&buffer[pos], buffer_length - pos, &((*output)->a_string), &bytes_read);
				pos += bytes_read;
				break;
			case (2):
				(*output)->an_int = varint_decode(&buffer[pos], buffer_length - pos, &bytes_read);
				pos += bytes_read;
				break;
			case(3): {
				size_t test2_size = varint_decode(&buffer[pos], buffer_length - pos, &bytes_read);
				pos += bytes_read;
				Test2_protobuf_decode(&buffer[pos], buffer_length - pos, &((*output)->test2));
				pos += test2_size;
				break;
			}
		}
	}
	return 1;
}
