#include "Server.h"
#include "NetProtocol.h"



Server::Server(const StreamSocket& socket)
:NetHelper(socket),
_dead(false)
{
}


Server::~Server()
{
}


void Server::Disconnect(){
	_called = false;

	if (_connected){
		Thread::quit();
		_connected = false;
	}
}

void Server::run(){
	std::string peerdes;
	{
		EXCEPTION_BEGIN
			peerdes = _socket.peerAddress().toString();
		EXCEPTION_END
	}


	LOG(INFO) << "Server:" << peerdes << " run begin";

	{
		EXCEPTION_BEGIN
			Poco::Timespan timeout(200000);
			if (_socket.poll(timeout, Socket::SELECT_READ)){
				int msgtype;
				int frametype;
				recvFrame(&msgtype, &frametype, nullptr, 0);
				if (msgtype != MSG_HANDSHAKE){
					close();
					throw SimpleNetException("Recv handshake ack error", SN_NETWORK_ERROR);
				}
			}
			else{
				close();
				throw Poco::TimeoutException("No handshake ack");
			}
			sendFrame(MSG_HANDSHAKE, FRAME_BINARY, nullptr, 0);
		EXCEPTION_END
		if (error_code != 0){
			_dead = true;
			return;
		}
	}

	_connected = true;
	OnConnected();

	while (!Thread::isQuit()){
		Poco::Timespan timeout(100000);
		if (_socket.poll(timeout, Socket::SELECT_READ)){
			EXCEPTION_BEGIN
				int msgtype;
				int frametype;
				int nRecv = recvFrame(&msgtype, &frametype, _recvbuff, _recvlen);
				assert(msgtype == MSG_NORMAL);

				OnRecvFrame(_recvbuff, nRecv, frametype);
			EXCEPTION_END

			if (error_code != 0){
				if (error_code == SN_NETWORK_DISCONNECTED){
					break;
				}
				else{
					//error handle
					if (error_code == SN_PAYLOAD_TOO_BIG || error_code == SN_FRAME_ERROR){
						EXCEPTION_BEGIN
							//read empty buffer
							readEmptyBuffer();
						EXCEPTION_END
					}

					OnError(error_code, error_msg);
				}
			}

		}
	}
	EXCEPTION_BEGIN
		close();
	EXCEPTION_END

	_connected = false;
	_dead = true;
	OnDisconnected();

	LOG(INFO) << "Server:" << peerdes << " run end";
}


