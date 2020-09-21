#pragma once
#include <common/ConfigBase.h>
#include "NetParam.h"


struct ConfigParam{
	LogFrameParam log_frame;
};

class Config :
	public ConfigBase<ConfigParam>
	, public utils::singleton<Config>
{
public:
	Config(){
		AddItemString<bool>(Data().log_frame.is_log, "LogFrame", "is_log", DEFAULT_IS_LOG);
		AddItemString<bool>(Data().log_frame.is_log_binary, "LogFrame", "is_log_binary", DEFAULT_IS_LOG_BINARY);
		AddItemString<long>(Data().log_frame.max_log_size, "LogFrame", "max_log_size", DEFAULT_MAX_LOG_SIZE);
	}
};

