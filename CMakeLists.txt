cmake_minimum_required (VERSION 2.8)

project (simple_socket)

#externals
set(EXTERNALS_INCLUDE_DIR D:/Workspace/externals/include)

#glog
set(GLOG_INCLUDE_DIR D:/Workspace/externals/glog-0.4.0/src/windows)
set(GLOG_LIB_DIR D:/Workspace/externals/glog-0.4.0)

#gtest
set(GTEST_INCLUDE_DIR D:/Workspace/externals/googletest-release-1.10.0/googletest/include)
set(GTEST_LIB_DIR D:/Workspace/externals/googletest-release-1.10.0/build/lib)

#boost
set(BOOST_INCLUDE_DIR D:/Workspace/externals/boost_1_73_0)
set(BOOST_LIB_DIR D:/Workspace/externals/boost_1_73_0/stage/lib)

add_subdirectory(api/simple_client_socket)
add_subdirectory(api/simple_server_socket)

add_subdirectory(test/UnitTest/UnitTest)