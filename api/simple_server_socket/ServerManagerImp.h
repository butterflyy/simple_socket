#pragma once
#include "Common.h"
#include "ServerManager.h"
#include "whsarmserver.h" //for callback
#include "NetParam.h"

_SS_BEGIN

class ServerManagerImp : public ServerManager
{
public:
	//friend class ServerImp;

	ServerManagerImp(const NetParam& netParam);
	~ServerManagerImp();

	static int TransError(int err);
private:
	Server* createConnection(const StreamSocket& socket) override;
private:
	NetParam _netParam;
};

_SS_END


