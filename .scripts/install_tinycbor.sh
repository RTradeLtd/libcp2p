#! /bin/bash

git clone https://github.com/intel/tinycbor.git
cd tinycbor
make -j$(nproc)
sudo make install