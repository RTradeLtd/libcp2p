add_library(libmultibase
    SHARED
        ./include/multibase/multibase.h
        ./src/multibase/multibase.c
)

target_compile_options(libmultibase PRIVATE ${flags})
target_link_libraries(libmultibase libencoding)


add_executable(libmultibase-test ./src/multibase/multibase_test.c)
target_link_libraries(libmultibase-test libmultibase)
target_link_libraries(libmultibase-test cmocka)
add_test(NAME LibMultiBaseTest COMMAND libmultibase-test)