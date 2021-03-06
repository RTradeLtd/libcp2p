add_library(libmultiaddr
    SHARED
        ./include/multiaddr/endian.h
        ./include/multiaddr/multiaddr.h
        ./include/multiaddr/protocols.h
        ./include/multiaddr/protoutils.h
        ./src/multiaddr/multiaddr.c
        ./src/multiaddr/protocols.c
        ./src/multiaddr/protoutils.c
)
target_compile_options(libmultiaddr PRIVATE ${flags})
target_link_libraries(libmultiaddr m)
target_link_libraries(libmultiaddr libencoding)
target_link_libraries(libmultiaddr libutils)
target_link_libraries(libmultiaddr libmultibase)

add_executable(libmultiaddr-test ./tests/multiaddr_test.c)
target_link_libraries(libmultiaddr-test cmocka)
target_link_libraries(libmultiaddr-test libmultiaddr)
target_link_libraries(libmultiaddr-test m)

add_test(NAME LibMultiAddrTest COMMAND libmultiaddr-test)