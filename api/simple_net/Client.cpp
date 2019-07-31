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
		Poco::Timespan timeoutc(100000);
		_socket.connect(Poco::Net::SocketAddress(ip, port), timeoutc);

		//make sure thread quit if loop connected.
		Thread::quit();

		//handshake
		sendFrame(MSG_HANDSHAKE, FRAME_BINARY, nullptr, 0);
		Poco::Timespan timeout(200000);
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
	LOG(INFO) << "Client run begin";

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
	OnDisconnected();

	LOG(INFO) << "Client run end";
}


