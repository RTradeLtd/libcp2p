#include "varint.h"

int test_varint() {

	unsigned long long ull = 150;
	size_t buffer_size = 256;
	unsigned char buffer[buffer_size];

	memset(buffer, 0, 256);

	size_t results;
	varint_encode(ull, buffer, buffer_size, &results);

	unsigned int pos0 = (unsigned int)buffer[0];
	unsigned int pos1 = (unsigned int)buffer[1];

	if ( pos0 != 150) {
		printf("Expected %d but got %d at position 0\n", 150, pos0);
		return 0;
	}

	if (pos1 != 1) {
		printf("Expected %d but got %d at position 1\n", 2, pos1);
		return 0;
	}

	// now do the reverse

	unsigned long long newVal = varint_decode(buffer, (int)results, &results);

	if (newVal != ull) {
		printf("Expected %llu but received %llu\n", ull, newVal);
		return 0;
	}

	return 1;
}
