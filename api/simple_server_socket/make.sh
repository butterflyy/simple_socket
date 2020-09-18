make clean
cp -f Makefile_first Makefile
make
cp -f Makefile_seconed Makefile
make
cp -f libwhsarmserver.so /usr/lib
cp -f libwhsarmserver.so /lib64
