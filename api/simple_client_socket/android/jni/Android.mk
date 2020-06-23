LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LIBUSB_ROOT_REL:= ../../../simple_client_socket
LIBUSB_ROOT_ABS:= $(LOCAL_PATH)/../../../simple_client_socket

LOCAL_LDFLAGS  += \
					$(LIBUSB_ROOT_ABS)/../../../externals/poco-poco-1.7.3-release/Android/armeabi/libPocoFoundation.a \
					$(LIBUSB_ROOT_ABS)/../../../externals/poco-poco-1.7.3-release/Android/armeabi/libPocoNet.a \
				  

LOCAL_LDLIBS  := -llog

LOCAL_SRC_FILES:= \
					$(LIBUSB_ROOT_REL)/AndroidLog.cpp \
					$(LIBUSB_ROOT_REL)/ClientImp.cpp \
					$(LIBUSB_ROOT_REL)/EventManager.cpp \
					$(LIBUSB_ROOT_REL)/whsarmclient.cpp \
					$(LIBUSB_ROOT_REL)/../simple_net/Client.cpp \
					$(LIBUSB_ROOT_REL)/../simple_net/NetHelper.cpp \
                    
LOCAL_C_INCLUDES := \
					$(LIBUSB_ROOT_REL)/ \
					$(LIBUSB_ROOT_REL)/../simple_net \
					$(LIBUSB_ROOT_REL)/../../../externals/include \
					$(LIBUSB_ROOT_REL)/../../../externals/poco-poco-1.7.3-release/Foundation/include \
					$(LIBUSB_ROOT_REL)/../../../externals/poco-poco-1.7.3-release/Net/include \
                                        
LOCAL_MODULE:= whsarmclient


include $(BUILD_SHARED_LIBRARY)