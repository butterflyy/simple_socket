#pragma once

#include <string>

typedef void* ;

class NetEventInterface  {
public:
	//event
	virtual void OnConnected( mag) = 0;

	virtual void OnDisconnected( mag) = 0;

	virtual void OnError( mag, int error_code, const std::string& error_msg) = 0;

	virtual void OnRecv( mag, const unsigned char* data, int len) = 0;
};