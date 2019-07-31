#include "whsarmserver.h"
#include "ServerImp.h"
#include "ServerManagerImp.h"

#define BETA_VERSION  1  //beta version for inner test, if is release beta version is 0.

//Global Variable
ServerManagerImp* g_serverManagerImp = nullptr;

inline bool IsInitialize() {
	if (!g_serverManagerImp) {
		LOG(ERROR) << "API not initialized";
		return false;
	}

	return true;
}

SS_API int WINAPI SS_Initialize(){
	if (g_serverManagerImp) {
		LOG(ERROR) << "API Already initialized";
		return SS_ERROR;
	}

	//log init
#if defined(WIN32) || defined(__gnu_linux__)
	google::InitGoogleLogging("simpleserversocket");
	FLAGS_logbuflevel = -1;
	FLAGS_alsologtostderr = true;
#endif

	LOG(INFO) << "SSAPI_VERSION : " << SS_GetLibVersion();

#if defined(__DATE__) && defined(__TIME__)
	LOG(INFO) << "Builded Time: " << __DATE__ << " " << __TIME__;
#endif

	EXCEPTION_BEGIN
		g_serverManagerImp = new ServerManagerImp();
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}

SS_API void WINAPI SS_Finalize(){
	if (!IsInitialize()) {
		return;
	}

	{
		EXCEPTION_BEGIN
			g_serverManagerImp->StopServer();
		EXCEPTION_END
	}

	{
		EXCEPTION_BEGIN
			SAFE_DELETE(g_serverManagerImp);
		EXCEPTION_END
	}


	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	google::ShutdownGoogleLogging();
#endif
}

SS_API const char* WINAPI SS_GetLibVersion(){
	if (BETA_VERSION == 0){//release version
		return SSAPI_VERSION;
	}
	else{//beta version
		static char str_version[20];
		sprintf(str_version, "%s-Beta%d", SSAPI_VERSION, BETA_VERSION);
		return str_version;
	}
}

SS_API const char* WINAPI SS_StrError(int error_code){
	switch (error_code)
	{
	case SS_SUCCESS: return "SS_SUCCESS";
	case SS_ERROR: return "SS_ERROR";
	case SS_INVALID_PARAM: return "SS_INVALID_PARAM";
	case SS_NETWORK_ERROR: return "SS_NETWORK_ERROR";
	case SS_NETWORK_DISCONNECTED: return "SS_NETWORK_DISCONNECTED";
	case SS_NETWORK_TIMEOUT: return "SS_NETWORK_TIMEOUT";
	case SS_PAYLOAD_TOO_BIG: return "SS_PAYLOAD_TOO_BIG";
	case SS_FRAME_ERROR: return "SS_FRAME_ERROR";
	default: assert(false);  return "**UNKNOWN**";
	}
}

SS_API int WINAPI SS_SetCallback(ss_connected_callback on_connected,
	ss_disconnected_callback on_disconnected,
	ss_error_callback on_error,
	ss_recvframe_callback on_recvframe){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	g_serverManagerImp->SetCallback(on_connected, on_disconnected, on_error, on_recvframe);

	return SS_SUCCESS;
}

SS_API int WINAPI SS_StartServer(int port){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	EXCEPTION_BEGIN
		g_serverManagerImp->StartServer(port);
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}


SS_API void WINAPI SS_StopServer(){
	if (!IsInitialize()) {
		return;
	}


	EXCEPTION_BEGIN
		g_serverManagerImp->StopServer();
	EXCEPTION_END
}

SS_API int WINAPI SS_DisconnectClient(SS_SESSION session){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	ServerImp* serverImp = reinterpret_cast<ServerImp*>(session);
	if (!serverImp){
		return SS_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		serverImp->Disconnect();
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}


SS_API int WINAPI SS_SendFrame(SS_SESSION session, const unsigned char* data, int len, int type){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	if (!data || len <= 0 || (type != SS_FRAME_STRING && type != SS_FRAME_BINARY)){
		return SS_INVALID_PARAM;
	}

	ServerImp* serverImp = reinterpret_cast<ServerImp*>(session);
	if (!serverImp){
		return SS_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		serverImp->SendFrame(data, len, type);
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}