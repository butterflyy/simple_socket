#pragma once
#include "Client.h"
#include "whsarmclient.h" //for callback

class ClientImp : public Client
{
public:
	ClientImp();
	~ClientImp();

	void OnConnected() override;
	void OnDisconnected() override;
	void OnError(int error_code, const std::string& error_msg) override;
	void OnRecvFrame(const byte* data, int len, int type) override;

	void SetCallback(sc_disconnected_callback on_disconnected,
		sc_error_callback on_error,
		sc_recvframe_callback on_recvframe);

	static int TransError(int err);
private:
	sc_disconnected_callback _on_disconnected;
	sc_error_callback _on_error;
	sc_recvframe_callback _on_recvframe;
};

inline void ClientImp::SetCallback(sc_disconnected_callback on_disconnected,
	sc_error_callback on_error,
	sc_recvframe_callback on_recvframe){
	_on_disconnected = on_disconnected;
	_on_error = on_error;
	_on_recvframe = on_recvframe;
}



