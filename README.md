# libcp2p

A libp2p-like protocol written in C and designed to support embedded systems. High level language bindings will use FFI. Heavy WIP

# difference between libcp2p and libp2p

* Written in C
* Uses mbedtls 
* Removal of built-in identify protocol
  * LibP2P is heavily reliant on the identify protocol and it is not possible to remove it
  * Redesigned so that identify is optional
* Maybe other things

# roadmap

* Get basic multiformats working in C
  * Multiaddr
  * Multihash
  * Protobuf
* Get basic networking done
* ....

# installation

## dependencies

* CMake >= 3.0
* GCC with support for C17
  * If you want to run static analysis, GCC-10 installed
* CMocka (testing dependency)
* Valgrind (dynamic analysis)
* clang-format (code formatting)
* doxygen (documentation generation)
* pthreads

# development

* Code submitted in PRs must be formatted using `make format`
* All code must be tested with CMocka
* All code must be documented with doxygen compatible comments
* All code must be tested with valgrind


