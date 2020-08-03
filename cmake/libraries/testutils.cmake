file(GLOB_RECURSE TESTUTILS_SOURCES
    ./include/testutils/*.h
    ./src/testutils/*.c
)

add_library(libtestutils
    SHARED
        ${TESTUTILS_SOURCES}
)

target_compile_options(libtestutils PRIVATE ${flags})
target_link_libraries(libcrypto mbedtls)
target_link_libraries(libcrypto mbedx509)
target_link_libraries(libcrypto mbedcrypto)
target_link_libraries(libtestutils libcrypto)