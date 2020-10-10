#build simple_server_socket
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm_linux_setup.cmake  -DGCC_STATIC_LINK=ON -DEXTERNALS_DIR=$(pwd)/../../externals
make
