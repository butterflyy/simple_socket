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
			Poco::Timespan timeout(10*1000000);
			if (_socket.poll(timeout, Socket::SELECT_READ)){
				byte paramBuff[MAX_TCP_PARAM];
				int msgtype;
				int frametype;
				recvFrame(&msgtype, &frametype, paramBuff, MAX_TCP_PARAM);
				if (msgtype != MSG_HANDSHAKE){
					close();
					throw SimpleNetException("Recv handshake ack error", SN_NETWORK_ERROR);
				}
			}
			else{
				close();
				throw Poco::TimeoutException("No handshake ack");
			}

			//send net param
			TCP_PARAM param;
			memset(&param, 0, sizeof(TCP_PARAM));
			sendFrame(MSG_HANDSHAKE, FRAME_BINARY, (byte*)&param, sizeof(TCP_PARAM));
		EXCEPTION_END
		if (error_code != 0){
			_dead = true;
			return;
		}
	}

	//keepalive status
	bool probeEnabled = false;
	TimeSpan aliveTimeSpan;
	TimeSpan intervalTimeSpan;
	int probeCount(0);

	if (_netParam.keep_alive.enabled) {
		aliveTimeSpan.start();
	}

	_connected = true;
	OnConnected();

	while (!Thread::isQuit()){
		EXCEPTION_BEGIN_ADDR(addr_info)
			Poco::Timespan timeout(1000000);
			if (_socket.poll(timeout, Socket::SELECT_READ)){
					int msgtype;
					int frametype;
					byte* recvbuff(nullptr);
					int nRecv = recvFrameAlloc(&msgtype, &frametype, &recvbuff);
					assert(msgtype == MSG_NORMAL || msgtype == MSG_HEARBEAT);

					if (_netParam.keep_alive.enabled) {
						//recv a frame, disable alive probe
						probeEnabled = false;
						aliveTimeSpan.restart();
					}

					if (msgtype == MSG_NORMAL){
						LogFrame(false, recvbuff, nRecv, frametype);

						OnRecvFrame(recvbuff, nRecv, frametype);
					}
					else if (msgtype == MSG_HEARBEAT) {//alive ack
						utils::LockGuard<utils::Mutex> lock(_sendMutex);
						sendFrame(MSG_HEARBEAT, FRAME_BINARY, nullptr, 0);
					}
			}
			else{//check alive
				if (_netParam.keep_alive.enabled) {
					if (probeEnabled) {
						if (probeCount < _netParam.keep_alive.probe) {
							if (intervalTimeSpan.elapsed() > _netParam.keep_alive.interval) {
								LOG(INFO) << addr_info << " interval timeout, start probe = " << probeCount;

								//new probe alive frame
								{
									utils::LockGuard<utils::Mutex> lock(_sendMutex);
									sendFrame(MSG_HEARBEAT, FRAME_BINARY, nullptr, 0);
								}

								probeCount++;
								intervalTimeSpan.start();
							}
						}
						else {//probe count is reached
							if (intervalTimeSpan.elapsed() > _netParam.keep_alive.interval) {
								//probe failed, disconnected
								LOG(INFO) << addr_info << "disconnected, probe failed = " << probeCount;
								break;
							}
						}
					}
					else {
						if (aliveTimeSpan.elapsed() > _netParam.keep_alive.time) {
							//enable alive probe
							probeEnabled = true;
							probeCount = 0;
							intervalTimeSpan.start();
						}
					}
				}//keep_alive.enabled
			}
		EXCEPTION_END
		if (error_code != 0) {
			if (error_code == SN_NETWORK_DISCONNECTED) {
				break;
			}
			else {
				//error handle
				if (error_code == SN_FRAME_ERROR) {
					EXCEPTION_BEGIN_ADDR(addr_info)
						//read empty buffer
						readEmptyBuffer();
					EXCEPTION_END
				}

				OnError(error_code, error_msg);
			}
		}

		Thread::msleep(10);
	}
	EXCEPTION_BEGIN_ADDR(addr_info)
		close();
	EXCEPTION_END

	_connected = false;
	_dead = true;
	OnDisconnected();

	LOG(INFO) << addr_info << "run end";
}


