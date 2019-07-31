#include "ClientImp.h"


ClientImp::ClientImp(){

}

ClientImp::~ClientImp(){
}

int ClientImp::TransError(int error){
	switch (error)
	{
	case 0: return SC_SUCCESS;
	case SN_NETWORK_ERROR: return SC_NETWORK_ERROR;
	case SN_NETWORK_DISCONNECTED: return SC_NETWORK_DISCONNECTED;
	case SN_NETWORK_TIMEOUT: return SC_NETWORK_TIMEOUT;
	case SN_PAYLOAD_TOO_BIG: return SC_PAYLOAD_TOO_BIG;
	case SN_FRAME_ERROR: return SC_FRAME_ERROR;
	default: assert(false);  return SC_ERROR;
	}
}

void ClientImp::OnConnected(){
	assert(false);
}

void ClientImp::OnDisconnected(){
	if (IsCalled()){
		if (_on_disconnected){
			_on_disconnected();
		}
	}
}

void ClientImp::OnError(int error_code, const std::string& error_msg){
	if (IsCalled()){
		if (_on_error){
			_on_error(TransError(error_code));
		}
	}
}

void ClientImp::OnRecvFrame(const byte* data, int len, int type){
	if (IsCalled()){
		if (_on_recvframe){
			_on_recvframe(data, len, type);
		}
	}
}
