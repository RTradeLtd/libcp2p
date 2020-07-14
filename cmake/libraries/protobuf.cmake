add_library(libprotobuf
    SHARED
        ./include/protobuf/varint.h
        ./include/protobuf/protobuf.h
        ./src/protobuf/varint.c
        ./src/protobuf/protobuf.c
)
target_compile_options(libprotobuf PRIVATE ${flags})

add_executable(libprotobuf-cli ./src/protobuf/main.c)
target_link_libraries(libprotobuf-cli libprotobuf)
target_compile_options(libprotobuf-cli PRIVATE ${flags})