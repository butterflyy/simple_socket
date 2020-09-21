# add include 
include_directories (../Poco/Foundation)
include_directories (../Poco/Net)
include_directories (../simple_net)
include_directories (.)
include_directories (${EXTERNALS_INCLUDE_DIR})

file(GLOB_RECURSE SRC_POCO_FOUNDATION "../Poco/Foundation/*.cpp")
file(GLOB_RECURSE SRC_POCO_NET "../Poco/Net/*.cpp")
file(GLOB_RECURSE SRC_NET "../simple_net/*.cpp")

list (APPEND SRC_LIB
    ./ServerImp.cpp
    ./ServerManagerImp.cpp
    ./EventManager.cpp
    ./whsarmserver.cpp
    ./whsarmserver.def
    ./whsarmserver.rc
)

# add glog
include_directories(${GLOG_INCLUDE_DIR})
link_directories(${GLOG_LIB_DIR})


add_definitions(-D SS_API_EXPORTS)

# 生成链接库
add_library (whsarmserver SHARED ${SRC_POCO_FOUNDATION} ${SRC_POCO_NET} ${SRC_NET} ${SRC_LIB})
target_link_libraries(whsarmserver PRIVATE glogd ws2_32)