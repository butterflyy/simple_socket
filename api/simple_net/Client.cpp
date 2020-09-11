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
			Poco::Timespan timeout(2000000);
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

				PTCP_PARAM tcp_param = (PTCP_PARAM)paramBuff;
				_netParam.recv_buff_size = tcp_param->recv_buff_size;
				_netParam.keep_alive.heatbeat_time = tcp_param->heatbeat_time;
				_netParam.keep_alive.keepalive_count = tcp_param->keepalive_count;
				_netParam.keep_alive.keepalive_time = tcp_param->keepalive_time;

				LOG(INFO) << "recv_buff_size=" << _netParam.recv_buff_size
					<< " heatbeat_time=" << _netParam.keep_alive.heatbeat_time
					<< " keepalive_count=" << _netParam.keep_alive.keepalive_count
					<< " keepalive_time=" << _netParam.keep_alive.keepalive_time;

				//create recv buffer
				createRecvBuffer();
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

	_sendSpan.start();
	_recvSpan.start();
	long noalive_times = 0;

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
							readEmptyBuffer(_recvbuff, _recvlen);
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
	OnDisconnected();

	LOG(INFO) << addr_info << "run end";
}


