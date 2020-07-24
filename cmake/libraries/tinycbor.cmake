file(GLOB_RECURSE TINYCBOR_SOURCES
    ./include/tinycbor/src/*.h
    ./src/tinycbor/*.c
)

add_library(libtinycbor
    SHARED
        ${TINYCBOR_SOURCES}
)


# target_compile_options(tinycbor PRIVATE ${flags})
