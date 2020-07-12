.PHONY: build
build:
	(rm -rf build ; mkdir build ; cd build; cmake .. ; cmake -build . ; make)

.PHONY: static-analysis
static-analysis:
	(rm -rf build ; mkdir build ; cd build; cmake -D CMAKE_BUILD_TYPE=Analyze .. ; cmake -D CMAKE_BUILD_TYPE=Analyze -build . ; make)

.PHONY: clean
clean:
	(./.scripts/clean.sh)