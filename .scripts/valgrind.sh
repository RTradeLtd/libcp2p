#! /bin/bash

# runs valgrind tests on master

cd build
ctest --extra-verbose -T memcheck