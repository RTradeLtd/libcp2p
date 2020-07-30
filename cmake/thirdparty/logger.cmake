file(GLOB_RECURSE LOGGER_SOURCES
    ./include/thirdparty/logger/*.h
    ./thirdparty/logger/*.c
)

add_library(liblogger
    SHARED
        ${LOGGER_SOURCES}
)

target_compile_options(liblogger PRIVATE ${flags})