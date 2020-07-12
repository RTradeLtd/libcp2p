.PHONY: build
build:
	(rm -rf build ; mkdir build ; cd build; cmake .. ; cmake -build . ; make)

.PHONY: static-analysis
static-analysis:
	(rm -rf build ; mkdir build ; cd build; cmake -D CMAKE_BUILD_TYPE=Analyze .. ; cmake -D CMAKE_BUILD_TYPE=Analyze -build . ; make)

.PHONY: clean
clean:
	(./.scripts/clean.sh)

.PHONY: format
format:
	find . -type f -name "*.c" -exec clang-format -i {} \;
	find . -type f -name "*.h" -exec clang-format -i {} \;