#include <stdio.h>
#include <string.h>

#include "test_protobuf.h"
#include "test_varint.h"
 		 
int testit(const char* name, int (*func)(void)) {
	printf("Testing %s...\n", name);
	int retVal = func();
	if (retVal)
		printf("%s success!\n", name);
	else
		printf("** Uh oh! %s failed.**\n", name);
	return retVal == 0;
}

const char* names[] = {
		"test_write_simple",
		"test_write_negative",
		"test_write_string",
		"test_rightshift",
		"test_complex_protobuf",
		"test_varint"
};

int (*funcs[])(void) = {
		test_write_simple,
		test_write_negative,
		test_write_string,
		test_rightshift,
		test_complex_protobuf,
		test_varint
};

/**
 * run 1 test or run all
 */
int main(int argc, char** argv) {
	int counter = 0;
	int tests_ran = 0;
	char* test_wanted;
	int only_one = 0;
	if(argc > 1) {
		only_one = 1;
		if (argv[1][0] == '\'') { // some shells put quotes around arguments
			argv[1][strlen(argv[1])-1] = 0;
			test_wanted = &(argv[1][1]);
		}
		else
			test_wanted = argv[1];
	}
	int array_length = sizeof(funcs) / sizeof(funcs[0]);
	for (int i = 0; i < array_length; i++) {
		if (only_one && strcmp(names[i], test_wanted) == 0) {
			tests_ran++;
			counter += testit(names[i], funcs[i]);
		}
		else
			if (!only_one) {
				tests_ran++;
				counter += testit(names[i], funcs[i]);
			}

	}

	if (tests_ran == 0)
		printf("***** No tests found *****\n");
	else {
		if (counter > 0) {
			printf("***** There were %d failed test(s) *****\n", counter);
		} else {
			printf("All %d tests passed\n", tests_ran);
		}
	}
	return 1;
}
