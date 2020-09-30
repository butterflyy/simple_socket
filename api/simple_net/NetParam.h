#pragma once


struct LogFrameParam{
	bool is_log;
	bool is_log_binary;
	long max_log_size;
};

struct KeepAlive{
	bool enabled;
	long time;
	long interval;
	long probe;
};

struct NetParam{
	KeepAlive keep_alive;
};


