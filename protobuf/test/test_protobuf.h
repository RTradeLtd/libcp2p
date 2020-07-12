
/***
 * This...
 *
 * message Test1 {
 *  required int32 a = 1;
 * }
 *
 * should generate Test1_protobuf.h and c that contains this...
 *
 * enum WireType Test1_message_fields[] = { WIRETYPE_VARINT }
 *
 * struct Test1 {
 * 	int a;
 * }
 *
 * int Test1_protobuf_encode(struct Test1* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written);
 * int Test1_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Test1** output);
 */
#include <stdlib.h>
#include <string.h>

#include "Test3_protobuf.h"
#include "Test1_protobuf.h"
#include "Test2_protobuf.h"

int test_complex_protobuf() {
	int buffer_length = 255;
	size_t bytes_used = 0;
	unsigned char buffer[buffer_length];

	// build the objects
	struct Test2* test2;
	Test2_new(&test2);
	struct Test3* test3;
	Test3_new(&test3);
	test3->test2 = test2;

	// set values
	test3->a_string = malloc(15);
	strcpy(test3->a_string, "Hello, World!");
	test3->an_int = 29;
	test3->test2->a = malloc(10);
	strcpy(test3->test2->a, "Testing");

	// encode
	Test3_protobuf_encode(test3, buffer, buffer_length, &bytes_used);

	// decode
	struct Test3* results;
	Test3_protobuf_decode(buffer, bytes_used, &results);

	// view results
	if (strcmp(results->a_string, test3->a_string) != 0) {
		printf("String a does not match: %s vs %s\n", test3->a_string, results->a_string);
		Test3_free(test3);
		Test3_free(results);
		return 0;
	}

	if (results->an_int != test3->an_int) {
		printf("Integers do not match: %d vs %d\n", test3->an_int, results->an_int);
		Test3_free(test3);
		Test3_free(results);
		return 0;
	}

	if (strcmp(results->test2->a, test3->test2->a) != 0) {
		printf("String a does not match: %s vs %s\n", test3->test2->a, results->test2->a);
		Test3_free(test3);
		Test3_free(results);
		return 0;
	}

	Test3_free(test3);
	Test3_free(results);
	return 1;

}

int test_rightshift() {
	struct Test1 test1;
	test1.a = 0;

	size_t buffer_size = 256;
	unsigned char buffer[buffer_size];
	memset(buffer, 0, 256);

	int retVal = 0;
	size_t bytes_written = 0;
	retVal = Test1_protobuf_encode(&test1, buffer, buffer_size, &bytes_written);

	if (retVal == 0) {
		printf("Error encoding\n");
		return 0;
	}

	if (bytes_written != 2) {
		printf("Incorrect number of bytes written. Expected 2, but got %d\n", retVal);
		return 0;
	}

	unsigned int pos0 = buffer[0];
	unsigned int pos1 = buffer[1];

	if (pos0 != 8) {
		printf("Results at pos 0 not as expected\n");
		return 0;
	}

	if (pos1 != 0) {
		printf("Results at pos 1 not as expected\n");
		return 0;
	}

	return 1;
}

int test_write_simple() {
	struct Test1 test1;
	test1.a = 150;

	size_t buffer_size = 256;
	unsigned char buffer[buffer_size];
	memset(buffer, 0, 256);

	int retVal = 0;
	size_t bytes_written = 0;
	retVal = Test1_protobuf_encode(&test1, buffer, buffer_size, &bytes_written);

	if (retVal == 0) {
		printf("Error encoding\n");
		return 0;
	}

	if (bytes_written != 3) {
		printf("Incorrect number of bytes written. Expected 3, but got %d\n", retVal);
		return 0;
	}

	unsigned int pos0 = buffer[0];
	unsigned int pos1 = buffer[1];
	unsigned int pos2 = buffer[2];

	if (pos0 != 8 || pos1 != 150 || pos2 != 1) {
		printf("Results not as expected\n");
		return 0;
	}

	struct Test1* results;
	retVal = Test1_protobuf_decode(buffer, bytes_written, &results);

	if (results->a != test1.a) {
		printf("Incorrect results after decode");
		Test1_free(results);
		return 0;
	}

	Test1_free(results);

	return 1;
}

int test_write_negative() {
	struct Test1 test1;
	test1.a = -150;

	size_t buffer_size = 256;
	unsigned char buffer[buffer_size];
	memset(buffer, 0, 256);

	int retVal = 0;
	size_t bytes_written = 0;
	retVal = Test1_protobuf_encode(&test1, buffer, buffer_size, &bytes_written);

	if (retVal == 0) {
		printf("Error encoding\n");
		return 0;
	}

	if (bytes_written != 11) {
		printf("Incorrect number of bytes written. Expected 11, but got %d\n", retVal);
		return 0;
	}

	unsigned int pos0 = buffer[0];
	unsigned int pos1 = buffer[1];
	unsigned int pos2 = buffer[2];
	unsigned int pos3 = buffer[3];
	unsigned int pos4 = buffer[4];
	unsigned int pos5 = buffer[5];
	unsigned int pos6 = buffer[6];
	unsigned int pos7 = buffer[7];
	unsigned int pos8 = buffer[8];
	unsigned int pos9 = buffer[9];
	unsigned int pos10 = buffer[10];

	if (pos0 != 8 || pos1 != 234 || pos2 != 254
			|| pos3 != 255 || pos4 != 255 || pos5 != 255 || pos6 != 255
			|| pos7 != 255 || pos8 != 255 || pos9 != 255 || pos10 != 1
			) {
		printf("Results not as expected\n");
		return 0;
	}

	return 1;
}

int test_write_string() {
	struct Test2 test2;
	test2.a = "testing";

	size_t buffer_size = 256;
	unsigned char buffer[buffer_size];
	memset(buffer, 0, 256);

	int retVal = 0;
	size_t bytes_written = 0;

	retVal = Test2_protobuf_encode(&test2, buffer, buffer_size, &bytes_written);

	if (retVal == 0) {
		printf("Error encoding\n");
		return 0;
	}

	if (bytes_written != 9) {
		printf("Incorrect number of bytes written. Expected 9, but got %d\n", retVal);
		return 0;
	}

	unsigned char expected[] = { 10, 7, 't', 'e', 's', 't', 'i', 'n', 'g' };

	int correct = 1;
	for(int i = 0; i < 9; i++) {
		if (expected[i] != buffer[i]) {
			correct = 0;
			printf("Incorrect value at position %d... Expected %02x but received %02x.\n", i, expected[i], buffer[i]);
		}
	}
	if (!correct)
		return 0;

	struct Test2* test_results;
	Test2_protobuf_decode(buffer, bytes_written, &test_results);
	if (strcmp(test2.a, test_results->a) != 0) {
		printf("Strings do not match. %s vs %s", test2.a, test_results->a);
		Test2_free(test_results);
		return 0;
	}

	Test2_free(test_results);

	return 1;
}
