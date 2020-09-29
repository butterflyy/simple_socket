// UnitTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <gtest/gtest.h>
#include <whsarmserver.h>
#include <whsarmclient.h>
#include <common/utils.h> //for msleep
#include <commonex/convert_sync.h>

//指定测试选项
// --gtest_filter=TestServerClient.BigFrame

struct KeepAlive {
	long heatbeat_time;
	long keepalive_time;
	long keepalive_count;
};

struct NetParam {
	long recv_buff_size;
	KeepAlive keep_alive;
};

SS_API int WINAPI SS_GetNetParam(struct NetParam* param);
SS_API int WINAPI SS_SetNetParam(const struct NetParam* param);

namespace {
	class TestServerClient : public testing::Test {
	public:
		// Sets up the test fixture.
		void SetUp() override{
			//initialize server
			int ret = SS_Initialize();
			EXPECT_EQ(ret, SS_SUCCESS);
			ret = SS_SetCallback(ss_connected_callback, ss_disconnected_callback, ss_error_callback, ss_recvframe_callback);
			EXPECT_EQ(ret, SS_SUCCESS);

			//initialize client
			ret = SC_Initialize();
			EXPECT_EQ(ret, SS_SUCCESS);
			ret = SC_SetCallback(sc_disconnected_callback, sc_error_callback, sc_recvframe_callback);
			EXPECT_EQ(ret, SS_SUCCESS);
		}

		// Tears down the test fixture.
		void TearDown() override {
			SC_Finalize();
			SS_Finalize();
		}

		static void BuildRandomData(utils::Buffer& buff) {
			for (size_t i = 0; i < buff.size(); i++) {
				buff[i] = rand() & 0xFF;
			}
		}

		struct ss_connected_callback_data {
			SS_SERVER server;
			SS_SESSION session;
			std::string client_ip;
			int client_port;
		};

		struct ss_disconnected_callback_data {
			SS_SESSION session;
		};

		struct ss_error_callback_data {
			SS_SESSION session;
			int error_code;
		};

		struct ss_recvframe_callback_data {
			SS_SESSION session;
			unsigned char* data;
			int len;
			int type;
		};

		struct sc_disconnected_callback_data {
			SC_CLIENT client;
		};

		struct sc_error_callback_data {
			SC_CLIENT client;
			int error_code;
		};

		struct sc_recvframe_callback_data {
			SC_CLIENT client;
			unsigned char* data;
			int len;
			int type;
		};

		//server callback


		static void CALLBACK  ss_connected_callback(SS_SERVER server, SS_SESSION session, const char* client_ip, int client_port) {
			ss_connected_callback_data* data = new ss_connected_callback_data();
			data->server = server;
			data->session = session;
			data->client_ip = client_ip;
			data->client_port = client_port;

			bool ret = Sync.set("ss_connected_callback", data);
			EXPECT_EQ(ret, true);
		}
												 
		static void CALLBACK  ss_disconnected_callback(SS_SESSION session) {
			ss_disconnected_callback_data* data = new ss_disconnected_callback_data();
			data->session = session;

			bool ret = Sync.set("ss_disconnected_callback", data);
			//EXPECT_EQ(ret, 0);
		}
												 
		static void CALLBACK  ss_error_callback(SS_SESSION session, int error_code) {
			ss_error_callback_data* data = new ss_error_callback_data();
			data->session = session;
			data->error_code = error_code;

			bool ret = Sync.set("ss_error_callback", data);
			EXPECT_EQ(ret, true);
		}
												 
		static void CALLBACK  ss_recvframe_callback(SS_SESSION session, const unsigned char* data_, int len, int type) {
			ss_recvframe_callback_data* data = new ss_recvframe_callback_data();
			data->session = session;
			data->data = new byte[len + 1];
			memcpy(data->data, data_, len);
			data->data[len] = 0;
			data->len = len;
			data->type = type;

			bool ret = Sync.set("ss_recvframe_callback", data);
			EXPECT_EQ(ret, true);
		}
												 
		//client callback			  
		static void CALLBACK  sc_disconnected_callback(SC_CLIENT client) {
			sc_disconnected_callback_data* data = new sc_disconnected_callback_data();
			data->client = client;

			bool ret = Sync.set("sc_disconnected_callback", data);
			//EXPECT_EQ(ret, 0);
		}
												 
		static void CALLBACK  sc_error_callback(SC_CLIENT client, int error_code) {
			sc_error_callback_data* data = new sc_error_callback_data();
			data->client = client;
			data->error_code = error_code;

			bool ret = Sync.set("sc_error_callback", data);
			EXPECT_EQ(ret, true);
		}
												 
