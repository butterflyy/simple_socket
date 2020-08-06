test_echo_server : test_echo_server.o
	g++  -std=c++11 test_echo_server.o -o test_echo_server -L/usr/lib -lwhsarmclient


test_echo_server.o : test_echo_server.cpp
	g++ -std=c++11 -c test_echo_server.cpp -I../../api/simple_client_socket -I../../../externals/include


clean :
	rm test_echo_server test_echo_server.o
