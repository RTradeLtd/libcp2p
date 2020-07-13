add_library(libvarint
    SHARED
        ./src/protobuf/varint.h
        ./src/protobuf/varint.c
)
target_compile_options(libvarint PRIVATE ${flags})

add_library(libprotobuf
    SHARED
        ./src/protobuf/protobuf.h
        ./src/protobuf/protobuf.c
)
target_compile_options(libprotobuf PRIVATE ${flags})
target_link_libraries(libprotobuf libvarint)

add_executable(libprotobuf-cli ./src/protobuf/main.c)
target_link_libraries(libprotobuf-cli libvarint)
target_link_libraries(libprotobuf-cli libprotobuf)
target_compile_options(libprotobuf-cli PRIVATE ${flags})