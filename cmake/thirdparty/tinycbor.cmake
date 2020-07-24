file(GLOB_RECURSE TINYCBOR_SOURCES
    ${PROJECT_SOURCE_DIR}/thirdparty/tinycbor/src/*.h
    ${PROJECT_SOURCE_DIR}/thirdparty/tinycbor/src/*.c
)

add_library(tinycbor
    SHARED
        ${TINYCBOR_SOURCES}
)



