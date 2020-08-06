file(GLOB_RECURSE MULTICODEC_SOURCES
    ./include/multicodec/*.h
    ./src/multicodec/*.c
)

add_library(libmulticodec
    SHARED
        ${MULTICODEC_SOURCES}
)

target_compile_options(libmulticodec PRIVATE ${flags})

add_executable(libmulticodec-test ./tests/multicodec_test.c)
target_compile_options(libmulticodec-test PRIVATE ${flags})
target_link_libraries(libmulticodec-test libmulticodec)
target_link_libraries(libmulticodec-test cmocka)

add_test(NAME LibMultiCodecTest COMMAND libmulticodec-test)