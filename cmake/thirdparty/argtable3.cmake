file(GLOB_RECURSE ARGTABLE_SOURCES
    ./include/thirdparty/argtable3/*.h
    ./thirdparty/argtable3/*.c
)

add_library(libargtable
    SHARED
        ${ARGTABLE_SOURCES}
)

target_compile_options(libargtable PRIVATE ${flags})