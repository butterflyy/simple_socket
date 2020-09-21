#include "ServerImp.h"
#include "ServerManagerImp.h"


ServerImp::ServerImp(ServerManagerImp* serverManagerImp, const StreamSocket& socket)
:Server(socket),
_serverManagerImp(serverManagerImp){

}

ServerImp::~ServerImp(){

}

void ServerImp::OnConnected(){
	if (IsCalled()){
		EXCEPTION_BEGIN
			std::string client_ip = RemoteAddress();
			int client_port = RemotePort();

			EventData eventData;
			memset(&eventData, 0, sizeof(EventData));
			eventData.type = EVENT_CONNECT;
			eventData.session = this;
			eventData.client.server = _serverManagerImp;
			utils::SafeStrCpy(eventData.client.client_ip, client_ip.c_str(), 50);
			eventData.client.client_port = client_port;
		
			LOG(INFO) <<  PeerAddressLasting() << " OnConnected";
			EVENT->OnCallback(eventData);
		EXCEPTION_END
	}
}

void ServerImp::OnDisconnected(){
	if (IsCalled()){
		EventData eventData;
		memset(&eventData, 0, sizeof(EventData));
		eventData.type = EVENT_DISCONNECT;
		eventData.session = this;

		LOG(INFO) << PeerAddressLasting() << " OnDisconnected";
		EVENT->OnCallback(eventData);
	}
}

void ServerImp::OnError(int error_code, const std::string& error_msg){
	if (IsCalled()){
			EventData eventData;
			memset(&eventData, 0, sizeof(EventData));
			eventData.type = EVENT_ERROR;
			eventData.session = this;
			eventData.error_code = ServerManagerImp::TransError(error_code);

			LOG(INFO) << PeerAddressLasting() << " OnError";
			EVENT->OnCallback(eventData);
	}
}

void ServerImp::OnRecvFrame(const byte* data, int len, int type){
	if (IsCalled()){
		EventData eventData;
		memset(&eventData, 0, sizeof(EventData));
		eventData.type = EVENT_RECV_FRAME;
		eventData.session = this;

		eventData.frame.data = new byte[len + 1];
		memcpy(eventData.frame.data, data, len);
		eventData.frame.data[len] = 0;

		eventData.frame.len = len;
		eventData.frame.type = type;

		LOG(INFO) << PeerAddressLasting() << " OnRecvFrame";
		EVENT->OnCallback(eventData);
	}
}