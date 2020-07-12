/*
 * Test3_protobuf.h
 */

#pragma once

#include "protobuf.h"

#include "Test2_protobuf.h"

struct Test3 {
	char* a_string;
	int an_int;
	struct Test2* test2;
};

int Test3_new(struct Test3** test1);
int Test3_free(struct Test3* test1);

int Test3_protobuf_encode(struct Test3* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written);
int Test3_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test3** output);

