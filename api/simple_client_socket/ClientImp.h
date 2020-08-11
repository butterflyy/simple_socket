#pragma once
#include "Common.h"
#include "Client.h"
#include "EventManager.h"

_SC_BEGIN

class ClientImp : public Client
{
public:
	ClientImp(const NetParam& netParam);
	~ClientImp();

	void OnConnected() override;
	void OnDisconnected() override;
	void OnError(int error_code, const std::string& error_msg) override;
	void OnRecvFrame(const byte* data, int len, int type) override;

	static int TransError(int err);
private:
};

_SC_END



