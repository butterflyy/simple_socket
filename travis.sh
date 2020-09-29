#!/usr/bin/env sh

# add externals
cd ..
git clone -b develop https://github.com/butterflyy/externals.git  --single-branch --depth=1
# build glog
cd externals/3rdparty/glog-0.4.0
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..

# build gtest
cd googletest-release-1.10.0
mkdir build
cd build
cmake .. -DBUILD_GMOCK=OFF
make
sudo make install
cd ../../..

#build simple_server_socket
cd simple_socket
mkdir build
cd build
cmake .. -DEXTERNALS_DIR=$(pwd)/../../externals
make

#run UnitTest
cd Bin
./UnitTest
