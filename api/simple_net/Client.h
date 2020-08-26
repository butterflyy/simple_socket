#pragma once
#include "NetHelper.h"

class Client : public NetHelper, private utils::Thread
{
public:
	Client(const NetParam& netParam);
	virtual ~Client();

	void Connect(const char* ip, int port);

	void Disconnect();
private:
	void run() override;
};

