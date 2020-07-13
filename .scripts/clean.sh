#! /bin/bash

rm -rf build
rm -rf docs-build

find . -type f -name "*.o" -exec rm {} \;
find . -type f -name "*.a" -exec rm {} \;
