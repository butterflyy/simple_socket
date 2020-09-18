# build glog
cd ..
git clone -b v0.3.4 https://github.com/google/glog.git
cd glog
./configure
make
make install

#build simple_server_socket
cd ../simple_socket
cd api/simple_server_socket
cp Makefile_first Makefile
make
cp Makefile_second Makefile
make
make install

#build simple_server_socket
cd ../simple_client_socket
cp Makefile_first Makefile
make
cp Makefile_second Makefile
make
make install
