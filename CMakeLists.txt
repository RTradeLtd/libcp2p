#####################
# START CMAKE SETUP #
#####################

cmake_minimum_required(VERSION 3.0)
project(libcp2p LANGUAGES C)
set(CMAKE_C_COMPILER /usr/bin/gcc)


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

enable_testing()