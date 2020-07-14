file(GLOB_RECURSE CRYPTO_SOURCES
    ./include/crypto/*.h
    ./include/crypto/encoding/*.h
    ./src/crypto/*.c
    ./src/crypto/encoding/*.c
)

add_library(libcrypto
    SHARED
        ${CRYPTO_SOURCES}
)

target_compile_options(libcrypto PRIVATE ${flags})
target_link_libraries(libcrypto libprotobuf)
target_link_libraries(libcrypto libmultihash)
target_link_libraries(libcrypto mbedtls)
target_link_libraries(libcrypto mbedx509)
target_link_libraries(libcrypto mbedcrypto)

add_executable(libcrypto-test ./src/crypto/crypto_test.c)
target_link_libraries(libcrypto-test mbedtls)
target_link_libraries(libcrypto-test mbedx509)
target_link_libraries(libcrypto-test mbedcrypto)
target_link_libraries(libcrypto-test libcrypto)
target_link_libraries(libcrypto-test libprotobuf)
target_link_libraries(libcrypto-test cmocka)
target_link_libraries(libcrypto-test m)
add_test(NAME LibCryptoTest COMMAND libcrypto-test)