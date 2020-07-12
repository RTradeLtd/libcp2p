#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobuf.h"
#include "varint.h"
#include "Test1_protobuf.h"

enum WireType Test1_message_fields[] = { WIRETYPE_VARINT };

int Test1_new(struct Test1** test1) {
	*test1 = (struct Test1*)malloc(sizeof(struct Test1));
	if (*test1 == NULL)
		return 0;
	(*test1)->a = 0;
	return 1;
}

int Test1_free(struct Test1* test1) {
	free(test1);
	return 1;
}

int Test1_protobuf_encode(struct Test1* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written) {
	protobuf_encode_varint(1, Test1_message_fields[0], incoming->a, buffer, max_buffer_length, bytes_written);
	return 1;
}

int Test1_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test1** output) {
	int pos = 0;
	if (Test1_new(output) == 0)
		return 0;

	while (pos < buffer_length) { // loop through buffer
		size_t bytes_read = 0;
		int field_no;
		enum WireType field_type;
		if (protobuf_decode_field_and_type(buffer, buffer_length, &field_no, &field_type, &bytes_read) == 0) {
			Test1_free(*output);
			return 0;
		}
		pos += bytes_read;
		switch (field_no) {
			case (1):
				(*output)->a = varint_decode(&buffer[pos], buffer_length - pos, &bytes_read);
				pos += (unsigned int)bytes_read;
				break;
		}
	}
	return 1;
}
