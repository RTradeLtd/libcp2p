#! /bin/bash


# (mkdir cmbuild ; cd cmbuild ; wget https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz ; tar -Jxvf cmocka-1.1.5.tar.xz ; cd cmocka-1.1.5 ; mkdir build ; cd build ; cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug .. ; make ; sudo make -j$(nproc) install ; sudo ldconfig)

mkdir cmbuild
cd cmbuild
wget https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz
tar -Jxvf cmocka-1.1.5.tar.xz 
cd cmocka-1.1.5
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=/usr -D CMAKE_BUILD_TYPE=Debug -D CMAKE_C_COMPILER=/usr/bin/gcc-9 .. 
make
sudo make -j$(nproc) install
sudo ldconfig
cd "$TRAVIS_BUILD_DIR" # go back to the build directory
rm -rf cmbuild