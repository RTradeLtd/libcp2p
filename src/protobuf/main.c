#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "protobuf.h"

/**
 * opens a file to give a list of fields
 */

size_t os_utils_file_size(const char* path) {
	size_t file_size = 0;
	// open file
	FILE* in_file = fopen(path, "r");
	if (in_file == NULL)
		return 0;
	// determine size
	fseek(in_file, 0L, SEEK_END);
	file_size = ftell(in_file);
	fclose(in_file);
	return file_size;
}

char* getWireType(enum WireType type) {
	switch(type) {
		case(WIRETYPE_VARINT):
			return "Varint";
		case (WIRETYPE_LENGTH_DELIMITED):
			return "Length Delimited";
		case (WIRETYPE_64BIT):
			return "64 Bit";
		case (WIRETYPE_START_GROUP):
			return "Start Group";
		case (WIRETYPE_END_GROUP):
			return "End Group";
		case (WIRETYPE_32BIT):
			return "32 Bit";
	}
	return "Wire Type Invalid";
}

void stripit(int argc, char** argv) {
	char tmp[strlen(argv[argc])];
	strcpy(tmp, &argv[argc][1]);
	tmp[strlen(tmp)-1] = 0;
	strcpy(argv[argc], tmp);
	return;
}

void strip_quotes(int argc, char** argv) {
	for(int i = 0; i < argc; i++) {
		if (argv[i][0] == '\'' && argv[i][strlen(argv[i])-1] == '\'') {
			stripit(i, argv);
		}
	}
}

int main(int argc, char** argv) {
	strip_quotes(argc, argv);
	if (argc <= 1) {
		printf("Syntax: %s [filename] [start_pos]\n", argv[0]);
		return 0;
	}
	char* fileName = argv[1];
	size_t numBytes = os_utils_file_size(fileName);
	printf("Number of bytes in file: %lu\n", numBytes);
	if (numBytes == 0)
		return 0;

	FILE* file = fopen(fileName, "rb");
	unsigned char buffer[numBytes];
	if (fread(buffer, 1, numBytes, file) != numBytes) {
		printf("Unexpected number of bytes.\n");
		return 0;
	}
	//skip ahead
	size_t skip = 0;
	if (argc > 2) {
		skip = atoi(argv[2]);
		if (skip > 0) {
			printf("Skipping first %lu bytes\n", skip);
		}
	}
	int fieldCounter = 0;
	size_t pos = skip;
	while(pos < numBytes) {
		size_t bytes_read = 0;
		int field_no = 0;
		enum WireType wire_type = WIRETYPE_64BIT;
		if (protobuf_decode_field_and_type(&buffer[pos], numBytes - pos, &field_no, &wire_type, &bytes_read) == 0) {
			printf("Unexpected return value from protobuf_decode_field_and_type at pos %lu\n", pos);
			return 0;
		}
		printf("Field counter: %d at position %lu, Field Number: %d, Wire Type: %s", fieldCounter, pos, field_no, getWireType(wire_type));
		pos += bytes_read;
		// read the value
		switch(wire_type) {
			case(WIRETYPE_VARINT): {
				unsigned long long varint = 0;
				protobuf_decode_varint(&buffer[pos], numBytes-pos, &varint, &bytes_read);
				pos += bytes_read;
				printf(" Value: %llu. Next read position at %lu\n", varint, pos);
				break;
			}
			case (WIRETYPE_LENGTH_DELIMITED): {
				unsigned long long varint = 0;
				protobuf_decode_varint(&buffer[pos], numBytes-pos, &varint, &bytes_read);
				pos += bytes_read;
				printf(" Field width: %llu. Next read position at %lu\n", varint, pos);
				pos += varint;
				break;
			}
			default:
				pos++;
		}
		fieldCounter++;
	}
	return 0;
}
