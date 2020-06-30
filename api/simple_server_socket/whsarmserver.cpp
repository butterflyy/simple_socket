#include "whsarmserver.h"
#include "ServerImp.h"
#include "ServerManagerImp.h"
#include <common/ConvertSync.h>

#if defined(WIN32) && defined(_DEBUG)
//#include <vld.h>
#endif

#define BETA_VERSION  0  //beta version for inner test, if is release beta version is 0.
#define RC_VERSION      1  //release candidate version. After beta version test ok.

//Global Variable
ServerManagerImp* g_serverManagerImp = nullptr;

//Gloable External Variable 
EventManager* EVENT = nullptr;

//Gloable Convert Sync Variable
ConvertSync<std::string, std::string>* SYNC_EVENT = nullptr;

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

	EVENT = new EventManager();

	SYNC_EVENT = new ConvertSync<std::string, std::string>();

	EXCEPTION_BEGIN
		g_serverManagerImp = new ServerManagerImp();
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}

SS_API void WINAPI SS_Finalize(){
	if (!IsInitialize()) {
		return;
	}

	LOG(INFO) << "SS_Finalize";

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

	SAFE_DELETE(EVENT);

	SAFE_DELETE(SYNC_EVENT);

	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	google::ShutdownGoogleLogging();
#endif
}

SS_API const char* WINAPI SS_GetLibVersion(){
	static char str_version[20];
	if (BETA_VERSION){
		sprintf(str_version, "%s-Beta%d", SSAPI_VERSION, BETA_VERSION);

		return str_version;
	}
	else if (RC_VERSION){
		sprintf(str_version, "%s-RC%d", SSAPI_VERSION, RC_VERSION);

		return str_version;
	}
	else{
		return SSAPI_VERSION;
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

	LOG(INFO) << "SS_SetCallback";

	EVENT->SetCallback(on_connected, on_disconnected, on_error, on_recvframe);

	return SS_SUCCESS;
}

SS_API int WINAPI SS_StartServer(int port){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_StartServer";

	EXCEPTION_BEGIN
		g_serverManagerImp->StartServer(port);
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}


SS_API void WINAPI SS_StopServer(){
	if (!IsInitialize()) {
		return;
	}

	LOG(INFO) << "SS_StopServer";

	EXCEPTION_BEGIN
		g_serverManagerImp->StopServer();
	EXCEPTION_END
}

SS_API int WINAPI SS_DisconnectClient(SS_SESSION session){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_DisconnectClient";

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

	LOG(INFO) << "SS_SendFrame";

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

/**
* 描述： 启动服务器监听服务。
* 参数： ip 需要绑定的网卡IP地址。
* 参数： port 服务器端口号。
* 返回： 0 成功，其他值失败，参考 ss_error_code。
*/
SS_API int WINAPI SS_StartServerBindAddr(const char* ip, int port){
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_StartServerBindAddr";

	EXCEPTION_BEGIN
		g_serverManagerImp->StartServer(ip, port);
	EXCEPTION_END

	return ServerManagerImp::TransError(error_code);
}

SS_API int WINAPI SS_Helper_SetEvent(const char* id, const char* data){
	if (!SYNC_EVENT || !id || !data){
		return -1;
	}

	return SYNC_EVENT->SetEvent(id, data);
}

SS_API int WINAPI SS_Helper_WaitEvent(const char* id, int timeout, char* data, int len){
	if (!SYNC_EVENT || !id || !data){
		return -1;
	}

	std::string data_buff;
	int ret = SYNC_EVENT->WaitEvent(id, &data_buff, timeout);
	if (ret == 0){//successed
		utils::SafeStrCpy(data, data_buff.c_str(), len);
	}

	return ret;
}

SS_API int WINAPI SS_Helper_Log(int level, const char* info) {
	if (!IsInitialize()) {
		return SS_ERROR;
	}

	if (!info){
		return SS_INVALID_PARAM;
	}

	switch (level){
	case 0:
		LOG(INFO) << info;
		break;
	case 1:
		LOG(WARNING) << info;
		break;
	case 2:
		LOG(ERROR) << info;
		break;
	case 3:
		LOG(FATAL) << info;
		break;
	default:
		LOG(INFO) << info;
		break;
	}

	return SS_SUCCESS;
}