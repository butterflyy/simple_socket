#pragma once
#include <common/ConfigBase.h>
#include "NetParam.h"


struct ConfigParam{
	NetParam net;
	LogFrameParam log_frame;
};

class Config :
	public ConfigBase<ConfigParam>
	, public utils::singleton<Config>
{
public:
	Config(){
		AddItemString<long>(Data().net.recv_buff_size, "Normal", "recv_buff_size", DEFAULT_RECV_BUFF_SIZE);
		AddItemString<long>(Data().net.keep_alive.heatbeat_time, "KeepAlive", "heatbeat_time", DEFAULT_HEARTBEAT_TIME);
		AddItemString<long>(Data().net.keep_alive.keepalive_time, "KeepAlive", "keepalive_time", DEFAULT_KEEPALIVE_TIME);
		AddItemString<long>(Data().net.keep_alive.keepalive_count, "KeepAlive", "keepalive_count", DEFAULT_KEEPALIVE_COUNT);
		AddItemString<bool>(Data().log_frame.is_log, "LogFrame", "is_log", DEFAULT_IS_LOG);
		AddItemString<bool>(Data().log_frame.is_log_binary, "LogFrame", "is_log_binary", DEFAULT_IS_LOG_BINARY);
		AddItemString<long>(Data().log_frame.max_log_size, "LogFrame", "max_log_size", DEFAULT_MAX_LOG_SIZE);
	}
};
