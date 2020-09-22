# add include 
include_directories (${EXTERNALS_INCLUDE_DIR})

# add whsarmclient
include_directories (../../api/simple_client_socket)
#link_directories()


# add whsarmserver
include_directories (../../api/simple_server_socket)
#link_directories()


# add gtest
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

# add boost
include_directories(${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIB_DIR})

add_executable (UnitTest UnitTest.cpp)
target_link_libraries(UnitTest whsarmclient whsarmserver gtestd gtest_maind)