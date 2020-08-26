#define complier 
CC = g++
CFLAGS = -std=c++11 -DNDEBUG -Wl,-Bsymbolic

#define include lib path
INCLUDE_PATH = -I./../../../externals/include -I../simple_net -I../Poco/Foundation -I../Poco/Net
LIBS = -ldl -lglog

OBJS = whsarmserver.o \
		ServerManagerImp.o \
		ServerImp.o \
		EventManager.o \
		Server.o \
		ServerManager.o \
		NetHelper.o \
		Timestamp.o \
		Timespan.o \
		RefCountedObject.o \
		Exception.o \
		Bugcheck.o \
		Debugger.o \
		AtomicCounter.o \
		Ascii.o \
		IPAddress.o \
		IPAddressImpl.o \
		Net.o \
		NetException.o \
		ServerSocket.o \
		ServerSocketImpl.o \
		Socket.o \
		SocketAddress.o \
		SocketAddressImpl.o \
		SocketImpl.o \
		StreamSocket.o \
		StreamSocketImpl.o \

TARGET = libwhsarmserver.so

$(TARGET) : $(OBJS) 
	$(CC) -shared -o $@ $(OBJS)  $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -fPIC -c $<  $(INCLUDE_PATH)

.PHONY:clean
clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp $(TARGET) /usr/lib
