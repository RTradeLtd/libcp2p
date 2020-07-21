.PHONY: build
build:
	(./.scripts/build.sh)

.PHONY: build-ci
build-ci:
	(./.scripts/build.sh ci)

.PHONY: build-debug
build-debug:
	(./.scripts/build.sh debug)

.PHONY: build-analysis
build-analysis:
	(./.scripts/build.sh analyze)

.PHONY: docs
docs:
	(cd build; cmake --build . --target doxygen-docs)

.PHONY: install-cmocka
install-cmocka:
	(./.scripts/install_cmocka.sh)

.PHONY: install-mbedtls
install-mbedtls:
	(./.scripts/install_mbedtls.sh)

.PHONY: install-deps-ubuntu
install-deps-ubuntu: deps-ubuntu install-mbedtls

.PHONY: deps-ubuntu
deps-ubuntu:
	sudo apt install doxygen -y
	sudo apt install libcmocka0 libcmocka-dev -y

.PHONY: run-valgrind
run-valgrind:
	(./.scripts/valgrind.sh)

.PHONY: clean
clean:
	(./.scripts/clean.sh)

.PHONY: format
format:
	find . -type f -name "*.c" -not -name "*_test.c" -exec clang-format -i {} \;
	find . -type f -name "*.h" -exec clang-format -i {} \;

