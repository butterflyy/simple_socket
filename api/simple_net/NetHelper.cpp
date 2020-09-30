#include "NetHelper.h"
#include <typeinfo>

bool FLAGS_glog_init = false;
bool FLAGS_glog_shutdown = false;

POCO_IMPLEMENT_EXCEPTION(SimpleNetException, NetException, "SimpleNet exception")

NetHelper::NetHelper()
:_connected(false), 
_called(true)
{
	memset(&_netParam, 0, sizeof(_netParam));
	memset(&_logFrameParam, 0, sizeof(_logFrameParam));
}

NetHelper::NetHelper(const StreamSocket& socket)
:_socket(socket),
_connected(false),
_called(true)
{
	memset(&_netParam, 0, sizeof(_netParam));
	memset(&_logFrameParam, 0, sizeof(_logFrameParam));
}


NetHelper::~NetHelper()
{
}

void NetHelper::SendFrame(const byte* data, int len, int type){
	utils::LockGuard<utils::Mutex> lock(_sendMutex);

	LogFrame(true, data, len, type);

	sendFrame(MSG_NORMAL, type, data, len);
}


void NetHelper::sendFrame(int msgtype, int frametype, const byte* data, int len){
	assert(msgtype == MSG_NORMAL || msgtype == MSG_HEARBEAT || msgtype == MSG_HANDSHAKE);
	assert(frametype == FRAME_STRING || frametype == FRAME_BINARY);

	TCP_HEADER header;
	memset(&header, 0, sizeof(TCP_HEADER));
	header.flag = FLAG_TCP;
	header.version = PROTOCOL_VERSION;
	header.msgtype = msgtype;
	header.frametype = frametype;
	header.msglen = len;

	sendAll((byte*)&header, sizeof(TCP_HEADER));
	if (data && len > 0){
		sendAll(data, len);
	}
}

void NetHelper::recvFrameHead(PTCP_HEADER header){
	recvAll((byte*)header, sizeof(TCP_HEADER));

	//check msg
	checkHeader(*header);
}

void NetHelper::recvFrameData(PTCP_HEADER header, byte* data, int len){
	assert(len >= header->msglen);
	if (len < header->msglen){
		throw NetException("recv data buffer is to small", SN_NETWORK_ERROR);
	}
	recvAll(data, header->msglen);
}

int NetHelper::recvFrame(int* msgtype, int* frametype, byte* data, int len){
	TCP_HEADER header;
	memset(&header, 0, sizeof(TCP_HEADER));

	recvFrameHead(&header);

	*msgtype = header.msgtype;
	*frametype = header.frametype;

	if (header.msglen > 0) {
		recvFrameData(&header, data, len);
	}

	return header.msglen;
}

int NetHelper::recvFrameAlloc(int* msgtype, int* frametype, byte** data) {
	TCP_HEADER header;
	memset(&header, 0, sizeof(TCP_HEADER));

	recvFrameHead(&header);

	*msgtype = header.msgtype;
	*frametype = header.frametype;

	if (header.msglen > 0) {
		*data = new byte[header.msglen + 1];
		if (!data) {
			throw Poco::OutOfMemoryException();
		}
		recvFrameData(&header, *data, header.msglen);
		(*data)[header.msglen] = 0;//string callback need 0/ end
	}

	return header.msglen;
}

void NetHelper::sendAll(const byte* data, int len){
	int nTotal = 0;
	while (nTotal < len){
		int nSend = _socket.sendBytes(data + nTotal, len - nTotal);
		nTotal += nSend;
	}
}

void NetHelper::recvAll(byte* data, int len){
	int nTotal = 0;
	while (nTotal < len){
		int nRecv = _socket.receiveBytes(data + nTotal, len - nTotal);
		if (nRecv == 0){
			throw SimpleNetException("receive bytes is 0", SN_NETWORK_DISCONNECTED);
		}
		nTotal += nRecv;
	}
}

void NetHelper::checkHeader(const TCP_HEADER& header){
	if (header.flag != FLAG_TCP){
		throw SimpleNetException("header flag error", SN_FRAME_ERROR);
	}

	checkVersionCompatibility(header.version);

	if (header.msgtype != MSG_NORMAL && header.msgtype != MSG_HEARBEAT && header.msgtype != MSG_HANDSHAKE){
		throw SimpleNetException("header msg type error", SN_FRAME_ERROR);
	}

	if (header.frametype != FRAME_STRING && header.frametype != FRAME_BINARY){
		throw SimpleNetException("header data type error", SN_FRAME_ERROR);
	}
}

void NetHelper::checkVersionCompatibility(uint8_t version) {
	if (version != PROTOCOL_VERSION) {
		throw SimpleNetException("header version error", SN_FRAME_ERROR);
	}
}

void NetHelper::readEmptyBuffer(){
	Poco::Timespan timeout(100000);
	char buff[1024];
	while (_socket.poll(timeout, Socket::SELECT_READ)){
		_socket.receiveBytes(buff, 1024);
	}
}
