file(GLOB_RECURSE THREADPOOL_SOURCES
    ./include/thirdparty/thread_pool/*.h
    ./thirdparty/thread_pool/*.c
)

add_library(libthreadpool
    SHARED
        ${THREADPOOL_SOURCES}
)

target_compile_options(libthreadpool PRIVATE ${flags})