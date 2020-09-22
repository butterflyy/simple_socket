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

	ServerManagerImp();
	~ServerManagerImp();

	static int TransError(int err);
private:
	Server* createConnection(const StreamSocket& socket) override;
private:
};

_SS_END


