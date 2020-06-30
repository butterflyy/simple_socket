// test_server.cpp : Defines the entry point for the console application.
//
#include <common\utils.h>
#include <common\ConvertSync.h>
#include <common\TimeSpan.h>
#include <whsarmclient.h>

#define IP    "111.229.4.210"
//#define IP    "127.0.0.1"

#define PORT  49877

#define FRAME_ID_SIZE   8

ConvertSync<std::string, byte*> SYNC_EVENT;

void CALLBACK disconnected_callback(){
	printf("[client] disconnect \n");
}

void CALLBACK error_callback(int error_code){
	printf("[client]  error callback, err = %s \n", SC_StrError(error_code));
}

void CALLBACK recvframe_callback(const unsigned char* data, int len, int type){
	assert(type == SC_FRAME_BINARY);
	assert(len >= FRAME_ID_SIZE);
	std::string id((char*)data, FRAME_ID_SIZE);

	byte* retdata = new byte[len];
	memcpy(retdata, data, len); 
	SYNC_EVENT.SetEvent(id, retdata);
}

void main(int argc, char* argv[])
{
	int ret = SC_Initialize();
	if (ret < 0) return;


	ret = SC_SetCallback(disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0) return;

	ret = SC_ConnectToHost(IP, PORT);
	if (ret < 0){
		printf("[client]  connect failed, err = %s \n", SC_StrError(ret));
		return;
	}


	while (true)
	{
		//build data
		int bufflen = rand() % 1024 * 1024;
		if (bufflen < FRAME_ID_SIZE) return;
		utils::auto_array_ptr<byte> buffer(new byte[bufflen]);
		std::string id = utils::StrFormat("%08d", clock());
		assert(id.size() == FRAME_ID_SIZE);
		memcpy(buffer.get(), id.c_str(), id.size());

		for (int i = FRAME_ID_SIZE; i < bufflen; i++){
			buffer[i] = rand() & 0xFF;
		}

		ret = SC_SendFrame(buffer.get(), bufflen, SC_FRAME_BINARY);
		if (ret < 0) {
			printf("[ERROR]  send frame failed, err = %s \n", SC_StrError(ret));
			break;
		}

		TimeSpan span;
		span.start();
		//waiting return
		byte* retdata = NULL;
		ret = SYNC_EVENT.WaitEvent(id, &retdata, 100*1000);
		if (ret != 0){
			printf("[ERROR] Wait return faild, ret = %d, buflen = %d \n", ret, bufflen);
		}
		else{
			float speed = bufflen * 1000 / (span.elapsed()*1024.0);
			printf("Recv speed %.2f kb/s \n", speed);

			//check data if same
			if (memcmp(buffer.get(), retdata, bufflen)){
				printf("[ERROR] diffent return data \n");
				break;
			}
		}
	}


	SC_Finalize();

	system("pause");
}

