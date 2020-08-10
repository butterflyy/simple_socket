// UnitTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <atomic>
#include <gtest/gtest.h>
#include <whsarmserver.h>
#include <whsarmclient.h>
#include <common/ConvertSync.h>


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


		struct ss_connected_callback_data {
			SS_SERVER server;
			SS_SESSION session;
			std::string client_ip;
			int client_port;
		};

		struct ss_disconnected_callback_data {
			SS_SESSION session;
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

			utils::Thread::msleep(10);
			int ret = Sync.SetEvent("ss_connected_callback", data);
			EXPECT_EQ(ret, 0);
		}
												 
		static void CALLBACK  ss_disconnected_callback(SS_SESSION session) {
			ss_disconnected_callback_data* data = new ss_disconnected_callback_data();
			data->session = session;

			utils::Thread::msleep(10);
			int ret = Sync.SetEvent("ss_disconnected_callback", data);
			//EXPECT_EQ(ret, 0);
		}
												 
		static void CALLBACK  ss_error_callback(SS_SESSION session, int error_code) {
			EXPECT_TRUE(false);
		}
												 
		static void CALLBACK  ss_recvframe_callback(SS_SESSION session, const unsigned char* data_, int len, int type) {
			ss_recvframe_callback_data* data = new ss_recvframe_callback_data();
			data->session = session;
			data->data = new byte[len];
			memcpy(data->data, data_, len);
			data->len = len;
			data->type = type;

			int ret = Sync.SetEvent("ss_recvframe_callback", data);
			EXPECT_EQ(ret, 0);
		}
												 
		//client callback			  
		static void CALLBACK  sc_disconnected_callback(SC_CLIENT client) {
			sc_disconnected_callback_data* data = new sc_disconnected_callback_data();
			data->client = client;

			utils::Thread::msleep(10);
			int ret = Sync.SetEvent("sc_disconnected_callback", data);
			//EXPECT_EQ(ret, 0);
		}
												 
		static void CALLBACK  sc_error_callback(SC_CLIENT client, int error_code) {
			EXPECT_TRUE(false);
		}
												 
		static void CALLBACK  sc_recvframe_callback(SC_CLIENT client, const unsigned char* data_, int len, int type) {
			sc_recvframe_callback_data* data = new sc_recvframe_callback_data();
			data->client = client;
			data->data = new byte[len];
			memcpy(data->data, data_, len);
			data->len = len;
			data->type = type;

			int ret = Sync.SetEvent("sc_recvframe_callback", data);
			EXPECT_EQ(ret, 0);
		}

		static ConvertSync<std::string, void*> Sync;
		static int GenUID(){
			static std::atomic<int> uid = 0;
			return ++uid;
		}
	};


	ConvertSync<std::string, void*> TestServerClient::Sync;

	TEST_F(TestServerClient, OneClient)
	{
		const std::string ip = "127.0.0.1";
		const int port = 56234;

		SS_SERVER server  = nullptr;
		int ret = SS_StartServer(port, &server);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(server, nullptr);

		SC_CLIENT client = nullptr;
		ret = SC_ConnectToHost(ip.c_str(), port, &client);
		EXPECT_EQ(ret, 0);
		EXPECT_NE(client, nullptr);

		ss_connected_callback_data* data(nullptr);
		ret = Sync.WaitEvent("ss_connected_callback", (void**)&data, 5000);
		EXPECT_EQ(ret, 0);
		EXPECT_EQ(data->server, server);
		EXPECT_EQ(data->client_ip, ip);

		SS_SESSION session = data->session;
		delete data;

		//server send string to client frame
		{
			std::string teststr = "Just a test frame data";
			ret = SS_SendFrame(session, (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
			EXPECT_EQ(ret, 0);

			sc_recvframe_callback_data* data(nullptr);
			ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			
			if (ret == 0) {
				EXPECT_EQ(data->client, client);
				std::string recvstr((char*)data->data, data->len);
				EXPECT_EQ(teststr, recvstr);
				EXPECT_EQ(data->type, SS_FRAME_STRING);

				delete[]data->data;
				delete data;
			}
		}


		//server send binary to client frame
		{
			byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

			ret = SS_SendFrame(session, sendbt, 12, SS_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			sc_recvframe_callback_data* data(nullptr);
			ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->client, client);
				EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}

		//client send string to server frame
		{
			std::string teststr = "Just a test frame data";
			ret = SC_SendFrame(client, (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
			EXPECT_EQ(ret, 0);

			ss_recvframe_callback_data* data(nullptr);
			ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				std::string recvstr((char*)data->data, data->len);
				EXPECT_EQ(teststr, recvstr);
				EXPECT_EQ(data->type, SS_FRAME_STRING);

				delete[]data->data;
				delete data;
			}

		}


		//client send binary to server frame
		{
			byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

			ret = SC_SendFrame(client, sendbt, 12, SS_FRAME_BINARY);
			EXPECT_EQ(ret, 0);

			ss_recvframe_callback_data* data(nullptr);
			ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);

			if (ret == 0) {
				EXPECT_EQ(data->session, session);
				EXPECT_EQ(memcmp(data->data, sendbt, 12), 0);
				EXPECT_EQ(data->type, SC_FRAME_BINARY);

				delete[]data->data;
				delete data;
			}
		}

		//disconnect
		{
			ret = SC_DisconnectFromHost(client);
			EXPECT_EQ(ret, 0);

			ss_disconnected_callback_data* data(nullptr);
			ret = Sync.WaitEvent("ss_disconnected_callback", (void**)&data, 5000);
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
			ret = SC_ConnectToHost(ip.c_str(), port, &clients[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(clients[i], nullptr);

			ss_connected_callback_data* data(nullptr);
			ret = Sync.WaitEvent("ss_connected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			EXPECT_EQ(data->server, server);
			EXPECT_EQ(data->client_ip, ip);

			sessions[i] = data->session;
			delete data;
		}

		for (int i = 0; i < clientsize; i++) {
			//server send string to client frame
			{
				std::string teststr = "Just a test frame data";
				ret = SS_SendFrame(sessions[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
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
				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SS_SendFrame(sessions[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
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
				std::string teststr = "Just a test frame data";
				ret = SC_SendFrame(clients[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
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
				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SC_SendFrame(clients[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
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
				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);

				ss_disconnected_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_disconnected_callback", (void**)&data, 5000);
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
			int ret = SC_ConnectToHost(ip.c_str(), port + i, &clients[i]);
			EXPECT_EQ(ret, 0);
			EXPECT_NE(clients[i], nullptr);

			ss_connected_callback_data* data(nullptr);
			ret = Sync.WaitEvent("ss_connected_callback", (void**)&data, 5000);
			EXPECT_EQ(ret, 0);
			EXPECT_EQ(data->server, servers[i]);
			EXPECT_EQ(data->client_ip, ip);

			sessions[i] = data->session;
			delete data;
		}

		for (int i = 0; i < clientsize; i++) {
			//server send string to client frame
			{
				std::string teststr = "Just a test frame data";
				ret = SS_SendFrame(sessions[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
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
				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SS_SendFrame(sessions[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				sc_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("sc_recvframe_callback", (void**)&data, 5000);
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
				std::string teststr = "Just a test frame data";
				ret = SC_SendFrame(clients[i], (byte*)teststr.c_str(), teststr.length(), SS_FRAME_STRING);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
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
				byte sendbt[12] = { 0x42, 0x4D, 0x16, 0x52, 0xF2, 0x32, 0x12, 0x12, 0x92, 0xA2, 0x12, 0x32 };

				ret = SC_SendFrame(clients[i], sendbt, 12, SS_FRAME_BINARY);
				EXPECT_EQ(ret, 0);

				ss_recvframe_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_recvframe_callback", (void**)&data, 5000);
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
				ret = SC_DisconnectFromHost(clients[i]);
				EXPECT_EQ(ret, 0);

				ss_disconnected_callback_data* data(nullptr);
				ret = Sync.WaitEvent("ss_disconnected_callback", (void**)&data, 5000);
				EXPECT_EQ(ret, 0);

				if (ret == 0) {
					EXPECT_EQ(data->session, sessions[i]);
					delete data;
				}
			}
		}
	}
}