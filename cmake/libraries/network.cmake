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
target_link_libraries(libnetwork libmultiaddr)
target_link_libraries(libnetwork libtinycbor)
target_link_libraries(libnetwork libencoding)
target_link_libraries(libnetwork liblogger)
target_link_libraries(libnetwork libthreadpool)
target_link_libraries(libnetwork libpeerstore)

add_executable(libnetwork-test ./tests/socket_test.c)
target_compile_options(libnetwork-test PRIVATE ${flags})
target_link_libraries(libnetwork-test libnetwork)
target_link_libraries(libnetwork-test cmocka)
target_link_libraries(libnetwork-test libtestutils)
add_test(NAME LibNetworkTest COMMAND libnetwork-test)