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
		EventData eventData;
		memset(&eventData, 0, sizeof(EventData));
		eventData.type = EVENT_DISCONNECT;

		EVENT->OnCallback(eventData);
	}
}

void ClientImp::OnError(int error_code, const std::string& error_msg){
	if (IsCalled()){
		EventData eventData;
		memset(&eventData, 0, sizeof(EventData));
		eventData.type = EVENT_ERROR;
		eventData.error_code = TransError(error_code);

		EVENT->OnCallback(eventData);
	}
}

void ClientImp::OnRecvFrame(const byte* data, int len, int type){
	if (IsCalled()){
		EventData eventData;
		memset(&eventData, 0, sizeof(EventData));
		eventData.type = EVENT_RECV_FRAME;

		eventData.frame.data = new byte[len + 1];
		memcpy(eventData.frame.data, data, len);
		eventData.frame.data[len] = 0;

		eventData.frame.len = len;
		eventData.frame.type = type;

		EVENT->OnCallback(eventData);
	}
}
