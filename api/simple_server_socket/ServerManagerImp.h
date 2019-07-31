#pragma once
#include "ServerManager.h"
#include "whsarmserver.h" //for callback
class ServerManagerImp : public ServerManager
{
public:
	friend class ServerImp;

	ServerManagerImp();
	~ServerManagerImp();

	void SetCallback(ss_connected_callback on_connected,
		ss_disconnected_callback on_disconnected,
		ss_error_callback on_error,
		ss_recvframe_callback on_recvframe);

	static int TransError(int err);
private:
	Server* createConnection(const StreamSocket& socket) override;
private:
	ss_connected_callback _on_connected;
	ss_disconnected_callback _on_disconnected;
	ss_error_callback _on_error;
	ss_recvframe_callback _on_recvframe;
};

inline void ServerManagerImp::SetCallback(ss_connected_callback on_connected,
	ss_disconnected_callback on_disconnected,
	ss_error_callback on_error,
	ss_recvframe_callback on_recvframe){
	_on_connected = on_connected;
	_on_disconnected = on_disconnected;
	_on_error = on_error;
	_on_recvframe = on_recvframe;
}

