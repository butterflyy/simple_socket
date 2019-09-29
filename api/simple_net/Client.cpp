#include "Client.h"
#include "NetProtocol.h"


Client::Client()
{
}


Client::~Client()
{
}

void Client::Connect(const char* ip, int port){
	_called = true;

	if (_connected){
		throw SimpleNetException("Client is already connected", SN_NETWORK_ERROR);
	}
	else{
		//make sure thread quit if loop connected.
		Thread::quit();

		try{
			Poco::Timespan timeoutc(2000000);
			_socket.connect(Poco::Net::SocketAddress(ip, port), timeoutc);

			//handshake
			sendFrame(MSG_HANDSHAKE, FRAME_BINARY, nullptr, 0);
			Poco::Timespan timeout(2000000);
			if (_socket.poll(timeout, Socket::SELECT_READ)){
				int msgtype;
				int frametype;
				recvFrame(&msgtype, &frametype, nullptr, 0);
				if (msgtype != MSG_HANDSHAKE){
					throw SimpleNetException("Recv handshake ack error", SN_NETWORK_ERROR);
				}
			}
			else{
				throw Poco::TimeoutException("No handshake ack");
			}
		}
		catch (...){
			close();
			throw;
		}


		Thread::start();

		_connected = true;
	}
}

void Client::Disconnect(){
	_called = false;

	if (_connected){
		Thread::quit();
		_connected = false;
	}
}

void Client::run(){
	std::string peerdes;
	{
		EXCEPTION_BEGIN
			peerdes = _socket.peerAddress().toString();
		EXCEPTION_END
	}

	LOG(INFO) << "Client:" << peerdes << " run begin";

	_sendSpan.start();
	_recvSpan.start();

	while (!Thread::isQuit()){
		Poco::Timespan timeout(1000000);
		if (_socket.poll(timeout, Socket::SELECT_READ)){
			EXCEPTION_BEGIN_PEER(peerdes)
				int msgtype;
				int frametype;
				int nRecv = recvFrame(&msgtype, &frametype, _recvbuff, _recvlen);
				assert(msgtype == MSG_NORMAL || msgtype == MSG_HEARBEAT);

				_recvSpan.restart();

				if (msgtype == MSG_NORMAL){
					LogFrame(false, _recvbuff, nRecv, frametype);

					OnRecvFrame(_recvbuff, nRecv, frametype);
				}
			EXCEPTION_END

			if (error_code != 0){
				if (error_code == SN_NETWORK_DISCONNECTED){
					break;
				}
				else{
					//error handle
					if (error_code == SN_PAYLOAD_TOO_BIG || error_code == SN_FRAME_ERROR){
						EXCEPTION_BEGIN_PEER(peerdes)
							//read empty buffer
							readEmptyBuffer();
						EXCEPTION_END
					}

					OnError(error_code, error_msg);
				}
			}
		}
		else{//check hearbeat
			if (_sendSpan.elapsed() > HEARTBEAT_TIME){
				utils::LockGuard<utils::Mutex> lock(_sendMutex);

				EXCEPTION_BEGIN_PEER(peerdes)
					sendFrame(MSG_HEARBEAT, FRAME_BINARY, nullptr, 0);
				EXCEPTION_END

				_sendSpan.restart();
			}
			else if (_recvSpan.elapsed() > KEEPALIVE_TIMEOUT){
				LOG(ERROR) << peerdes << " keepalive timeout";
				break;//disconnected.
			}
		}
	}
	EXCEPTION_BEGIN_PEER(peerdes)
		close();
	EXCEPTION_END

	_connected = false;
	OnDisconnected();

	LOG(INFO) << "Client:" << peerdes << " run end";
}


