#! /bin/bash

rm -rf build
find . -type f -name "*.o" -exec rm {} \;
find . -type f -name "*.a" -exec rm {} \;
