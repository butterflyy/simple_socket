#pragma once
#include <common/ConfigBase.h>
#include "NetParam.h"


class Config :
	public ConfigBase<NetParam>
	, public utils::singleton<Config>
{
public:
	Config(){
		AddItemString<long>(Data().heatbeat_time, "Normal", "heatbeat_time", DEFAULT_HEARTBEAT_TIME);
		AddItemString<long>(Data().recv_buff_size, "Normal", "recv_buff_size", DEFAULT_RECV_BUFF_SIZE);
		AddItemString<bool>(Data().log_frame.is_log, "LogFrame", "is_log", DEFAULT_IS_LOG);
		AddItemString<bool>(Data().log_frame.is_log_binary, "LogFrame", "is_log_binary", DEFAULT_IS_LOG_BINARY);
		AddItemString<long>(Data().log_frame.max_log_size, "LogFrame", "max_log_size", DEFAULT_MAX_LOG_SIZE);
	}
};
