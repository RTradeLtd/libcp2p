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
target_link_libraries(libcrypto libmbedtls)
target_link_libraries(libcrypto libprotobuf)
target_link_libraries(libcrypto libmultihash)

add_executable(libcrypto-test ./src/crypto/crypto_test.c)
target_link_libraries(libcrypto-test libcrypto)
target_link_libraries(libcrypto-test libprotobuf)
target_link_libraries(libcrypto-test libmbedtls)
target_link_libraries(libcrypto-test cmocka)
target_link_libraries(libcrypto-test m)
add_test(NAME LibCryptoTest COMMAND libcrypto-test)