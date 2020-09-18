pwd
# build glog
cd ..
git clone -b v0.3.4 https://github.com/google/glog.git
cd glog
./configure
autoreconf -ivf
make
sudo make install
cd ..
pwd

# build gtest
cd ..
git clone -b release-1.10.0 https://github.com/google/googletest.git
cd googletest
cmake CMakeLists.txt
make
sudo make install
cd ..
pwd

# add externals
git clone https://github.com/butterflyy/externals.git
pwd

#build simple_server_socket
cd simple_socket
cd api/simple_server_socket
sudo ./make.sh
pwd

#build simple_server_socket
cd ../simple_client_socket
sudo ./make.sh
pwd

#build UnitTest
cd ../../test/UnitTest/UnitTest
make
pwd

#run UnitTest
./UnitTest
