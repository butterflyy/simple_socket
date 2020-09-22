cmake_minimum_required (VERSION 2.8)

project (simple_socket)

#externals
if (NOT EXTERNALS_DIR)
    set(EXTERNALS_DIR D:/Workspace/externals)
endif ()

#externals include
set(EXTERNALS_INCLUDE_DIR ${EXTERNALS_DIR}/include)

#glog
set(GLOG_DIR ${EXTERNALS_DIR}/glog-0.4.0)
set(GLOG_INCLUDE_DIR ${GLOG_DIR}/src/windows)
set(GLOG_LIB_DIR ${GLOG_DIR})

#gtest
set(GTEST_DIR ${EXTERNALS_DIR}/googletest-release-1.10.0)
set(GTEST_INCLUDE_DIR ${GTEST_DIR}/googletest/include)
set(GTEST_LIB_DIR ${GTEST_DIR}/build/lib)

#boost
if (NOT BOOST_DIR)
    set(BOOST_DIR D:/Workspace/externals/boost_1_73_0)
endif ()
set(BOOST_INCLUDE_DIR ${BOOST_DIR})

if (NOT BOOST_LIB_DIR)
    set(BOOST_LIB_DIR ${BOOST_DIR}/stage/lib)
endif ()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)


add_subdirectory(api/simple_client_socket)
add_subdirectory(api/simple_server_socket)
add_subdirectory(test/UnitTest)