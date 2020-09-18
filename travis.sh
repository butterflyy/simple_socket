# build glog
cd ..
git clone -b v0.3.4 https://github.com/google/glog.git
cd glog
./configure
autoreconf -ivf
make
sudo make install
cd ..

# add externals
git clone https://github.com/butterflyy/externals.git

#build simple_server_socket
cd simple_socket
cd api/simple_server_socket
sudo ./make.sh

#build simple_server_socket
cd ../simple_client_socket
sudo ./make.sh
