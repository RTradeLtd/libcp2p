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