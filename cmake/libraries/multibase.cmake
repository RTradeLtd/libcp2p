add_library(libmultibase
    SHARED
        ./include/multibase/multibase.h
        ./src/multibase/multibase.c
)
target_compile_options(libmultibase PRIVATE ${flags})

target_link_libraries(libmultibase libcrypto)