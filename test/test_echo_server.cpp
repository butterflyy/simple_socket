// test_server.cpp : Defines the entry point for the console application.
//
#include <common/utils.h>
#include <common/ConvertSync.h>
#include <common/TimeSpan.h>
#include <whsarmclient.h>


ConvertSync<std::string, byte*> SYNC_EVENT;

void CALLBACK disconnected_callback(SC_CLIENT client){
	printf("[client] disconnect \n");
}

void CALLBACK error_callback(SC_CLIENT client, int error_code){
	printf("[client]  error callback, err = %s \n", SC_StrError(error_code));
}

void CALLBACK recvframe_callback(SC_CLIENT client, const unsigned char* data, int len, int type){
	printf("recv frame size = %d \n", type);
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage : ip port \n");
		return 0;
	}

	int ret = SC_Initialize();
	if (ret < 0) return 0;


	ret = SC_SetCallback(disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0) return 0;

	for(int i = 0; i < 500; i++){
		printf("connect size = %d \n", i);
		SC_CLIENT client(nullptr);
		ret = SC_ConnectToHost(argv[1], utils::Stoi(argv[2]), &client);
		if (ret < 0){
			printf("[client]  connect failed, err = %s \n", SC_StrError(ret));
			return 0;
		}	

		assert(client);
		std::string testmsg("asdfasdfasdf");
		ret = SC_SendFrame(client, (unsigned char*)testmsg.c_str(), testmsg.size(), SC_FRAME_STRING);
		if (ret < 0) {
			printf("[ERROR]  send frame failed, err = %s \n", SC_StrError(ret));
			break;
		}	
	}

	getchar();

	SC_Finalize();

	return 0;
}

