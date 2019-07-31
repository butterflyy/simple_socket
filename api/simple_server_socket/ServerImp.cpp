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
		if (_serverManagerImp->_on_connected){
			try{
				std::string client_ip = RemoteAddress();
				_serverManagerImp->_on_connected(this, client_ip.c_str());
			}
			catch (Poco::Exception& e){
				LOG(ERROR) << e.displayText();
			}
		}
	}
}

void ServerImp::OnDisconnected(){
	if (IsCalled()){
		if (_serverManagerImp->_on_disconnected){
			_serverManagerImp->_on_disconnected(this);
		}
	}
}

void ServerImp::OnError(int error_code, const std::string& error_msg){
	if (IsCalled()){
		if (_serverManagerImp->_on_error){
			_serverManagerImp->_on_error(this, ServerManagerImp::TransError(error_code));
		}
	}
}

void ServerImp::OnRecvFrame(const byte* data, int len, int type){
	if (IsCalled()){
		if (_serverManagerImp->_on_recvframe){
			_serverManagerImp->_on_recvframe(this, data, len, type);
		}
	}
}
