#include "AndroidLog.h"

#include <common/utils.h>
#include <android/log.h>

const std::string LOGFILE = "/sdcard/simpleclientsocket.txt";

const int MAX_LOGFILE_SIZE = 10 * 1024 * 1024; //10M

std::ofstream* log_file = nullptr;
utils::Mutex* log_mutex = nullptr;

void InitLogging(){
	log_file = new std::ofstream;
	log_file->open(LOGFILE, std::ios::app);
	size_t size = static_cast<size_t>(log_file->tellp());
	if (size != 0){ //is add log
		(*log_file) << "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
		(*log_file) << "**************NEW_LOG************";
		(*log_file) << "\r\n\r\n\r\n\r\n";
		log_file->flush();
	}
	log_mutex = new utils::Mutex;
}

void ShutdownLogging(){
	log_file->close();
	delete log_file;

	delete log_mutex;
}

void CheckLoggingSize(){
	utils::LockGuard<utils::Mutex> lock(*log_mutex);
	//ofs->seekg(0, std::ios_base::end);
	size_t size = static_cast<size_t>(log_file->tellp());
	if (size >= MAX_LOGFILE_SIZE){
		printf("log file is big, remove it \n");
		//delete and generate new file
		log_file->close();

		if (remove(LOGFILE.c_str()) != 0){
			printf("remove log file failed \n");
		}

		log_file->open(LOGFILE, std::ios::app);
	}
}

AndroidLog::AndroidLog(int level) : _level(level) {
	CheckLoggingSize();

	time_t now = time(nullptr);
	tm* t = localtime(&now);

	std::string slevel;

	switch (level) {
	case INFO:
		slevel = "INFO   ";
		break;

	case WARNING:
		slevel = "WARNING";
		break;

	case ERROR:
		slevel = "ERROR  ";
		break;

	case FATAL:
		slevel = "FATAL  ";
		break;

	default:
		slevel = "*UNKNOW*";
		break;
	}

	char buff[100];
	sprintf(buff, "%s %02d-%02d %02d:%02d:%02d  ", slevel.c_str(),
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	_ostr << buff;
}

AndroidLog::~AndroidLog() {
	__android_log_print(_level, "whsusbapi", "%s", _ostr.str().c_str());
	//write log
	log_mutex->lock();
	(*log_file) << _ostr.str() << "\r\n";
	log_file->flush();
	log_mutex->unlock();
}
