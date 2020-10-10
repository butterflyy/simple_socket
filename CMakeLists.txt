cmake_minimum_required (VERSION 3.0)

project (simple_socket)


option (USE_BOOST "Not support C11, need boost" ON)

#externals
set(EXTERNALS_DIR ${CMAKE_SOURCE_DIR}/../externals)

#externals include
set(EXTERNALS_INCLUDE_DIR ${EXTERNALS_DIR}/include)

if (WIN32)
    #glog
    set(GLOG_DIR ${EXTERNALS_DIR}/3rdparty/glog-0.4.0)
    set(GLOG_INCLUDE_DIR ${GLOG_DIR}/src/windows)

    #gtest
    set(GTEST_DIR ${EXTERNALS_DIR}/3rdparty/googletest-release-1.10.0)
    set(GTEST_INCLUDE_DIR ${GTEST_DIR}/googletest/include) 

    if(CMAKE_CL_64)
        set(GLOG_LIB_DIR ${GLOG_DIR}/build_x64)
        set(GTEST_LIB_DIR ${GTEST_DIR}/build_x64/lib)
    else()
        set(GLOG_LIB_DIR ${GLOG_DIR}/build_Win32)
        set(GTEST_LIB_DIR ${GTEST_DIR}/build_Win32/lib)
    endif()
elseif (UNIX)
    #glog
    set(GLOG_INCLUDE_DIR /usr/local/include)
    set(GLOG_LIB_DIR /usr/lib)

    #gtest
    set(GTEST_INCLUDE_DIR /usr/local/include)
    set(GTEST_LIB_DIR /usr/lib)

    #boost
else ()
    message(satus "Other system")
endif ()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory(api/simple_client_socket)
add_subdirectory(api/simple_server_socket)
add_subdirectory(test)