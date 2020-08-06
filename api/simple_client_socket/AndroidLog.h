#ifndef __ANDROID_LOG_H__
#define __ANDROID_LOG_H__

#include <sstream>

const int INFO = 4, WARNING = 5, ERROR = 6, FATAL = 7; // some to android log level

void InitLogging();

void ShutdownLogging();

void CheckLoggingSize();

class AndroidLog {
public:
	AndroidLog(int level);
	~AndroidLog();

	std::ostringstream& stream(){
		return _ostr;
	}

private:
	std::ostringstream _ostr;
	int _level;
};

#define LOG(level) AndroidLog(level).stream()
#define LOG_IF(level, con) if(con) LOG(level)

#endif //__ANDROID_LOG_H__