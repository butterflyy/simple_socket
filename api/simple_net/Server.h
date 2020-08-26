#pragma once
#include "NetHelper.h"

class NetEventInterface;
class Server : public NetHelper, private utils::Thread
{
public:
	friend class ServerManager;
	Server(const StreamSocket& socket, const NetParam& netParam);
	virtual ~Server();

	void Disconnect();

	bool IsDead() const;

	std::string PeerAddressLasting() const;
private:

	void run() override;
private:
	bool _dead; //when disconnect, need remove.
	std::string _peerAddressLasting; //if device disconnect, call RemoveAddress will carse exception
};

inline bool Server::IsDead() const{
	return _dead;
}

inline std::string Server::PeerAddressLasting() const{
	return _peerAddressLasting;
}
