# add include 
include_directories (${EXTERNALS_INCLUDE_DIR})

# add whsarmclient
include_directories (../api/simple_client_socket)
include_directories (../api/simple_server_socket)
#link_directories()


# add gtest
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})


add_definitions(-std=c++11)

link_libraries(whsarmclient whsarmserver)

if(UNIX)
link_libraries(pthread)
endif()

add_executable(echo_server echo_server.cpp)
add_executable(test_echo_server test_echo_server.cpp)
add_executable(test_client test_client.cpp)
add_executable(test_server test_server.cpp)
add_executable(UnitTest UnitTest.cpp)
target_link_libraries(UnitTest gtest gtest_main)
