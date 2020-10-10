#include "whsarmclient.h"
#include "Common.h"
#include "ClientImp.h"
#include "Config.h"
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

_SC_BEGIN

//Global Variable
ObjectList<ClientImp*>* g_clientImpList = nullptr;

//Gloable External Variable 
EventManager* EVENT = nullptr;

//Gloable dll dir
std::string FLAGS_dll_dir;

_SC_END

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

							   __SC FLAGS_dll_dir = path;
	}
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

#else
_SC_BEGIN

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

_SC_END

#endif

_SC_BEGIN

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

_SC_END

SC_API int WINAPI SC_Initialize(){
	using namespace sc;

	if (g_clientImpList) {
		LOG(ERROR) << "API Already initialized";
		return SC_ERROR;
	}

	//log init
#if defined(WIN32) || defined(__gnu_linux__)
	if (!FLAGS_glog_init){
#ifndef ARM_D20
//		google::InitGoogleLogging("whsarmclient");
#endif
		FLAGS_glog_init = true;
		FLAGS_glog_shutdown = false;
	}
	FLAGS_logbuflevel = -1;
#if defined(_DEBUG) || defined(__gnu_linux__)
	FLAGS_alsologtostderr = true;
#endif
#else//android log
	InitLogging();
#endif

	LOG(INFO) << "SCAPI_VERSION : " << SC_GetLibVersion();

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

	std::string config_path = Config::MakePath(FLAGS_dll_dir, "whsarmclient.ini");

	LOG(INFO) << "config path : " << config_path;
	LOG(INFO) << "Check config file existed : " << (utils::CheckFileExist(config_path) ? "Yes" : "No");

	//config init
	if (!Config::instance().Init(config_path)){
		LOG(ERROR) << Config::instance().errMsg();
	}
	LOG(INFO) << Config::instance().DisplayText();

	EVENT = new EventManager();

	g_clientImpList = new ObjectList<ClientImp*>();

	return SC_SUCCESS;
}

SC_API void WINAPI SC_Finalize(){
	using namespace sc;

	if (!IsInitialize()) {
		return;
	}

	LOG(INFO) << "SC_Finalize";

	g_clientImpList->Clear(ClientDeleter());

	SAFE_DELETE(g_clientImpList);

	SAFE_DELETE(EVENT);

	//log shutdown
#if defined(WIN32) || defined(__gnu_linux__)
	if (!FLAGS_glog_shutdown){
		google::ShutdownGoogleLogging();
		FLAGS_glog_shutdown = true;
		FLAGS_glog_init = false;
	}
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
	//case SC_PAYLOAD_TOO_BIG: return "SC_PAYLOAD_TOO_BIG";
	case SC_FRAME_ERROR: return "SC_FRAME_ERROR";
	default: assert(false);  return "**UNKNOWN**";
	}
}

SC_API int WINAPI SC_SetCallback(sc_disconnected_callback on_disconnected,
	sc_error_callback on_error,
	sc_recvframe_callback on_recvframe){
	using namespace sc;

	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_SetCallback";

	EVENT->SetCallback(on_disconnected, on_error, on_recvframe);

	return SC_SUCCESS;
}

SC_API int WINAPI SC_ConnectToHost(const char* ip, int port, SC_CLIENT* client){
	using namespace sc;

	if (!IsInitialize()) {
		return SC_ERROR;
	}

	LOG(INFO) << "SC_ConnectToHost";

	ClientImp* clientImp = nullptr;

	EXCEPTION_BEGIN
		clientImp = new ClientImp();
		clientImp->SetNetParam(Config::instance().Data().net);
		clientImp->SetLogFrameParam(Config::instance().Data().log_frame);
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
	using namespace sc;

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
	using namespace sc;

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
