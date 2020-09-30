#include "Client.h"
#include "NetProtocol.h"


Client::Client()
:NetHelper()
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
			_socket.connect(Poco::Net::SocketAddress(Poco::Net::IPAddress(ip), port), timeoutc);

			//handshake
			sendFrame(MSG_HANDSHAKE, FRAME_BINARY, nullptr, 0);
			Poco::Timespan timeout(10*1000000);
			if (_socket.poll(timeout, Socket::SELECT_READ)){
				byte paramBuff[MAX_TCP_PARAM];
				int msgtype;
				int frametype;
				int msglen = recvFrame(&msgtype, &frametype, paramBuff, MAX_TCP_PARAM);
				if (msgtype != MSG_HANDSHAKE){
					throw SimpleNetException("Recv handshake ack error", SN_NETWORK_ERROR);
				}

				if (msglen < sizeof(TCP_PARAM)){
					throw SimpleNetException("Recv tcp param size error, less than TCP_PARAM", SN_FRAME_ERROR);
				}

				//PTCP_PARAM tcp_param = (PTCP_PARAM)paramBuff;
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
	std::string addr_info;
	{
		EXCEPTION_BEGIN
			addr_info = FormatAddress();
		EXCEPTION_END
	}

	LOG(INFO) << addr_info << "run begin";

	//keepalive status
	bool probeEnabled = false;
	TimeSpan aliveTimeSpan;
	TimeSpan intervalTimeSpan;
	int probeCount(0);

	if (_netParam.keep_alive.enabled) {
		aliveTimeSpan.start();
	}

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
	}
	EXCEPTION_BEGIN_ADDR(addr_info)
		close();
	EXCEPTION_END

	_connected = false;
	OnDisconnected();

	LOG(INFO) << addr_info << "run end";
}


