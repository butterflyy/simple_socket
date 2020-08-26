#include "whsarmserver.h"
#include "Common.h"
#include "ServerImp.h"
#include "ServerManagerImp.h"
#include "Config.h"
#include <common/ConvertSync.h>
#include <common/ObjectList.h>
#include <common/message_.h>
#ifndef WIN32
#include <signal.h>
#endif

#if defined(WIN32) && defined(_DEBUG)
//#include <vld.h>
#endif

#define BETA_VERSION  0  //beta version for inner test, if is release beta version is 0.
#define RC_VERSION    1  //release candidate version. After beta version test ok.

_SS_BEGIN

//Gloable Server list
ObjectList<ServerManagerImp*>* g_serverManagerList = nullptr;

//Gloable External Variable 
EventManager* EVENT = nullptr;

//Gloable Convert Sync Variable
ConvertSync<std::string, std::string>* SYNC_EVENT = nullptr;

//Gloable dll dir
std::string FLAGS_dll_dir;

_SS_END

//get dll dir
#ifdef WIN32

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:{
							   char path[MAX_PATH];
							   GetModuleFileNameA(hModule, path, MAX_PATH);
							   if (char *ch = strrchr(path, '\\')){
								   ch[0] = 0;
							   }

							   _SS FLAGS_dll_dir = path;
	}
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

#else
_SS_BEGIN

#include <dlfcn.h>
void fun_hs(){
}

std::string GetDllDir(){

	Dl_info info;

	int rc = dladdr((void*)fun_hs, &info);
	char path[255];
	strcpy(path, info.dli_fname);
	if (char *ch = strrchr(path, '/')){
		ch[0] = 0;
	}
	return std::string(path);
}

_SS_END

#endif

_SS_BEGIN
inline bool IsInitialize() {
	if (!g_serverManagerList) {
		LOG(ERROR) << "API not initialized";
		return false;
	}

	return true;
}

//Delete server functional.
struct ServerManagerDeleter{
	void operator()(ServerManagerImp* item){
		if (item){
			item->StopServer();
			delete item;
		}
	}
};

_SS_END

SS_API int WINAPI SS_Initialize(){
	using namespace ss;

	if (g_serverManagerList) {
		LOG(ERROR) << "API Already initialized";
		return SS_ERROR;
	}

	//log init
#if defined(WIN32) || defined(__gnu_linux__)
	if (!FLAGS_glog_init){
		google::InitGoogleLogging("whsarmserver");
		FLAGS_glog_init = true;
		FLAGS_glog_shutdown = false;
	}
	FLAGS_logbuflevel = -1;
#if defined(_DEBUG) || defined(__gnu_linux__)
	FLAGS_alsologtostderr = true;
#endif
#endif
	FLAGS_glog_init = true;

	LOG(INFO) << "SSAPI_VERSION : " << SS_GetLibVersion();

#if defined(__DATE__) && defined(__TIME__)
	LOG(INFO) << "Builded Time: " << __DATE__ << " " << __TIME__;
#endif

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN); //ignore pipe error, when soceket close, but data is send
#endif

	//print log path
#if defined(WIN32) || defined(__gnu_linux__)
	const std::vector<std::string>& logdirs = google::GetLoggingDirectories();
	if (!logdirs.empty()){
		std::string strlogpath = "Default log path : " + logdirs[0];
		utils::OutputDebugLn(strlogpath);
	}
#endif

	//print dll dir
#ifdef __gnu_linux__
	FLAGS_dll_dir = GetDllDir();
#elif __ANDROID__
	FLAGS_dll_dir = "/sdcard";
#endif

	LOG(INFO) << "Dll dir : " << FLAGS_dll_dir;

	std::string config_path = Config::MakePath(FLAGS_dll_dir, "whsarmserver.ini");

	LOG(INFO) << "config path : " << config_path;
	LOG(INFO) << "Check config file existed : " << (utils::CheckFileExist(config_path) ? "Yes" : "No");
	
	//config init
	if (!Config::instance().Init(config_path)){
		LOG(ERROR) << SimpleConfig<NetParam>::instance().errMsg();
	}
	LOG(INFO) << Config::instance().DisplayText();

	EVENT = new EventManager();

	SYNC_EVENT = new ConvertSync<std::string, std::string>();

	g_serverManagerList = new ObjectList<ServerManagerImp*>();

	return SS_SUCCESS;
}

