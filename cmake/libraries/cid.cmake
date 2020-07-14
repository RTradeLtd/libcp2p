add_library(libcid
    SHARED
        ./include/cid/cid.h
        ./src/cid/cid.c
        ./src/cid/set.c
)

target_compile_options(libcid PRIVATE ${flags})
target_link_libraries(libcid libprotobuf)
target_link_libraries(libcid libmultibase)
target_link_libraries(libcid libcrypto)