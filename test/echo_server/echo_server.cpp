// test_server.cpp : Defines the entry point for the console application.
//
#include <map>
#include <common/utils.h>
#include <whsarmserver.h>

#define PORT  49877

std::map<SS_SESSION, std::string> ss;
utils::Mutex ss_mutex;

void CALLBACK connected_callback(SS_SESSION session, const char* client_ip){
	printf("[server] new client connect: %s \n", client_ip);
	utils::LockGuard<utils::Mutex> lock(ss_mutex);
	ss[session] = client_ip;
}

void CALLBACK disconnected_callback(SS_SESSION session){
	printf("[server] disconnect : %s \n", ss[session].c_str());
	utils::LockGuard<utils::Mutex> lock(ss_mutex);
	ss.erase(session);
}

void CALLBACK error_callback(SS_SESSION session, int error_code){
	printf("[server] client %s error: %d \n", ss[session].c_str(), error_code);
}

void CALLBACK recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type){
	//encode test
	ss_mutex.lock();
	std::string ip = ss[session];
	ss_mutex.unlock();

	switch (type)
	{
	case SS_FRAME_STRING:
		if (len > 10000){
			printf("[server] %s recv big string : %d \n", ip.c_str(), len);
		}
		else{
			printf("[server] %s recv string : %s \n", ip.c_str(), data);
		}
		break;
	case SS_FRAME_BINARY:
		printf("[server] %s recv binary size : %d \n", ip.c_str(), len);
		break;
	default:
		assert(false);
		break;
	}
	int ret = SS_SendFrame(session, data, len, type);
	if (ret < 0){
		printf("[server] %s send frame error, code = %d \n", ip.c_str(), ret);
	}
}


//linux exception
void dump(void)
{
	char szLog[MAX_LOG_LEN] = { 0 };
	int j, nptrs;
	const int BACKTRACE_SIZE = 16;
	void *buffer[BACKTRACE_SIZE];
	char **strings;


	nptrs = backtrace(buffer, BACKTRACE_SIZE);


	snprintf(szLog, MAX_LOG_LEN, "backtrace() returned %d addresses", nptrs);
	CLogError(szLog);
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}


	for (j = 0; j < nptrs; j++)
	{
		snprintf(szLog, MAX_LOG_LEN, "  [%02d] %s", j, strings[j]);
		CLogError(szLog);
	}



	free(strings);
	char buff[128] = { 0x00 };
	snprintf(buff, 128, "cat /proc/%d/maps", getpid());
	CLogError(buff);
	system((const char*)buff);
}


void signal_handler(int signo)
{
	signal(SIGSEGV, signal_handler);

	signal(SIGABRT, signal_handler);

	char szLog[MAX_LOG_LEN] = { 0 };
	snprintf(szLog, MAX_LOG_LEN, "\n=========>>>catch signal %d <<<=========\n", signo);
	CLogError(szLog);


	CLogError("Dump stack start...\n");
	dump();
	CLogError("Dump stack end...\n");


	signal(signo, SIG_DFL);
	raise(signo);

}

int main()
{
	int ret = SS_Initialize();
	if (ret < 0) return 0;

	ret = SS_SetCallback(connected_callback, disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0) return 0;

	ret = SS_StartServer(PORT);
	if (ret < 0) return 0;

	while (getchar() != 'q'){}

	SS_StopServer();

	SS_Finalize();
	
	system("pause");
	return 0;
}

