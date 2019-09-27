#include "whsarmclient.h"
#include "ClientImp.h"

#define BETA_VERSION  0  //beta version for inner test, if is release beta version is 0.

//Global Variable
ClientImp* g_clientImp = nullptr;

inline bool IsInitialize() {
	if (!g_clientImp) {
		LOG(ERROR) << "API not initialized";
		return false;
	}

	return true;
}

SC_API int WINAPI SC_Initialize(){
	if (g_clientImp) {
		LOG(ERROR) << "API Already initialized";
		return SC_ERROR;
	}

	//log init
#if defined(WIN32)
	google::InitGoogleLogging("simpleclientsocket");
#endif

#if defined(WIN32) || defined(__gnu_linux__)
	FLAGS_logbuflevel = -1;
	FLAGS_alsologtostderr = true;
#endif

	LOG(INFO) << "SSAPI_VERSION : " << SC_GetLibVersion();

#if defined(__DATE__) && defined(__TIME__)
	LOG(INFO) << "Builded Time: " << __DATE__ << " " << __TIME__;
#endif

	EXCEPTION_BEGIN
		g_clientImp = new ClientImp();
	EXCEPTION_END

	return ClientImp::TransError(error_code);
}

SC_API void WINAPI SC_Finalize(){
	if (!IsInitialize()) {
		return;
	}

	{
		EXCEPTION_BEGIN
			g_clientImp->Disconnect();
		EXCEPTION_END
	}

	{
		EXCEPTION_BEGIN
			SAFE_DELETE(g_clientImp);
		EXCEPTION_END
	}

	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	google::ShutdownGoogleLogging();
#endif
}

SC_API const char* WINAPI SC_GetLibVersion(){
	if (BETA_VERSION == 0){//release version
		return SCAPI_VERSION;
	}
	else{//beta version
		static char str_version[20];
		sprintf(str_version, "%s-Beta%d", SCAPI_VERSION, BETA_VERSION);
		return str_version;
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

	g_clientImp->SetCallback(on_disconnected, on_error, on_recvframe);

	return SC_SUCCESS;
}

SC_API int WINAPI SC_ConnectToHost(const char* ip, int port){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	EXCEPTION_BEGIN
		g_clientImp->Connect(ip, port);
	EXCEPTION_END

	return ClientImp::TransError(error_code);
}

SC_API void WINAPI SC_DisconnectFromHost(){
	if (!IsInitialize()) {
		return;
	}

	EXCEPTION_BEGIN
		g_clientImp->Disconnect();
	EXCEPTION_END
}

SC_API int WINAPI SC_SendFrame(const unsigned char* data, int len, int type){
	if (!IsInitialize()) {
		return SC_ERROR;
	}

	if (!data || len <= 0 || (type != SC_FRAME_STRING && type != SC_FRAME_BINARY)){
		return SC_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		g_clientImp->SendFrame(data, len, type);
	EXCEPTION_END

	return ClientImp::TransError(error_code);
}