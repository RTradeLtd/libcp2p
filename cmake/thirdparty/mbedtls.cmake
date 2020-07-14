file(GLOB_RECURSE MBEDTLS_SOURCES
    ./include/mbedtls/*.h
    ./src/thirdparty/mbedtls/*.c
)

add_library(libmbedtls
    SHARED
        ${MBEDTLS_SOURCES}
)

# commenting this out as there are a lot of warnings
# target_compile_options(libmbedtls PRIVATE ${flags})