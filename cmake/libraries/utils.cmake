file(GLOB_RECURSE UTIL_SOURCES
    ./include/utils/*.h
    ./src/utils/*.c
)

add_library(libutils
    SHARED
        ${UTIL_SOURCES}
)

target_compile_options(libutils PRIVATE ${flags})
target_link_libraries(libutils pthread)

add_executable(libutils-test ./tests/utils_test.c)
target_compile_options(libutils-test PRIVATE ${flags})
target_link_libraries(libutils-test libutils)
target_link_libraries(libutils-test cmocka)
target_link_libraries(libutils-test libthreadpool)
target_link_libraries(libutils-test liblogger)
add_test(NAME LibUtilsTest COMMAND libutils-test)