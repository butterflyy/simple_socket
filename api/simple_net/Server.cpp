#include "Server.h"
#include "NetProtocol.h"



Server::Server(const StreamSocket& socket, const NetParam& netParam)
:NetHelper(socket, netParam),
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
	std::string addr_info;
	{
		EXCEPTION_BEGIN
			addr_info = FormatAddress();
		EXCEPTION_END
	}

	_peerAddressLasting = addr_info;


	LOG(INFO) << addr_info << "run begin";

	{
		EXCEPTION_BEGIN_ADDR(addr_info)
			Poco::Timespan timeout(2000000);
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


	_sendSpan.start();
	_recvSpan.start();
	long noalive_times = 0;

	_connected = true;
	OnConnected();

	while (!Thread::isQuit()){
		Poco::Timespan timeout(1000000);
		if (_socket.poll(timeout, Socket::SELECT_READ)){
			EXCEPTION_BEGIN_ADDR(addr_info)
				int msgtype;
				int frametype;
				int nRecv = recvFrame(&msgtype, &frametype, _recvbuff, _recvlen);
				assert(msgtype == MSG_NORMAL || msgtype == MSG_HEARBEAT);

				_recvSpan.restart();
				noalive_times = 0;

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
						EXCEPTION_BEGIN_ADDR(addr_info)
							//read empty buffer
							readEmptyBuffer();
						EXCEPTION_END
					}

					OnError(error_code, error_msg);
				}
			}
		}
		else{//check hearbeat
			if (_sendSpan.elapsed() > _netParam.keep_alive.heatbeat_time){
				utils::LockGuard<utils::Mutex> lock(_sendMutex);

				EXCEPTION_BEGIN_ADDR(addr_info)
					sendFrame(MSG_HEARBEAT, FRAME_BINARY, nullptr, 0);
				EXCEPTION_END

				_sendSpan.restart();
			}
			else if (_recvSpan.elapsed() > _netParam.keep_alive.keepalive_time){
				noalive_times++;
				LOG(INFO) << addr_info << " keepalive timeout, no alive times = " << noalive_times;
				if (noalive_times >= _netParam.keep_alive.keepalive_count){
					LOG(INFO) << addr_info << "keepalive timeout times > _netParam.keep_alive.keepalive_count, disconnect";
					break;//disconnected.
				}
				_recvSpan.restart();
			}
		}
	}
	EXCEPTION_BEGIN_ADDR(addr_info)
		close();
	EXCEPTION_END

	_connected = false;
	_dead = true;
	OnDisconnected();

	LOG(INFO) << addr_info << "run end";
}


