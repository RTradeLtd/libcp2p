cmake_minimum_required(VERSION 3.15)
project(libcp2p LANGUAGES C)
set(CMAKE_C_COMPILER gcc)

# handle debug build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # -g indicates to include debug symbols
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g -fanalyzer)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Analyze")
    # override gcc compiler to v10 for static analysis
    set(CMAKE_C_COMPILER /usr/bin/gcc-10)
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g -fanalyzer)
    message(STATUS "enabling static analysis")
endif()

# handle no build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    # disable debug stuff, useful for disabling asserts
    add_definitions(-DNDEBUG)
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic)
endif()


list(APPEND flags -D_POSIX_C_SOURCE=201112L)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(libmultiaddr
    SHARED
        ./include/multiaddr/base58.h
        ./include/multiaddr/endian.h
        ./include/multiaddr/multiaddr.h
        ./include/multiaddr/protocols.h
        ./include/multiaddr/protoutils.h
        ./include/multiaddr/varhexutils.h
        ./include/multiaddr/varint.h
        ./multiaddr/base58.c
        ./multiaddr/multiaddr.c
        ./multiaddr/protocols.c
        ./multiaddr/testing.c
        ./multiaddr/varhexutils.c
        ./multiaddr/varint.c
)
target_compile_options(libmultiaddr PRIVATE ${flags})

add_library(libmultihash
    SHARED
        ./include/multihash/errors.h
        ./include/multihash/generic.h
        ./include/multihash/hashes.h
        ./include/multihash/multihash.h
        ./multihash/errors.c
        ./multihash/hashes.c
        ./multihash/multihash.c
)
target_compile_options(libmultihash PRIVATE ${flags})


add_library(libvarint
    SHARED
        ./protobuf/varint.h
        ./protobuf/varint.c
)
target_compile_options(libvarint PRIVATE ${flags})

add_library(libprotobuf
    SHARED
        ./protobuf/protobuf.h
        ./protobuf/protobuf.c
)
target_compile_options(libprotobuf PRIVATE ${flags})
target_link_libraries(libprotobuf libvarint)

add_executable(libprotobuf-cli ./protobuf/main.c)
target_link_libraries(libprotobuf-cli libvarint)
target_link_libraries(libprotobuf-cli libprotobuf)
target_compile_options(libprotobuf-cli PRIVATE ${flags})

# add source files
# file(GLOB_RECURSE LIBP2P_SOURCES
#
# )