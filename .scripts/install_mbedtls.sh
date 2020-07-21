#! /bin/bash

# used to install mbed tls

git clone https://github.com/ARMmbed/mbedtls.git
cd mbedtls
git checkout mbedtls-2.23.0
# dont build the tests with mbedtls
make no_test CFLAGS="-fPIC"
sudo make install CFLAGS="-fPIC"
sudo ldconfig