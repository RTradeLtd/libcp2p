.PHONY: build
build:
	(rm -rf build ; mkdir build ; cd build; cmake .. ; cmake -build . ; make)

.PHONY: build-debug
build-debug:
	(rm -rf build ; mkdir build ; cd build; cmake -D CMAKE_BUILD_TYPE=Debug .. ; cmake -D CMAKE_BUILD_TYPE=Debug -build . ; make)

.PHONY: static-analysis
static-analysis:
	(rm -rf build ; mkdir build ; cd build; cmake -D CMAKE_BUILD_TYPE=Analyze .. ; cmake -D CMAKE_BUILD_TYPE=Analyze -build . ; make)

.PHONY: docs
docs:
	(cd build; cmake --build . --target doxygen-docs)


.PHONY: clean
clean:
	(./.scripts/clean.sh)

.PHONY: format
format:
	find . -type f -name "*.c" -not -name "*_test.c" -exec clang-format -i {} \;
	find . -type f -name "*.h" -exec clang-format -i {} \;

