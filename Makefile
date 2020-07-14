.PHONY: build
build:
	(./.scripts/build.sh)

.PHONY: build-debug
build-debug:
	(./.scripts/build.sh debug)

.PHONY: build-analysis
build-analysis:
	(./.scripts/build.sh analyze)

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

