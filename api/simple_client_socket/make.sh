make clean
cp -f Makefile_first Makefile
make
cp -f Makefile_seconed Makefile
make
cp -f libwhsarmclient.so /usr/lib
cp -f libwhsarmclient.so /lib64
