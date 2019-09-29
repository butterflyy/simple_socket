#define complier 
CC = arm-linux-gnueabi-gcc
CFLAGS = -Wall -DNDEBUG

#define include lib path
INCLUDE_PATH = -I/opt/hs_project/sunxi/hslib/include/libusb-1.0/  -I/opt/hs_project/sunxi/hslib/include  -I./../../../externals/include/ -I../simple_net
LIBS = -lglog -lPocoFoundation -lPocoNet -L/opt/hs_project/sunxi/hslib/lib

OBJS = Client.o EventManager.o NetHelper.o Server.o ServerManager.o ClientImp.o whsarmclient.o

TARGET = libwhsarmclient.so

$(TARGET) : $(OBJS) 
	$(CC) -shared -o $@ $(OBJS)  $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -fPIC -c $<  $(INCLUDE_PATH)

.PHONY:clean
clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET)  /home/embest/Documents/tftpboot/libwhsarmclient.so
