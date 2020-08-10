#pragma once

#define USING_GLOG                      1
#define LOG_FRAME_DATA                  1

#include <Poco/Timespan.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/NetException.h>

#include <common/utils.h>
#include <common/TimeSpan.h>
#include <list>

//log
#if USING_GLOG
#ifdef ERROR
#undef ERROR
#endif

#ifdef __ANDROID__
#include "AndroidLog.h"
#else
#define GOOGLE_GLOG_DLL_DECL //using logging static library
#include <glog/logging.h>
#endif
#else
#include <iostream>
#define LOG(s) std::cout
#endif

//on linux if call 2 dll, may be used this
extern bool FLAGS_glog_init;

using Poco::Net::Socket;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::NetException;

