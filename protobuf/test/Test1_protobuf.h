#pragma once

#include "protobuf.h"

struct Test1 {
	int a;
};

int Test1_new(struct Test1** test1);
int Test1_free(struct Test1* test1);

int Test1_protobuf_encode(struct Test1* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written);
int Test1_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test1** output);
