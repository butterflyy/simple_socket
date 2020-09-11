#include "NetHelper.h"
#include <typeinfo>

bool FLAGS_glog_init = false;
bool FLAGS_glog_shutdown = false;

POCO_IMPLEMENT_EXCEPTION(SimpleNetException, NetException, "SimpleNet exception")

NetHelper::NetHelper()
:_connected(false), 
_recvlen(0),
_recvbuff(nullptr),
_called(true)
{
	memset(&_netParam, 0, sizeof(_netParam));
	memset(&_logFrameParam, 0, sizeof(_logFrameParam));
}

NetHelper::NetHelper(const StreamSocket& socket)
:_socket(socket),
_connected(false),
_recvlen(0),
_recvbuff(nullptr),
_called(true)
{
	memset(&_netParam, 0, sizeof(_netParam));
	memset(&_logFrameParam, 0, sizeof(_logFrameParam));
}


NetHelper::~NetHelper()
{
	SAFE_DELETE_ARRAY(_recvbuff)
}

void NetHelper::SendFrame(const byte* data, int len, int type){
	utils::LockGuard<utils::Mutex> lock(_sendMutex);

	LogFrame(true, data, len, type);

	sendFrame(MSG_NORMAL, type, data, len);

	_sendSpan.restart();
}


void NetHelper::sendFrame(int msgtype, int frametype, const byte* data, int len){
	assert(msgtype == MSG_NORMAL || msgtype == MSG_HEARBEAT || msgtype == MSG_HANDSHAKE);
	assert(frametype == FRAME_STRING || frametype == FRAME_BINARY);

	TCP_HEADER header;
	memset(&header, 0, sizeof(TCP_HEADER));
	header.msgtype = msgtype;
	header.frametype = frametype;
	strcpy(header.flag, TCP_FLAG);
	header.msglen = len;

	sendAll((byte*)&header, sizeof(TCP_HEADER));
	if (data && len > 0){
		sendAll(data, len);
	}
}

int NetHelper::recvFrame(int* msgtype, int* frametype, byte* data, int len){
	TCP_HEADER header;
	memset(&header, 0, sizeof(TCP_HEADER));

	recvAll((byte*)&header, sizeof(TCP_HEADER));

	//check msg
	checkHeader(header);

	if (len < header.msglen){
		throw SimpleNetException(SN_PAYLOAD_TOO_BIG);
	}

	*msgtype = header.msgtype;
	*frametype = header.frametype;

	if (data && len > 0 && header.msglen > 0){
		//recv msg data
		recvAll(data, header.msglen);
		data[header.msglen] = 0;
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
			throw SimpleNetException(SN_NETWORK_DISCONNECTED);
		}
		nTotal += nRecv;
	}
}

void NetHelper::checkHeader(const _TCP_HEADER& header){
	if (strcmp(header.flag, TCP_FLAG)){
		throw SimpleNetException("flag error", SN_FRAME_ERROR);
	}
	if (header.msgtype != MSG_NORMAL && header.msgtype != MSG_HEARBEAT && header.msgtype != MSG_HANDSHAKE){
		throw SimpleNetException("msg type error", SN_FRAME_ERROR);
	}
	if (header.frametype != FRAME_STRING && header.frametype != FRAME_BINARY){
		throw SimpleNetException("data type error", SN_FRAME_ERROR);
	}
}

void NetHelper::readEmptyBuffer(byte* data, int len){
	Poco::Timespan timeout(100000);
	while (_socket.poll(timeout, Socket::SELECT_READ)){
		_socket.receiveBytes(data, len);
	}
}
