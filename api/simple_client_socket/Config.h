#pragma once
#include "Common.h"
#include <common/ConfigBase.h>
#include "NetParam.h"

_SC_BEGIN

const bool DEFAULT_KEEPALIVE_ENABLED = true;                       //keepalive enabled
const long DEFAULT_KEEPALIVE_TIME = 30 * 1000;                     //keepalive idle time is 30 second
const long DEFAULT_KEEPALIVE_INTERVALS = 10 * 1000;                //keepalive intervals is 10 second
const long DEFAULT_KEEPALIVE_PROBE = 3;                            //keepalve probe is 3 times


const bool DEFAULT_IS_LOG = true;
const bool DEFAULT_IS_LOG_BINARY = false;
const long DEFAULT_MAX_LOG_SIZE = 512;

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
		AddItemString<bool>(Data().net.keep_alive.enabled, "KeepAlive", "enabled", DEFAULT_KEEPALIVE_ENABLED);
		AddItemString<long>(Data().net.keep_alive.time, "KeepAlive", "time", DEFAULT_KEEPALIVE_TIME);
		AddItemString<long>(Data().net.keep_alive.interval, "KeepAlive", "interval", DEFAULT_KEEPALIVE_INTERVALS);
		AddItemString<long>(Data().net.keep_alive.probe, "KeepAlive", "probe", DEFAULT_KEEPALIVE_PROBE);
		AddItemString<bool>(Data().log_frame.is_log, "LogFrame", "is_log", DEFAULT_IS_LOG);
		AddItemString<bool>(Data().log_frame.is_log_binary, "LogFrame", "is_log_binary", DEFAULT_IS_LOG_BINARY);
		AddItemString<long>(Data().log_frame.max_log_size, "LogFrame", "max_log_size", DEFAULT_MAX_LOG_SIZE);
	}
};

_SC_END
