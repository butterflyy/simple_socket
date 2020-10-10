#build simple_server_socket
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm_linux_c98_setup.cmake  -DARM_D20=ON -DEXTERNALS_DIR=$(pwd)/../../externals
make
