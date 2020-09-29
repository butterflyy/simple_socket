// test_server.cpp : Defines the entry point for the console application.
//
#include <common\utils.h>
#include <whsarmclient.h>


void CALLBACK disconnected_callback(SC_CLIENT client){
	printf("[client] disconnect \n");
}

void CALLBACK error_callback(SC_CLIENT client, int error_code){
	printf("[client]  error callback, err = %s \n", SC_StrError(error_code));
}

void CALLBACK recvframe_callback(SC_CLIENT client, const unsigned char* data, int len, int type){
	//encode test
	switch (type)
	{
	case SC_FRAME_STRING:
		if (len > 10000){
			printf("[client] recv big string : %d \n", len);
		}
		else{
			printf("[client] recv string : %s \n", data);
		}
		break;
	case SC_FRAME_BINARY:
		printf("[client] recv binary size : %d \n", len);
		break;
	default:
		assert(false);
		break;
	}
}

void main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage : ip port \n");
		return;
	}

	int ret = SC_Initialize();
	if (ret < 0) return;

	do
	{
		ret = SC_SetCallback(disconnected_callback, error_callback, recvframe_callback);
		if (ret < 0) break;

		SC_CLIENT client(nullptr);
		ret = SC_ConnectToHost(argv[1], utils::Stoi(argv[2]), &client);
		if (ret < 0){
			printf("[client]  connect failed, err = %s \n", SC_StrError(ret));
			break;
		}

#if 1
		//test string
		for (int i = 0; i < 10000; i++){
			std::string s = utils::StrFormat("client test data %d£¡", i);
			ret = SC_SendFrame(client, (byte*)s.c_str(), s.size(), SC_FRAME_STRING);
			if (ret < 0) return;
		}

		//test big data
		for (int i = 0; i < 100; i++){
			std::string bigs;
			for (int i = 0; i < 1000000; i++){
				std::string s = utils::StrFormat("test big data-%d£¡", i);
				bigs += s;
			}

			printf("big string size = %d \n", bigs.size());
			ret = SC_SendFrame(client, (byte*)bigs.c_str(), bigs.size(), SC_FRAME_STRING);
			if (ret < 0) break;
		}
#endif

		//utils::Thread::msleep(10000);
		std::string s("paload client data");
		ret = SC_SendFrame(client, (byte*)s.c_str(), s.size(), SC_FRAME_STRING);
		if (ret < 0) return;

	} while (0);

	while (getchar() != 'q'){}

	SC_Finalize();

	system("pause");
}

