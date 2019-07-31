#pragma once
#include "NetHelper.h"

class NetEventInterface;
class Server : public NetHelper, private utils::Thread
{
public:
	friend class ServerManager;
	Server(const StreamSocket& socket);
	virtual ~Server();

	void Disconnect();

	bool IsDead() const;
private:

	void run() override;
private:
	bool _dead; //when disconnect, need remove.
};

inline bool Server::IsDead() const{
	return _dead;
}
