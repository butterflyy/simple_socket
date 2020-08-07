#include "whsarmclient.h"
#include "ClientImp.h"
#include <common/ObjectList.h>
#ifndef WIN32
#include <signal.h>
#endif

#if defined(WIN32) && defined(_DEBUG)
//#include <vld.h>
#endif

#define BETA_VERSION  0  //beta version for inner test, if is release beta version is 0.
#define RC_VERSION    1  //release candidate version. After beta version test ok.
 
//Global Variable
ObjectList<ClientImp*>* g_clientImpList = nullptr;

//Gloable External Variable 
EventManager* EVENT = nullptr;

//Delete server functional.
struct ClientDeleter{
	void operator()(ClientImp* item){
		if (item){
			item->Disconnect();
			delete item;
		}
	}
};

inline bool IsInitialize() {
	if (!g_clientImpList) {
		LOG(ERROR) << "API not initialized";
		return false;
	}

	return true;
}

SC_API int WINAPI SC_Initialize(){
	if (g_clientImpList) {
		LOG(ERROR) << "API Already initialized";
		return SC_ERROR;
	}

	//log init
#if defined(WIN32) || defined(__gnu_linux__)
	google::InitGoogleLogging("simpleclientsocket");
	FLAGS_logbuflevel = -1;
#if defined(_DEBUG) || defined(__gnu_linux__)
	FLAGS_alsologtostderr = true;
#endif
#else//android log
	InitLogging();
#endif

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN); //ignore pipe error, when soceket close, but data is send
#endif

	LOG(INFO) << "SSAPI_VERSION : " << SC_GetLibVersion();

#if defined(__DATE__) && defined(__TIME__)
	LOG(INFO) << "Builded Time: " << __DATE__ << " " << __TIME__;
#endif

	EVENT = new EventManager();

	g_clientImpList = new ObjectList<ClientImp*>();

	return SC_SUCCESS;
}

SC_API void WINAPI SC_Finalize(){
	if (!IsInitialize()) {
		return;
	}

	LOG(INFO) << "SC_Finalize";

	g_clientImpList->Clear(ClientDeleter());

	SAFE_DELETE(g_clientImpList);

	SAFE_DELETE(EVENT);

	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	google::ShutdownGoogleLogging();
#else
	ShutdownLogging();
#endif
}

SC_API const char* WINAPI SC_GetLibVersion(){
	static char str_version[20];
	if (BETA_VERSION){
		sprintf(str_version, "%s-beta%d", SCAPI_VERSION, BETA_VERSION);

		return str_version;
	}
	else if (RC_VERSION){
		sprintf(str_version, "%s-rc%d", SCAPI_VERSION, RC_VERSION);

		return str_version;
	}
	else{
		return SCAPI_VERSION;
	}
}

SC_API const char* WINAPI SC_StrError(int error_code){
	switch (error_code)
	{
	case SC_SUCCESS: return "SC_SUCCESS";
	case SC_ERROR: return "SC_ERROR";
	case SC_INVALID_PARAM: return "SC_INVALID_PARAM";
	case SC_NETWORK_ERROR: return "SC_NETWORK_ERROR";
	case SC_NETWORK_DISCONNECTED: return "SC_NETWORK_DISCONNECTED";
	case SC_NETWORK_TIMEOUT: return "SC_NETWORK_TIMEOUT";
	case SC_PAYLOAD_TOO_BIG: return "SC_PAYLOAD_TOO_BIG";
	case SC_FRAME_ERROR: return "SC_FRAME_ERROR";
	default: assert(false);  return "**UNKNOWN**";
	}
}

SC_API int WINAPI SC_SetCallback(sc_disconnected_callback on_disconnected,
	sc_error_callback on_error,
	sc_recvframe_callback on_recvframe){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_SetCallback";

	EVENT->SetCallback(on_disconnected, on_error, on_recvframe);

	return SC_SUCCESS;
}

SC_API int WINAPI SC_ConnectToHost(const char* ip, int port, SC_CLIENT* client){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_ConnectToHost";

	ClientImp* clientImp = nullptr;

	EXCEPTION_BEGIN
		clientImp = new ClientImp();
		clientImp->Connect(ip, port);
	EXCEPTION_END

	int ret = ClientImp::TransError(error_code);
	if (ret < 0){
		EXCEPTION_BEGIN
			SAFE_DELETE(clientImp);
		EXCEPTION_END
	}

	*client = clientImp;

	g_clientImpList->Add(clientImp);
	return ret;
}

SC_API int WINAPI SC_DisconnectFromHost(SC_CLIENT client){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_DisconnectFromHost";

	ClientImp* clientImp = reinterpret_cast<ClientImp*>(client);
	if (!clientImp){
		return SC_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		clientImp->Disconnect();
	EXCEPTION_END

	int ret = ClientImp::TransError(error_code);

	g_clientImpList->Remove(clientImp);

	return ret;
}

SC_API int WINAPI SC_SendFrame(SC_CLIENT client, const unsigned char* data, int len, int type){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_SendFrame";

	if (!data || len <= 0 || (type != SC_FRAME_STRING && type != SC_FRAME_BINARY)){
		return SC_INVALID_PARAM;
	}

	ClientImp* clientImp = reinterpret_cast<ClientImp*>(client);
	if (!clientImp){
		return SC_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		clientImp->SendFrame(data, len, type);
	EXCEPTION_END

	return ClientImp::TransError(error_code);
}