		static void CALLBACK  sc_recvframe_callback(SC_CLIENT client, const unsigned char* data_, int len, int type) {
			sc_recvframe_callback_data* data = new sc_recvframe_callback_data();
			data->client = client;
			data->data = new byte[len+1];
			memcpy(data->data, data_, len);
			data->data[len] = 0;
			data->len = len;
			data->type = type;

			bool ret = Sync.set("sc_recvframe_callback", data);
			EXPECT_EQ(ret, true);
		}

		static utils::convert_sync<std::string, void*> Sync;
	};


	utils::convert_sync<std::string, void*> TestServerClient::Sync;

	TEST_F(TestServerClient, OneClient)
	{
		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server  = nullptr;
		int ret = SS_StartServer(port, &server);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(server, nullptr);

		EXPECT_TRUE(Sync.add("ss_connected_callback"));

		SC_CLIENT client = nullptr;
		ret = SC_ConnectToHost(ip.c_str(), port, &client);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(client, nullptr);

		ss_connected_callback_data* data(nullptr);
		ret = Sync.wait_once("ss_connected_callback", (void**)&data, 5000);
		EXPECT_EQ(ret, 0);
		EXPECT_EQ(data->server, server);
		EXPECT_EQ(data->client_ip, ip);

		SS_SESSION session = data->session;
		delete data;

		//server send string to client frame
		{
			EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

			std::string teststr = "Just a test frame data";
			ret = SS_SendFrame(session, (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
			EXPECT_EQ(ret, 0);

			sc_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			
			if (ret == 0) {
				EXPECT_EQ(data->client, client);
				EXPECT_EQ(data->len, teststr.length());
				std::string recvstr((char*)data->data);
				EXPECT_EQ(teststr, recvstr);
				EXPECT_EQ(data->type, SS_FRAME_STRING);

				delete[]data->data;
				delete data;
			}
		}


		//server send binary to client frame
		{
			EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

			byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

			ret = SS_SendFrame(session, sendbt, 12, SS_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			sc_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->client, client);
				EXPECT_EQ(data->len, 12);
				EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}

		//client send string to server frame
		{
			EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

			std::string teststr = "Just a test frame data";
			ret = SC_SendFrame(client, (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
			EXPECT_EQ(ret, 0);

			ss_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				EXPECT_EQ(data->len, teststr.length());
				std::string recvstr((char*)data->data);
				EXPECT_EQ(teststr, recvstr);
				EXPECT_EQ(data->type, SS_FRAME_STRING);

				delete[]data->data;
				delete data;
			}

		}


		//client send binary to server frame
		{
			EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

			byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

			ret = SC_SendFrame(client, sendbt, 12, SS_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			ss_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				EXPECT_EQ(data->len, 12);
				EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}

		//disconnect
		{
			EXPECT_TRUE(Sync.add("ss_disconnected_callback"));

			ret = SC_DisconnectFromHost(client);
			EXPECT_EQ(ret, 0);

			ss_disconnected_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_disconnected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				delete data;
			}
		}
	}

	TEST_F(TestServerClient, MutiClient)
	{
		const int clientsize = 20;

		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
		int ret = SS_StartServer(port, &server);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(server, nullptr);

		SC_CLIENT clients[clientsize] = {0};
		SS_SESSION sessions[clientsize] = {0};

		for (int i = 0; i < clientsize; i++) {
			EXPECT_TRUE(Sync.add("ss_connected_callback"));

			ret = SC_ConnectToHost(ip.c_str(), port, &clients[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(clients[i], nullptr);

			ss_connected_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_connected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			EXPECT_EQ(data->server, server);
			EXPECT_EQ(data->client_ip, ip);

			sessions[i] = data->session;
			delete data;
		}

		for (int i = 0; i < clientsize; i++) {
			//server send string to client frame
			{
				EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

				std::string teststr = "Just a test frame data";
				ret = SS_SendFrame(sessions[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->client, clients[i]);
					std::string recvstr((char*)data->data, data->len);
					EXPECT_EQ(teststr, recvstr);
					EXPECT_EQ(data->type, SS_FRAME_STRING);

					delete[]data->data;
					delete data;
				}
			}

			//server send binary to client frame
			{
				EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SS_SendFrame(sessions[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->client, clients[i]);
					EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
					EXPECT_EQ(data->type, SC_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}
			}

			//client send string to server frame
			{
				EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

				std::string teststr = "Just a test frame data";
				ret = SC_SendFrame(clients[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					std::string recvstr((char*)data->data, data->len);
					EXPECT_EQ(teststr, recvstr);
					EXPECT_EQ(data->type, SS_FRAME_STRING);

					delete[]data->data;
					delete data;
				}

			}


			//client send binary to server frame
			{
				EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SC_SendFrame(clients[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
					EXPECT_EQ(data->type, SC_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}
			}
		}

		//disconnect
		for (int i = 0; i < clientsize; i++) {
			{
				EXPECT_TRUE(Sync.add("ss_disconnected_callback"));

				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);

				ss_disconnected_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_disconnected_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					delete data;
				}
			}
		}
	}

	TEST_F(TestServerClient, MutiServer)
	{
		const int clientsize = 20;

		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SC_CLIENT clients[clientsize] = { 0 };
		SS_SERVER servers[clientsize] = { 0 };
		SS_SESSION sessions[clientsize] = { 0 };

		int ret = 0;

		for (int i = 0; i < clientsize; i++) {
			int ret = SS_StartServer(port+i, &servers[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(servers[i], nullptr);
		}

		for (int i = 0; i < clientsize; i++) {
			EXPECT_TRUE(Sync.add("ss_connected_callback"));

			int ret = SC_ConnectToHost(ip.c_str(), port + i, &clients[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(clients[i], nullptr);

			ss_connected_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_connected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			EXPECT_EQ(data->server, servers[i]);
			EXPECT_EQ(data->client_ip, ip);

			sessions[i] = data->session;
			delete data;
		}

		for (int i = 0; i < clientsize; i++) {
			//server send string to client frame
			{
				EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

				std::string teststr = "Just a test frame data";
				ret = SS_SendFrame(sessions[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->client, clients[i]);
					std::string recvstr((char*)data->data, data->len);
					EXPECT_EQ(teststr, recvstr);
					EXPECT_EQ(data->type, SS_FRAME_STRING);

					delete[]data->data;
					delete data;
				}
			}

			//server send binary to client frame
			{
				EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SS_SendFrame(sessions[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->client, clients[i]);
					EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
					EXPECT_EQ(data->type, SC_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}
			}

			//client send string to server frame
			{
				EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

				std::string teststr = "Just a test frame data";
				ret = SC_SendFrame(clients[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					std::string recvstr((char*)data->data, data->len);
					EXPECT_EQ(teststr, recvstr);
					EXPECT_EQ(data->type, SS_FRAME_STRING);

					delete[]data->data;
					delete data;
				}

			}


			//client send binary to server frame
			{
				EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SC_SendFrame(clients[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
					EXPECT_EQ(data->type, SC_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}
			}
		}

		//disconnect
		for (int i = 0; i < clientsize; i++) {
			{
				EXPECT_TRUE(Sync.add("ss_disconnected_callback"));

				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);

				ss_disconnected_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_disconnected_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					delete data;
				}
			}
		}
	}

	TEST_F(TestServerClient, Function) {
		NetParam param;
		memset(&param, 0, sizeof(NetParam));

		int ret = SS_GetNetParam(&param);
		ASSERT_EQ(ret, 0);


		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
		ret = SS_StartServer(port, &server);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(server, nullptr);

		EXPECT_TRUE(Sync.add("ss_connected_callback"));

		SC_CLIENT client = nullptr;
		ret = SC_ConnectToHost(ip.c_str(), port, &client);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(client, nullptr);

		ss_connected_callback_data* data(nullptr);
		ret = Sync.wait_once("ss_connected_callback", (void**)&data, 5000);
		EXPECT_EQ(ret, 0);
		EXPECT_EQ(data->server, server);
		EXPECT_EQ(data->client_ip, ip);

		SS_SESSION session = data->session;
		delete data;


		//make max package  test
		utils::Buffer max_buff(param.recv_buff_size * 1024 * 1024);
		BuildRandomData(max_buff);
		utils::Buffer max_buff_1(param.recv_buff_size * 1024 * 1024 + 1);
		//server send binary to client frame
		{
			EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

			ret = SS_SendFrame(session, max_buff.data(), max_buff.size(), SS_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			sc_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->client, client);
				EXPECT_EQ(data->len, max_buff.size());
				EXPECT_EQ(memcmp(data->data, max_buff.data(), max_buff.size()), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}

		{
			ret = SS_SendFrame(session, max_buff_1.data(), max_buff_1.size(), SS_FRAME_BINARY);
			EXPECT_EQ(ret, SS_PAYLOAD_TOO_BIG);

			//sc_error_callback_data* data(nullptr);
			//ret = Sync.wait_once("sc_error_callback", (void**)&data, 5000);
			//EXPECT_EQ(ret, 0);

			//if (ret == 0) {
			//	EXPECT_EQ(data->client, client);
			//	EXPECT_EQ(data->error_code, SC_PAYLOAD_TOO_BIG);

			//	delete data;
			//}
		}


		//client send binary to server frame
		{
			EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

			ret = SC_SendFrame(client, max_buff.data(), max_buff.size(), SC_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			ss_recvframe_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				EXPECT_EQ(data->len, max_buff.size());
				EXPECT_EQ(memcmp(data->data, max_buff.data(), max_buff.size()), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}
		{
			ret = SC_SendFrame(client, max_buff_1.data(), max_buff_1.size(), SS_FRAME_BINARY);
			EXPECT_EQ(ret, SC_PAYLOAD_TOO_BIG);

			//ss_error_callback_data* data(nullptr);
			//ret = Sync.wait_once("ss_error_callback", (void**)&data, 5000);
			//EXPECT_EQ(ret, 0);

			//if (ret == 0) {
			//	EXPECT_EQ(data->session, session);
			//	EXPECT_EQ(data->error_code, SC_PAYLOAD_TOO_BIG);

			//	delete data;
			//}
		}


		//test string boundary
		//OneClient has tested

		//disconnect
		{
			EXPECT_TRUE(Sync.add("ss_disconnected_callback"));

			ret = SC_DisconnectFromHost(client);
			EXPECT_EQ(ret, 0);

			ss_disconnected_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_disconnected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				delete data;
			}
		}
	}

	TEST_F(TestServerClient, BigFrame) {
		enum {
			MAX_FRAME_SIZE = 100//MB
		};

		NetParam param;
		memset(&param, 0, sizeof(NetParam));

		int ret = SS_GetNetParam(&param);
		ASSERT_EQ(ret, 0);

		param.recv_buff_size = MAX_FRAME_SIZE;

		ret = SS_SetNetParam(&param);
		ASSERT_EQ(ret, 0);

		const int clientsize = 2;

		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
		ret = SS_StartServer(port, &server);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(server, nullptr);

		SC_CLIENT clients[clientsize] = { 0 };
		SS_SESSION sessions[clientsize] = { 0 };

		for (int i = 0; i < clientsize; i++) {
			EXPECT_TRUE(Sync.add("ss_connected_callback"));

			ret = SC_ConnectToHost(ip.c_str(), port, &clients[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(clients[i], nullptr);

			ss_connected_callback_data* data(nullptr);
			ret = Sync.wait_once("ss_connected_callback", (void**)&data, 8000);
			EXPECT_EQ(ret, 0);
			EXPECT_EQ(data->server, server);
			EXPECT_EQ(data->client_ip, ip);

			sessions[i] = data->session;
			delete data;
		}

		//build data
		//make max package  test
		utils::Buffer max_buff(param.recv_buff_size * 1024 * 1024);

		std::cout << "begin build random data over";
		BuildRandomData(max_buff);
		std::cout << "build random data over";

		for (int i = 0; i < clientsize; i++) {
			//server send to client frame
			{
				EXPECT_TRUE(Sync.add("sc_recvframe_callback"));

				ret = SS_SendFrame(sessions[i], max_buff.data(), max_buff.size(), SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 10000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->client, clients[i]);
					EXPECT_EQ(data->len, max_buff.size());
					EXPECT_EQ(memcmp(data->data, max_buff.data(), max_buff.size()), 0);
					EXPECT_EQ(data->type, SS_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}
			}


			//client send to server frame
			{
				EXPECT_TRUE(Sync.add("ss_recvframe_callback"));

				ret = SC_SendFrame(clients[i], max_buff.data(), max_buff.size(), SC_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 10000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					EXPECT_EQ(data->len, max_buff.size());
					EXPECT_EQ(memcmp(data->data, max_buff.data(), max_buff.size()), 0);
					EXPECT_EQ(data->type, SC_FRAME_BINARY);

					delete[]data->data;
					delete data;
				}

			}
		}

		//disconnect
		for (int i = 0; i < clientsize; i++) {
			{
				EXPECT_TRUE(Sync.add("ss_disconnected_callback"));

				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);

				ss_disconnected_callback_data* data(nullptr);
				ret = Sync.wait_once("ss_disconnected_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					delete data;
				}
			}
		}
	}
}
