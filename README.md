# libcp2p

> **this is a work in progress**

A libp2p-like protocol written in C with first class support for embedded systems. Goal is to be able to run in your fridge, arduinos, and any embedded system. Higher level languages will be done using FFI.

# difference between libcp2p and libp2p

* Written in C
* Uses mbedtls 
* Removal of built-in identify protocol
  * LibP2P is heavily reliant on the identify protocol and it is not possible to remove it
  * Redesigned so that identify is optional
* Protobuf replaced with cbor
* first class support for embedded systems
* higher level languages will use FFI instead of a language specific implementation

# features

## multiformats

* multicodec
  * full encode/decode support for all codecs except 0xcert-imprint-256
* multiaddr
  * full multiaddr functionality
* multihash
  * basic multihash functionality, only supports SHAX-XXX, and Blake2B
* multibase
  * supports base16, base32, base58, base64
  * note: base58 encoding/decoding is partially broken

## misc

* cid
  * basic support only
* protobuf support
  * basic support only
  * this will eventually be removed in favor of CBOR
* cryptography
  * ecdsa key management
  * rsa key management
  * sha1, sha256, sha512 hashing
  * misc other crypto functionality

# installation

## building

```shell
$> make # builds in release mode
$> make build-debug # builds in debug mode
$> make build-analysis # builds in static analysis mode
```

## dependencies

* CMake >= 3.0
* GCC with support for C17
  * If you want to run static analysis, GCC-10 installed
* CMocka (testing dependency)
* Valgrind (dynamic analysis)
* clang-format (code formatting)
* doxygen (documentation generation)
* pthreads

## compiling mbedtls

```shell
$> make -j6 CFLAGS="-fPIC" # reduce -jX to suit your environment appropriately
$> sudo make install CFLAGS="-fPIC"
$> sudo ldconfig
```

## testing

```shell
$> cd build && ctest # runs regular tests
$> cd build && ctest -T memcheck # runs valgrind 
```

# development

* see [DEVELOPMENT.md](./DEVELOPMENT.md)

# license

Certain libraries are copy&pasted from other open sources repositories are explicitly licensed under MIT or GPLv3. The code written for this lbirary specifically is licensed under AGPLv3. [According to this stack overflow post](https://opensource.stackexchange.com/questions/5909/is-every-license-that-is-gplv3-compatible-also-agplv3-compatible/6785#6785) this is acceptable. If you think this is not acceptable please open a github issue and we will make ammends