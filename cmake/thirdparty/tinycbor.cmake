file(GLOB_RECURSE TINYCBOR_SOURCES
    ${PROJECT_SOURCE_DIR}/thirdparty/tinycbor/src/*.h
    ${PROJECT_SOURCE_DIR}/thirdparty/tinycbor/src/*.c
)

add_library(libtinycbor
    SHARED
        ${TINYCBOR_SOURCES}
)


# target_compile_options(tinycbor PRIVATE ${flags})
