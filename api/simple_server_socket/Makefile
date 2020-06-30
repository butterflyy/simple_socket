#define complier 
CC = g++
CFLAGS = -Wall -std=c++11 -DNDEBUG

#define include lib path
INCLUDE_PATH = -I./../../../externals/include -I../simple_net -I../Poco/Foundation -I../Poco/Net
LIBS = -lglog

OBJS = whsarmserver.o \
		ServerManagerImp.o \
		ServerImp.o \
		EventManager.o \
		../simple_net/Server.o \
		../simple_net/ServerManager.o \
		../simple_net/NetHelper.o \
		../Poco/Foundation/Poco/Timestamp.o \
		../Poco/Foundation/Poco/Timespan.o \
		../Poco/Foundation/Poco/RefCountedObject.o \
		../Poco/Foundation/Poco/Exception.o \
		../Poco/Foundation/Poco/Bugcheck.o \
		../Poco/Foundation/Poco/Debugger.o \
		../Poco/Foundation/Poco/AtomicCounter.o \
		../Poco/Foundation/Poco/Ascii.o \
		../Poco/Net/Poco/Net/IPAddress.o \
		../Poco/Net/Poco/Net/IPAddressImpl.o \
		../Poco/Net/Poco/Net/Net.o \
		../Poco/Net/Poco/Net/NetException.o \
		../Poco/Net/Poco/Net/ServerSocket.o \
		../Poco/Net/Poco/Net/ServerSocketImpl.o \
		../Poco/Net/Poco/Net/Socket.o \
		../Poco/Net/Poco/Net/SocketAddress.o \
		../Poco/Net/Poco/Net/SocketAddressImpl.o \
		../Poco/Net/Poco/Net/SocketImpl.o \
		../Poco/Net/Poco/Net/StreamSocket.o \
		../Poco/Net/Poco/Net/StreamSocketImpl.o \

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
