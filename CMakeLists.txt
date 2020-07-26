#####################
# START CMAKE SETUP #
#####################

cmake_minimum_required(VERSION 3.0)
project(libcp2p LANGUAGES C)
set(CMAKE_C_COMPILER /usr/bin/gcc)


# handle debug build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # -g indicates to include debug symbols
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Analyze")
    # override gcc compiler to v10 for static analysis
    set(CMAKE_C_COMPILER /usr/bin/gcc-10)
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g -fanalyzer)
    message(STATUS "enabling static analysis")
endif()

# handle ci build type
if(CMAKE_BUILD_TYPE STREQUAL "CI")
    set(CMAKE_BUILD_TYPE "Release")
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -I/home/travis/build/RTradeLtd/libcp2p/destdir/usr/include/ -I /home/travis/build/RTradeLtd/libcp2p/tinycbor -I/usr/local/lib/libtinycbor.so.0.5 -I/usr/local/lib)
    # override the previous C compiler choice
    set(CMAKE_C_COMPILER /usr/bin/gcc-9)
endif()


# handle no build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic)
endif()

list(APPEND flags -D_POSIX_C_SOURCE=201112L)

set(CMAKE_C_FLAGS_RELEASE "")

include_directories(${PROJECT_SOURCE_DIR}/include)
include(${PROJECT_SOURCE_DIR}/cmake/pkgs/doxygen.cmake)

###################
# END CMAKE SETUP #
###################

###########################
# START COMPILE LIBRARIES #
###########################

# multiaddr #

include(cmake/libraries/multiaddr.cmake)

# multihash #

include(cmake/libraries/multihash.cmake)

# protobuf #

include(cmake/libraries/protobuf.cmake)

# crypto #

include(cmake/libraries/crypto.cmake)

# multibase #

include(cmake/libraries/multibase.cmake)

# cid #

include(cmake/libraries/cid.cmake)

# encoding #

include(cmake/libraries/encoding.cmake)

# utils #

include(cmake/libraries/utils.cmake)

# valgrind #

include(cmake/pkgs/valgrind.cmake)

# multicodec #

include(cmake/libraries/multicodec.cmake)

# networking #

include(cmake/libraries/network.cmake)

# tinycbor #

include(cmake/libraries/tinycbor.cmake)

# cli #

include(cmake/libraries/cli.cmake)

#########################
# END COMPILE LIBRARIES #
#########################

##############
# MISC TASKS #
##############

# build documentation
add_doxygen_doc(
    BUILD_DIR
        ${PROJECT_SOURCE_DIR}/build
    DOXY_FILE
        ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in
    TARGET_NAME
        doxygen-docs
    COMMENT
        "HTML documentation"
)

#################
# TESTING SETUP #
#################

define_setup_valgrind()

enable_testing()