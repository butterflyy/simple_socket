#include "ServerManagerImp.h"
#include "ServerImp.h"

_SS_BEGIN

ServerManagerImp::ServerManagerImp(const NetParam& netParam)
:_netParam(netParam)
{
}


ServerManagerImp::~ServerManagerImp()
{
}

Server* ServerManagerImp::createConnection(const StreamSocket& socket){
	return new ServerImp(this, socket, _netParam);
}

int ServerManagerImp::TransError(int error){
	switch (error)
	{
	case 0: return SS_SUCCESS;
	case SN_NETWORK_ERROR: return SS_NETWORK_ERROR;
	case SN_NETWORK_DISCONNECTED: return SS_NETWORK_DISCONNECTED;
	case SN_NETWORK_TIMEOUT: return SS_NETWORK_TIMEOUT;
	case SN_PAYLOAD_TOO_BIG: return SS_PAYLOAD_TOO_BIG;
	case SN_FRAME_ERROR: return SS_FRAME_ERROR;
	default: assert(false);  return SS_ERROR;
	}
}

_SS_END