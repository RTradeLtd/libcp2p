file(GLOB_RECURSE PEERSTORE_SOURCES
    ./include/peerstore/*.h
    ./src/peerstore/*.c
)

add_library(libpeerstore
    SHARED
        ${PEERSTORE_SOURCES}
)

target_compile_options(libpeerstore PRIVATE ${flags})
target_link_libraries(libpeerstore pthread)