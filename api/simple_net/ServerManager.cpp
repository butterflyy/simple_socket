#include "ServerManager.h"
#include "Server.h"


ServerManager::ServerManager()
:_listening(false){
}


ServerManager::~ServerManager()
{
}

void ServerManager::StartServer(int port){
	if (_listening){
		throw SimpleNetException("Server is already runing", SN_NETWORK_ERROR);
	}
	else{
		_serverSocket.bind(port);
		_serverSocket.listen();

		Thread::start();

		_listening = true;
	}
}

ServerManager::ServerList ServerManager::GetServers(){
	utils::LockGuard<utils::Mutex> lock(_serversMutex);

	ServerList mags;

	for (ServerList::const_iterator it = _servers.begin();
		it != _servers.end();
		++it){
		if ((*it)->IsConnected()){
			mags.push_back(*it);
		}
	}

	return mags;
}

void ServerManager::StopServer(){
	if (_listening){
		Thread::quit();
		_serverSocket.close();
		_listening = false;

		//remove and disconnected client
		utils::LockGuard<utils::Mutex> lock(_serversMutex);

		for (ServerList::const_iterator it = _servers.begin();
			it != _servers.end();
			++it){
			(*it)->Disconnect();
			delete (*it);
		}

		_servers.clear();
	}
}

void ServerManager::removeDeadConnect(){
	for (ServerList::iterator it = _servers.begin();
		it != _servers.end();){
		if ((*it)->IsDead()){
			delete (*it);
			it = _servers.erase(it);
		}
		else{
			it++;
		}
	}
}

void ServerManager::run(){
	LOG(INFO) << "ServerManager run begin";

	while (!Thread::isQuit()){
		//recv client connect
		try{
			Poco::Timespan timeout(HEARTBEAT_TIME * 1000);
			if (_serverSocket.poll(timeout, Socket::SELECT_READ)){
				//new client
				StreamSocket socket = _serverSocket.acceptConnection();
				LOG(INFO) << "acceptConnection, current connect size = " << _servers.size();
				Server* mag = createConnection(socket);
				mag->start();

				_serversMutex.lock();
				_servers.push_back(mag);
				_serversMutex.unlock();
			}
		}
		catch (Poco::Exception& e){
			LOG(ERROR) << e.displayText();
		}

		//remove disconnect client
		_serversMutex.lock();
		removeDeadConnect();
		_serversMutex.unlock();
	}

	LOG(INFO) << "ServerManager run end";
}
