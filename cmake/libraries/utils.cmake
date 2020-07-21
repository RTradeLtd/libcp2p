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