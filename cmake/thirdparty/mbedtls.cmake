file(GLOB_RECURSE MBEDTLS_SOURCES
    ./include/mbedtls/*.h
    ./thirdparty/mbedtls/*.c
)

add_library(libmbedtls
    SHARED
        ${MBEDTLS_SOURCES}
)