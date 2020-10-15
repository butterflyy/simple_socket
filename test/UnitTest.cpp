// UnitTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <gtest/gtest.h>
#include <whsarmserver.h>
#include <whsarmclient.h>
#include <common/utils.h> //for msleep
#include <commonex/convert_sync.h>

//指定测试选项
// --gtest_filter=TestServerClient.BigFrame


SS_API int WINAPI SS_GetNetParam(struct NetParam* param);
SS_API int WINAPI SS_SetNetParam(const struct NetParam* param);

//inner interface
SC_API int WINAPI SC_GetClientPort(SC_CLIENT client, int* port);

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
		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
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


		//make max package  test
		utils::Buffer max_buff(6 * 1024 * 1024);
		BuildRandomData(max_buff);
		//utils::Buffer max_buff_1(param.recv_buff_size * 1024 * 1024 + 1);
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
	
	class TestServerClientAsync : public testing::Test {
	public:
		// Sets up the test fixture.
		void SetUp() override {
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
			std::unique_ptr<byte[]> data;
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
			std::unique_ptr<byte[]> data;
			int len;
			int type;
		};

		//server callback
		static void CALLBACK  ss_connected_callback(SS_SERVER server, SS_SESSION session, const char* client_ip, int client_port) {
			std::unique_ptr<ss_connected_callback_data> data(std::make_unique<ss_connected_callback_data>());
			data->server = server;
			data->session = session;
			data->client_ip = client_ip;
			data->client_port = client_port;

			data_mutex.lock();
			ss_connected_callback_datas.push_back(std::move(data));
			data_mutex.unlock();
		}

		static void CALLBACK  ss_disconnected_callback(SS_SESSION session) {
			std::unique_ptr<ss_disconnected_callback_data> data(std::make_unique<ss_disconnected_callback_data>());
			data->session = session;

			data_mutex.lock();
			ss_disconnected_callback_datas.push_back(std::move(data));
			data_mutex.unlock();
		}

		static void CALLBACK  ss_error_callback(SS_SESSION session, int error_code) {
			ASSERT_TRUE(false);
		}

		static void CALLBACK  ss_recvframe_callback(SS_SESSION session, const unsigned char* data_, int len, int type) {
			std::unique_ptr<ss_recvframe_callback_data> data(std::make_unique<ss_recvframe_callback_data>());
			data->session = session;
			data->data = std::make_unique<byte[]>(len);
			memcpy(data->data.get(), data_, len);
			data->len = len;
			data->type = type;

			data_mutex.lock();
			ss_recvframe_callback_datas.push_back(std::move(data));
			data_mutex.unlock();
		}

		//client callback			  
		static void CALLBACK  sc_disconnected_callback(SC_CLIENT client) {
			ASSERT_TRUE(false);
		}

		static void CALLBACK  sc_error_callback(SC_CLIENT client, int error_code) {
			ASSERT_TRUE(false);
		}

		static void CALLBACK  sc_recvframe_callback(SC_CLIENT client, const unsigned char* data_, int len, int type) {
			std::unique_ptr<sc_recvframe_callback_data> data(std::make_unique<sc_recvframe_callback_data>());
			data->client = client;
			data->data = std::make_unique<byte[]>(len);
			memcpy(data->data.get(), data_, len);
			data->len = len;
			data->type = type;

			data_mutex.lock();
			sc_recvframe_callback_datas.push_back(std::move(data));
			data_mutex.unlock();
		}

		static std::vector<std::unique_ptr<ss_connected_callback_data>> ss_connected_callback_datas;
		static std::vector<std::unique_ptr<ss_recvframe_callback_data>> ss_recvframe_callback_datas;
		static std::vector<std::unique_ptr<ss_disconnected_callback_data>> ss_disconnected_callback_datas;


		static std::vector<std::unique_ptr<sc_recvframe_callback_data>> sc_recvframe_callback_datas;

		static std::mutex data_mutex;
	};


	std::vector<std::unique_ptr<TestServerClientAsync::ss_connected_callback_data>> TestServerClientAsync::ss_connected_callback_datas;
	std::vector<std::unique_ptr<TestServerClientAsync::ss_recvframe_callback_data>> TestServerClientAsync::ss_recvframe_callback_datas;
	std::vector<std::unique_ptr<TestServerClientAsync::ss_disconnected_callback_data>> TestServerClientAsync::ss_disconnected_callback_datas;
	std::vector<std::unique_ptr<TestServerClientAsync::sc_recvframe_callback_data>> TestServerClientAsync::sc_recvframe_callback_datas;
	std::mutex TestServerClientAsync::data_mutex;


	TEST_F(TestServerClientAsync, ConcurrencyAsync) {
		const int clientsize = 40;

		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
		int ret = SS_StartServer(port, &server);
		ASSERT_EQ(ret, 0);
		ASSERT_NE(server, nullptr);

		SC_CLIENT clients[clientsize] = { 0 };
		int clientports[clientsize] = { 0 };
		SS_SESSION sessions[clientsize] = { 0 };

		std::vector<std::thread> thds;
		for (int i = 0; i < clientsize; i++) {
			std::thread thd([&, i] {
				ret = SC_ConnectToHost(ip.c_str(), port, &clients[i]);
				ASSERT_EQ(ret, 0);
				ASSERT_NE(clients[i], nullptr);

				ret = SC_GetClientPort(clients[i], &clientports[i]);
				ASSERT_EQ(ret, 0);
				ASSERT_NE(clientports[i], 0);
			});

			thds.push_back(std::move(thd));
		}

		for (std::vector<std::thread>::iterator it = thds.begin();
			it != thds.end();
			++it) {
			it->join();
		}
		thds.clear();

		utils::Thread::msleep(2000);//waiting recv data
		//check recv data
		ASSERT_EQ(ss_connected_callback_datas.size(), clientsize);
		//check connected data
		for (int i = 0; i < clientsize; i++) {
			bool find(false);
			for (auto& var : ss_connected_callback_datas) {
				if (var->client_port == clientports[i]) {
					ASSERT_EQ(var->server, server);
					sessions[i] = var->session;
					std::cout << "session : " << var->session << std::endl;
					find = true;
					break;
				}
			}
			ASSERT_TRUE(find);
		}
		
		std::string teststr1 = "Just a server test frame data";
		std::string teststr2 = "Just a client test frame data";
		std::vector<std::thread> thd1s;
		for (int i = 0; i < clientsize; i++) {
			//server send string to client frame
			std::thread thd([&, i]{
				std::string teststr = teststr1 + std::to_string(i);
				ret = SS_SendFrame(sessions[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);
			});
			thd1s.push_back(std::move(thd));
		}

		std::vector<std::thread> thd2s;
		for (int i = 0; i < clientsize; i++) {
			//client send string to server frame
			std::thread thd([&, i]{
				std::string teststr = teststr2 + std::to_string(i);
				ret = SC_SendFrame(clients[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);
			});
			thd2s.push_back(std::move(thd));
		}

		for (std::vector<std::thread>::iterator it = thd1s.begin();
			it != thd1s.end();
			++it) {
			it->join();
		}
		thd1s.clear();

		for (std::vector<std::thread>::iterator it = thd2s.begin();
			it != thd2s.end();
			++it) {
			it->join();
		}
		thd2s.clear();

		utils::Thread::msleep(5000);//waiting recv data
		//check server recv data
		ASSERT_EQ(ss_recvframe_callback_datas.size(), clientsize);
		for (int i = 0; i < clientsize; i++) {
			std::string teststr = teststr2 + std::to_string(i);
			bool find(false);
			for (auto& var : ss_recvframe_callback_datas) {
				if (var->session == sessions[i]) {
					ASSERT_EQ(var->len, teststr.length());
					ASSERT_TRUE(memcmp(var->data.get(), teststr.c_str(), var->len)== 0);
					ASSERT_EQ(var->type, SC_FRAME_STRING);

					find = true;
					break;
				}
			}
			ASSERT_TRUE(find);
		}

		//check client recv data
		ASSERT_EQ(sc_recvframe_callback_datas.size(), clientsize);
		for (int i = 0; i < clientsize; i++) {
			std::string teststr = teststr1 + std::to_string(i);
			bool find(false);
			for (auto& var : sc_recvframe_callback_datas) {
				if (var->client == clients[i]) {
					ASSERT_EQ(var->len, teststr.length());
					ASSERT_TRUE(memcmp(var->data.get(), teststr.c_str(), var->len) == 0);
					ASSERT_EQ(var->type, SC_FRAME_STRING);

					find = true;
					break;
				}
			}
			ASSERT_TRUE(find);
		}

		//disconnect
		thds.clear();
		for (int i = 0; i < clientsize; i++) {
			std::thread thd([&, i] {
				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);
			});
			thds.push_back(std::move(thd));
		}
		for (std::vector<std::thread>::iterator it = thds.begin();
			it != thds.end();
			++it) {
			it->join();
		}
		thds.clear();

		utils::Thread::msleep(5000);//disconnected callback
		//check ss_disconnected_callback_datas
		ASSERT_EQ(ss_disconnected_callback_datas.size(), clientsize);
		for (int i = 0; i < clientsize; i++) {
			bool find(false);
			for (auto& var : ss_disconnected_callback_datas) {
				if (var->session == sessions[i]) {
					find = true;
					break;
				}
			}
			ASSERT_TRUE(find);
		}
	}

	TEST_F(TestServerClient, ConcurrencySync) {
		const int clientsize = 500;

		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server = nullptr;
		int ret = SS_StartServer(port, &server);
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
			ret = Sync.wait_once("ss_connected_callback", (void**)&data, 10000);
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
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 10000);
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
				ret = Sync.wait_once("sc_recvframe_callback", (void**)&data, 10000);
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
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 10000);
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
				ret = Sync.wait_once("ss_recvframe_callback", (void**)&data, 10000);
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
}
