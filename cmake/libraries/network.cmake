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

add_executable(libnetwork-test ./src/network/socket_test.c)
target_compile_options(libnetwork-test PRIVATE ${flags})
target_link_libraries(libnetwork-test libnetwork)
target_link_libraries(libnetwork-test cmocka)
add_test(NAME LibNetworkTest COMMAND libnetwork-test)