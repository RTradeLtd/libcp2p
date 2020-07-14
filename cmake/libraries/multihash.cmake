add_library(libmultihash
    SHARED
        ./include/multihash/errors.h
        ./include/multihash/generic.h
        ./include/multihash/hashes.h
        ./include/multihash/multihash.h
        ./include/multihash/base64.h
        ./include/multihash/dec.h
        ./include/multihash/hex.h
        ./include/multihash/operations.h
        ./src/multihash/errors.c
        ./src/multihash/hashes.c
        ./src/multihash/multihash.c
        ./src/multihash/base64.c
        ./src/multihash/dec.c
        ./src/multihash/operations.c
)
target_compile_options(libmultihash PRIVATE ${flags})

add_executable(libmultihash-test ./src/multihash/multihash_test.c)
target_link_libraries(libmultihash-test cmocka)
target_link_libraries(libmultihash-test libmultihash)
target_link_libraries(libmultihash-test m)
add_test(NAME LibMultiHashTest COMMAND libmultihash-test)