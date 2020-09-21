#pragma once
#include "Server.h"
#include "EventManager.h"


class ServerManagerImp;
class ServerImp : public Server
{
public:
	ServerImp(ServerManagerImp* serverManagerImp, const StreamSocket& socket);
	~ServerImp();

	void OnConnected() override;
	void OnDisconnected() override;
	void OnError(int error_code, const std::string& error_msg) override;
	void OnRecvFrame(const byte* data, int len, int type) override;
private:
	ServerManagerImp* _serverManagerImp;
};