SS_API void WINAPI SS_Finalize(){
	using namespace ss;

	if (!IsInitialize()) {
		return;
	}

	LOG(INFO) << "SS_Finalize";
	
	g_serverManagerList->Clear(ServerManagerDeleter());

	SAFE_DELETE(g_serverManagerList);

	SAFE_DELETE(EVENT);

	SAFE_DELETE(SYNC_EVENT);

	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	if (!FLAGS_glog_shutdown){
		google::ShutdownGoogleLogging();
		FLAGS_glog_shutdown = true;
		FLAGS_glog_init = false;
	}
#endif
}

SS_API const char* WINAPI SS_GetLibVersion(){
	static char str_version[20];
	if (BETA_VERSION){
		sprintf(str_version, "%s-beta%d", SSAPI_VERSION, BETA_VERSION);

		return str_version;
	}
	else if (RC_VERSION){
		sprintf(str_version, "%s-rc%d", SSAPI_VERSION, RC_VERSION);

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
	using namespace ss;

	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_SetCallback";

	EVENT->SetCallback(on_connected, on_disconnected, on_error, on_recvframe);

	return SS_SUCCESS;
}

SS_API int WINAPI SS_StartServer(int port, SS_SERVER* server){
	using namespace ss;

	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_StartServer";

	ServerManagerImp* serverManagerImp = nullptr;

	EXCEPTION_BEGIN
		serverManagerImp = new ServerManagerImp(Config::instance().Data());
		serverManagerImp->StartServer(port);
	EXCEPTION_END

	int ret = ServerManagerImp::TransError(error_code);
	if (ret < 0){
		EXCEPTION_BEGIN
			SAFE_DELETE(serverManagerImp);
		EXCEPTION_END
	}

	*server = serverManagerImp;

	g_serverManagerList->Add(serverManagerImp);

	return ret;
}


SS_API int WINAPI SS_StopServer(SS_SERVER server){
	using namespace ss;

	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_StopServer";

	ServerManagerImp* serverManagerImp = reinterpret_cast<ServerManagerImp*>(server);
	if (!serverManagerImp){
		return SS_INVALID_PARAM;
	}

	EXCEPTION_BEGIN
		serverManagerImp->StopServer();
	EXCEPTION_END

	int ret = ServerManagerImp::TransError(error_code);

	g_serverManagerList->Remove(serverManagerImp);

	return ret;
}

SS_API int WINAPI SS_DisconnectClient(SS_SESSION session){
	using namespace ss;

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
	using namespace ss;

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
SS_API int WINAPI SS_StartServerBindAddr(const char* ip, int port, SS_SERVER* server){
	using namespace ss;

	if (!IsInitialize()) {
		return SS_ERROR;
	}

	LOG(INFO) << "SS_StartServerBindAddr";

	ServerManagerImp* serverManagerImp = nullptr;

	EXCEPTION_BEGIN
		serverManagerImp = new ServerManagerImp(Config::instance().Data());
	serverManagerImp->StartServer(ip, port);
	EXCEPTION_END

		int ret = ServerManagerImp::TransError(error_code);
	if (ret < 0){
		EXCEPTION_BEGIN
			SAFE_DELETE(serverManagerImp);
		EXCEPTION_END
	}

	*server = serverManagerImp;

	return ret;
}

SS_API int WINAPI SS_Helper_SetEvent(const char* id, const char* data){
	using namespace ss;

	if (!SYNC_EVENT || !id || !data){
		return -1;
	}

	return SYNC_EVENT->SetEvent(id, data);
}

SS_API int WINAPI SS_Helper_WaitEvent(const char* id, int timeout, char* data, int len){
	using namespace ss;

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
	using namespace ss;

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
