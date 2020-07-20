#! /bin/bash

# used to install mbed tls

git clone https://github.com/ARMmbed/mbedtls.git
cd mbedtls
git checkout mbedtls-2.23.0
make CFLAGS="-fPIC"
sudo make install CFLAGS="-fPIC"
sudo ldconfig