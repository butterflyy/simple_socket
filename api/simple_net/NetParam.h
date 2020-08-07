#pragma once

const long DEFAULT_HEARTBEAT_TIME = 5000;                        //send heartbeat time is 5 second
const long DEFAULT_RECV_BUFF_SIZE = 5;                           //recv max buffer is 5MB
const bool DEFAULT_IS_LOG = false;                               
const bool DEFAULT_IS_LOG_BINARY = false;
const long DEFAULT_MAX_LOG_SIZE = 512;

struct LogFrame{
	bool is_log;
	bool is_log_binary;
	long max_log_size;
};

struct NetParam{
	long heatbeat_time;
	long recv_buff_size;
	LogFrame log_frame;
};



