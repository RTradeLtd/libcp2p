file(GLOB_RECURSE NETWORK_SOURCES
    ./include/network/*.h
    ./src/network/*.c
)

add_library(libnetwork
    SHARED
        ${NETWORK_SOURCES}
)

target_compile_options(libnetwork PRIVATE ${flags})
target_link_libraries(libnetwork pthread)
target_link_libraries(libnetwork libutils)