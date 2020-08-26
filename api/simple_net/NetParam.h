#pragma once

const long DEFAULT_HEARTBEAT_TIME = 5000;                        //send heartbeat time is 5 second
const long DEFAULT_KEEPALIVE_TIME = 10000;                       //check keepalive time is 10 second
const long DEFAULT_KEEPALIVE_COUNT = 3;                          //max check keepalve count is 3 times
const long DEFAULT_RECV_BUFF_SIZE = 5;                           //recv max buffer is 5MB
const bool DEFAULT_IS_LOG = true;                               
const bool DEFAULT_IS_LOG_BINARY = false;
const long DEFAULT_MAX_LOG_SIZE = 512;

struct LogFrame{
	bool is_log;
	bool is_log_binary;
	long max_log_size;
};

struct KeepAlive{
	long heatbeat_time;
	long keepalive_time;
	long keepalive_count;
};

struct NetParam{
	long recv_buff_size;
	KeepAlive keep_alive;
	LogFrame log_frame;
};



