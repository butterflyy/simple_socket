#pragma once
#include "ServerManager.h"
#include "whsarmserver.h" //for callback
#include "NetParam.h"


class ServerManagerImp : public ServerManager
{
public:
	//friend class ServerImp;

	ServerManagerImp();
	~ServerManagerImp();

	static int TransError(int err);
private:
	Server* createConnection(const StreamSocket& socket) override;
private:
};
