#pragma once

#include "protobuf.h"

struct Test2 {
	char* a;
};

int Test2_protobuf_encode(struct Test2* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written);
int Test2_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test2** output);
int Test2_free(struct Test2* test2);
int Test2_new(struct Test2** test2);
