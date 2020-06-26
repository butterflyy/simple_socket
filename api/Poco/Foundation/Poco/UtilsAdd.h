#ifndef Foundation_UtilsAdd_INCLUDED
#define Foundation_UtilsAdd_INCLUDED

#include <stdio.h>
#include <string>

#ifdef WIN32
#include "Poco/UnWindows.h"
#else
#include <sched.h> //for sched_yield
#endif

namespace Poco{
	static std::string trim(const std::string& str){
		std::string::size_type pos = str.find_first_not_of(' ');
		if (pos == std::string::npos)
		{
			return str;
		}
		std::string::size_type pos2 = str.find_last_not_of(' ');
		if (pos2 != std::string::npos)
		{
			return str.substr(pos, pos2 - pos + 1);
		}
		return str.substr(pos);
	}

	class UtilsAdd{
	public:
		static void yield(){
#ifdef WIN32
			Sleep(0);
#else
			sched_yield();
#endif
		}
	};

	class NumberFormatter{
	public:
		static std::string format(int value){
			char buff[15] = { 0 };
			sprintf(buff, "%d", value);
			return buff;
		}

		static void append(std::string& str, int value){
			str.append(format(value));
		}

		static void appendHex(std::string& str, int value){
			char buff[15] = { 0 };
			sprintf(buff, "%x", value);
			str.append(buff, strlen(buff));
		}
	};
}

#endif //!