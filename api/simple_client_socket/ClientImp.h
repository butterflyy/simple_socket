#pragma once
#include "Client.h"
#include "EventManager.h"

class ClientImp : public Client
{
public:
	ClientImp();
	~ClientImp();

	void OnConnected() override;
	void OnDisconnected() override;
	void OnError(int error_code, const std::string& error_msg) override;
	void OnRecvFrame(const byte* data, int len, int type) override;

	static int TransError(int err);
private:
};



