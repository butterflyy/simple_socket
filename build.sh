#build simple_server_socket
rm -rf build
mkdir build
cd build
cmake .. -DEXTERNALS_DIR=$(pwd)/../../externals
make

#run UnitTest
cd bin
./UnitTest
