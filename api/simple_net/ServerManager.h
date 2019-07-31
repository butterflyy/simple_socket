#pragma once

#include "NetConfig.h"

class Server;
class ServerManager : public utils::Thread
{
public:
	typedef std::list<Server*> ServerList;
	ServerManager();

	~ServerManager();

	void StartServer(int port);

	void StopServer();

	ServerList GetServers();

	virtual Server* createConnection(const StreamSocket& socket) = 0;

private:
	void run() override;
	void removeDeadConnect();
private:
	ServerList _servers;
	utils::Mutex _serversMutex;

	ServerSocket _serverSocket;
	bool _listening;
};

