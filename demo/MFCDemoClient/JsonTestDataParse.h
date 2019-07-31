#pragma once
#include <string>
#include <vector>


#define JSON_REQ  1
#define JSON_ACK  2


class JsonTestDataParse
{
public:
	JsonTestDataParse();
	~JsonTestDataParse();

	//req
	static std::string MakeUploadAccessInfo();

	static std::string MakeUploadDeviceStatus();


	//ack
	static void Parse(const std::string& json, std::string& mod, std::string& session_id, int& type);

	static bool SaveDownloadPersonImage(const std::string& json, std::string& imagePath, std::vector<std::string>& tmplPaths);

	static std::string MakeResult(std::string& mod, const std::string& session_id);

	static std::string MakeResultDeviceInfo(const std::string& session_id);
};

