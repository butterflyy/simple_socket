UnitTest : UnitTest.o
	g++  -std=c++11 -DNDEBUG -Wl,-Bsymbolic UnitTest.o -o UnitTest -L/usr/lib -lwhsarmclient -L/usr/lib -lwhsarmserver -lgtest_main -lgtest -lpthread -lboost_system -lboost_thread -lboost_timer -lboost_chrono


UnitTest.o : UnitTest.cpp
	g++ -std=c++11 -DNDEBUG -Wl,-Bsymbolic -c UnitTest.cpp -I../../../api/simple_client_socket -I../../../api/simple_server_socket -I../../../../externals/include


clean :
	rm UnitTest UnitTest.o
