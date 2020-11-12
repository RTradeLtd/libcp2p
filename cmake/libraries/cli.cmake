add_library(libcli
    SHARED
        ./include/cli/command_line.h
        ./src/cli/command_line.c
)

target_compile_options(libcli PRIVATE ${flags})

add_executable(cli ${PROJECT_SOURCE_DIR}/src/cli/main.c)
target_compile_options(cli PRIVATE ${flags})
target_link_libraries(cli libutils)
target_link_libraries(cli libargtable)
target_link_libraries(cli m)
target_link_libraries(cli libmultiaddr)
target_link_libraries(cli libnetwork) 
target_link_libraries(cli libcli)