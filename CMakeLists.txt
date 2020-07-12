cmake_minimum_required(VERSION 3.15)
project(libp2p C CXX)
enable_language(C ASM)

# handle debug build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # -g indicates to include debug symbols
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g)
endif()

# handle no build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    # disable debug stuff, useful for disabling asserts
    add_definitions(-DNDEBUG)
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic)
endif()

list(APPEND flags -D_POSIX_C_SOURCE=201112L)

add_library(libvarint
    SHARED
        ./protobuf/varint.h
        ./protobuf/varint.c
)

add_library(libprotobuf
    SHARED
        ./protobuf/protobuf.h
        ./protobuf/protobuf.c
)

target_compile_options(libvarint PRIVATE ${flags})
target_compile_options(libprotobuf PRIVATE ${flags})


# add source files
# file(GLOB_RECURSE LIBP2P_SOURCES
#
# )