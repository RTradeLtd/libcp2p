#! /bin/bash


(mkdir cmbuild ; cd cmbuild ; wget https://cmocka.org/files/1.0/cmocka-1.0.1.tar.xz ; tar -Jxvf cmocka-1.0.1.tar.xz ; mkdir cmocka-1.0.1/build ; pushd cmocka-1.0.1/build ; - cmake ../ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_BUILD_TYPE=Release ; sudo make -j$(nproc) install ; sudo ldconfig)