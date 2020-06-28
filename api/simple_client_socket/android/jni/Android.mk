LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LIBUSB_ROOT_REL:= ../../../simple_client_socket
LIBUSB_ROOT_ABS:= $(LOCAL_PATH)/../../../simple_client_socket
				  

LOCAL_LDLIBS  := -llog

LOCAL_SRC_FILES:= \
					$(LIBUSB_ROOT_REL)/AndroidLog.cpp \
					$(LIBUSB_ROOT_REL)/ClientImp.cpp \
					$(LIBUSB_ROOT_REL)/EventManager.cpp \
					$(LIBUSB_ROOT_REL)/whsarmclient.cpp \
					$(LIBUSB_ROOT_REL)/../simple_net/Client.cpp \
					$(LIBUSB_ROOT_REL)/../simple_net/NetHelper.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Timestamp.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Timespan.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/RefCountedObject.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Exception.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Bugcheck.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Debugger.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/AtomicCounter.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation/Poco/Ascii.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/IPAddress.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/IPAddressImpl.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/Net.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/NetException.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/ServerSocket.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/ServerSocketImpl.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/Socket.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/SocketAddress.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/SocketAddressImpl.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/SocketImpl.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/StreamSocket.cpp \
					$(LIBUSB_ROOT_REL)/../Poco/Net/Poco/Net/StreamSocketImpl.cpp \
                    
LOCAL_C_INCLUDES := \
					$(LIBUSB_ROOT_REL)/ \
					$(LIBUSB_ROOT_REL)/../simple_net \
					$(LIBUSB_ROOT_REL)/../../../externals/include \
					$(LIBUSB_ROOT_REL)/../Poco/Foundation \
					$(LIBUSB_ROOT_REL)/../Poco/Net \
                                        
LOCAL_MODULE:= whsarmclient


include $(BUILD_SHARED_LIBRARY)