#pragma once
#include "NetConfig.h"
#include "NetProtocol.h"
#include "NetParam.h"


POCO_DECLARE_EXCEPTION(, SimpleNetException, NetException)

//handle exception begin
#define EXCEPTION_BEGIN_ADDR(addr)                 \
	int error_code(0);                             \
	std::string error_msg;                         \
	std::string addr_(addr);                   \
try{                                               \

#define EXCEPTION_BEGIN EXCEPTION_BEGIN_ADDR("")

//handle exception end
#define EXCEPTION_END                              \
}                                                  \
catch (Poco::Net::ConnectionResetException& e){    \
	error_code = SN_NETWORK_DISCONNECTED;          \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::Net::ConnectionAbortedException& e){  \
	error_code = SN_NETWORK_DISCONNECTED;          \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::TimeoutException& e){                 \
    error_code = SN_NETWORK_TIMEOUT;               \
    error_msg = e.displayText();                   \
}                                                  \
catch (SimpleNetException& e){                     \
	error_code = e.code();                         \
	error_msg = e.displayText();                   \
}                                                  \
catch (Poco::Exception& e){                        \
	error_code = SN_NETWORK_ERROR;                 \
	error_msg = e.displayText();                   \
}                                                  \
catch (...){                                       \
	error_code = SN_NETWORK_ERROR;                 \
	error_msg = "Unknow exception";                \
}                                                  \
if (error_code != 0){                              \
	LOG(ERROR) << addr_                            \
		<< "error code : " <<                      \
		NetHelper::StrError(error_code)            \
		<< "  error msg : " << error_msg;          \
}


enum NetError
{
	SN_NETWORK_ERROR = 1000,
	SN_NETWORK_DISCONNECTED,
	SN_NETWORK_TIMEOUT,
	SN_PAYLOAD_TOO_BIG,
	SN_FRAME_ERROR,
};

struct _TCP_HEADER;
class NetHelper
{
public:
	//if recv buff is small, cause PayloadTooBigException
	NetHelper(const NetParam& netParam);

	//if recv buff is small, cause PayloadTooBigException
	NetHelper(const StreamSocket& socket, const NetParam& netParam);
	virtual ~NetHelper();

	//event
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnError(int error_code, const std::string& error_msg) = 0;
	virtual void OnRecvFrame(const byte* data, int len, int type) = 0;

	//send string
	void SendFrameString(const std::string& data);

	//send binary
	void SendFrameBinary(const byte* data, int len);

	//send string or binary
	void SendFrame(const byte* data, int len, int type);

	//return msglen
	//int RecvFrame(byte* data, int len, int* type);

	//local address
	std::string Address();

	//local port
	int Port();

	//remote address
	std::string RemoteAddress();

	//remote port
	int RemotePort();

	std::string FormatAddress();
	
	bool IsConnected() const;

	bool IsCalled() const;

	static const char* StrError(int err);

	//log frame data
	void LogFrame(bool send, const byte* data, int len, int type);

protected:
	void close();
	void sendFrame(int msgtype, int frametype, const byte* data, int len);
	//return msglen
	int recvFrame(int* msgtype, int* frametype, byte* data, int len);

	void sendAll(const byte* data, int len);
	void recvAll(byte* data, int len);
	void checkHeader(const _TCP_HEADER& header);
	void readEmptyBuffer();
protected:
	StreamSocket _socket;
	utils::Mutex _sendMutex;

	NetParam _netParam;

	bool _connected;

	int _recvlen;
	byte* _recvbuff;

	TimeSpan _sendSpan;
	TimeSpan _recvSpan;

	bool _called; //if event called.
};

inline void NetHelper::SendFrameString(const std::string& data){
	return SendFrame((byte*)data.c_str(), data.length(), FRAME_STRING);
}

inline void NetHelper::SendFrameBinary(const byte* data, int len){
	return SendFrame(data, len, FRAME_BINARY);
}

inline std::string NetHelper::Address(){
	return _socket.address().host().toString();
}

inline int NetHelper::Port(){
	return _socket.address().port();
}

inline std::string NetHelper::RemoteAddress(){
	return _socket.peerAddress().host().toString();
}

inline int NetHelper::RemotePort(){
	return _socket.peerAddress().port();
}

inline std::string NetHelper::FormatAddress(){
	return "[Local_" + _socket.address().toString() +
		" Remote_" + _socket.peerAddress().toString() + "] ";
}

inline bool NetHelper::IsConnected() const{
	return _connected;
}

inline bool NetHelper::IsCalled() const{
	return _called;
}

inline const char* NetHelper::StrError(int err){
	switch (err)
	{
	case SN_NETWORK_ERROR: return "SN_NETWORK_ERROR";
	case SN_NETWORK_DISCONNECTED: return "SN_NETWORK_DISCONNECTED";
	case SN_NETWORK_TIMEOUT: return "SN_NETWORK_TIMEOUT";
	case SN_PAYLOAD_TOO_BIG: return "SN_PAYLOAD_TOO_BIG";
	case SN_FRAME_ERROR: return "SN_FRAME_ERROR";
	default: assert(false); return "**SN_UNKNOW**";
	}
}

inline void NetHelper::close(){
	try{
		_socket.shutdown();
	}
	catch (Poco::Exception& e){
		LOG(WARNING) << "shutdown exception: " << e.displayText();
	}
	_socket.close();
}

inline void NetHelper::LogFrame(bool send, const byte* data, int len, int type){
	if (_netParam.log_frame.is_log){
		std::string str;
		
		if (type == FRAME_BINARY){
			if (_netParam.log_frame.is_log_binary){
				str = utils::HexFormat(data, (len * 2) > _netParam.log_frame.max_log_size ?
					(_netParam.log_frame.max_log_size / 2) : len);
				if ((len * 2) > _netParam.log_frame.max_log_size) str += "...";
			}
		}
		else{
			str = std::string((char*)data, len > _netParam.log_frame.max_log_size ? 
				_netParam.log_frame.max_log_size : len);
			if (len > _netParam.log_frame.max_log_size) str += "...";
		}

		LOG(INFO) << FormatAddress() << (send ? "Send frame: " : "Recv frame: ") << " type:" << 
			(type == FRAME_BINARY ? "BINARY" : "STRING") << " len: " << len << " data: " << str;
	}
}




