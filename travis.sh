#!/usr/bin/env sh

#sudo echo  /usr/local/lib >>/etc/ld.so.conf
#sudo ldconfig

pwd
# build glog
cd ..
git clone -b v0.4.0 https://github.com/google/glog.git --single-branch --depth=1
cd glog
cmake CMakeLists.txt
make
sudo make install
sudo cp /usr/local/lib/libglog.* /usr/lib/
cd ..
pwd

# build gtest
git clone -b release-1.10.0 https://github.com/google/googletest.git  --single-branch --depth=1
cd googletest
cmake CMakeLists.txt
make
sudo make install
cd ..
pwd

# add externals
git clone -b develop https://github.com/butterflyy/externals.git  --single-branch --depth=1
pwd

#build simple_server_socket
cd simple_socket
mkdir build
cd build
cmake .. -DEXTERNALS_DIR=$(pwd)/../../externals
make

#run UnitTest
./UnitTest
