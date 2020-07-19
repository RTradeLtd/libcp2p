file(GLOB_RECURSE MULTICODEC_SOURCES
    ./include/multicodec/*.h
    ./src/multicodec/*.c
)

add_library(libmulticodec
    SHARED
        ${MULTICODEC_SOURCES}
)

target_compile_options(libmulticodec PRIVATE ${flags})