# libcp2p

[![Build Status](https://travis-ci.com/RTradeLtd/libcp2p.svg?branch=master)](https://travis-ci.com/RTradeLtd/libcp2p)

> **this is a work in progress**

A libp2p-like protocol written in C with first class support for embedded systems. Goal is to be able to run in your fridge, arduinos, and any embedded system. Higher level languages will be done using FFI.

# difference between libcp2p and libp2p

* Written in C
* Base58 replaced with base32
* Removal of built-in identify protocol
  * LibP2P is heavily reliant on the identify protocol and it is not possible to remove it
  * Redesigned so that identify is optional
* Protobuf replaced with cbor
* first class support for embedded systems
* higher level languages will use FFI instead of a language specific implementation

# compatability with libp2p

* in theory there will be network level compatability
  * a libcp2p node would be able to open a connection to a libp2p node
  * this is in no way a design goal, nor a priority
* dht will not be compatible

# features

## multiformats

* multicodec
  * full encode/decode support for all codecs except 0xcert-imprint-256
* multiaddr
  * full multiaddr functionality
* multihash
  * basic multihash functionality, only supports SHAX-XXX, and Blake2B
* multibase
  * supports base16, base32, base64
  
## peerstore

* Adds a peerstore to contain public key and peer identifier information
* Eventually will contain addressing information

## networking

* TCP socket server
  * UDP has been tabled until we get a stable implementation of TCP
* Uses the multiaddress standard as the method of addressing other hosts
* Allows specifying two separate execution handlers whenever a new TCP connection is received
* todo:
  * enable secure connections
  * enable using peerIds

## misc

* cid
  * basic support only
* protobuf support
  * basic support only
  * this will eventually be removed in favor of CBOR
* cryptography
  * ecdsa key management
  * sha1, sha256, sha512 hashing
  * misc other crypto functionality

# installation

## building

```shell
$> make # builds in release mode
$> make build-debug # builds in debug mode
$> make build-analysis # builds in static analysis mode
```
## testing

```shell
$> cd build && ctest # runs regular tests
$> cd build && ctest -T memcheck # runs valgrind 
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
* tinycbor
  * in repo using [this commit](https://github.com/intel/tinycbor/commit/085ca40781f7c39febe6d14fb7e5cba342e1804b)


# development/contribution

For instructions on development and contribution practices please see [CONTRIBUTING.md](CONTRIBUTING.md)

# license

Certain libraries are copy&pasted from other open sources repositories are explicitly licensed under MIT or GPLv3. The code written for this lbirary specifically is licensed under AGPLv3. [According to this stack overflow post](https://opensource.stackexchange.com/questions/5909/is-every-license-that-is-gplv3-compatible-also-agplv3-compatible/6785#6785) this is acceptable. If you think this is not acceptable please open a github issue and we will make ammends