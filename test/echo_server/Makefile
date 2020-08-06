echo_server : echo_server.o
	g++  -std=c++11 echo_server.o -o echo_server -L/usr/lib -lwhsarmserver


echo_server.o : echo_server.cpp
	g++ -std=c++11 -c echo_server.cpp -I../../api/simple_server_socket -I../../../externals/include


clean :
	rm echo_server echo_server.o
