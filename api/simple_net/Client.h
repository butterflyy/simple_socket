#pragma once
#include "NetHelper.h"

class Client : public NetHelper, private utils::Thread
{
public:
	Client();
	virtual ~Client();

	void Connect(const char* ip, int port);

	void Disconnect();
private:
	void run() override;
};

