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
target_link_libraries(libpeerstore libcrypto)
target_link_libraries(libpeerstore libtestutils)

add_executable(libpeerstore-test ./src/peerstore/peerstore_test.c)
target_compile_options(libpeerstore-test PRIVATE ${flags})
target_link_libraries(libpeerstore-test libpeerstore)
target_link_libraries(libpeerstore-test cmocka)
target_link_libraries(libpeerstore-test libtestutils)
target_link_libraries(libpeerstore-test libcrypto)
add_test(NAME LibPeerstoreTest COMMAND libpeerstore-test)
