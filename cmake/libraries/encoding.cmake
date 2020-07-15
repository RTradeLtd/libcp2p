file(GLOB_RECURSE ENCODING_SOURCES
    ./include/encoding/*.h
    ./src/encoding/*.c
)

add_library(libencoding
    SHARED
        ${ENCODING_SOURCES}
)

target_compile_options(libencoding PRIVATE ${flags})
target_link_libraries(libencoding mbedtls)
target_link_libraries(libencoding mbedx509)
target_link_libraries(libencoding mbedcrypto)
target_link_libraries(libencoding m)