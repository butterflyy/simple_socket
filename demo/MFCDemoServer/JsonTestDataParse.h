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
	static std::string MakeGetDeviceInfo(std::string& session_id);

	static std::string MakeDownloadPerson(const std::string& imagePath,
		const std::vector<std::string>& tmplPaths, std::string& session_id);


	//ack
	static void Parse(const std::string& json, std::string& mod, std::string& session_id, int& type);

	static std::string MakeResult(std::string& mod, const std::string& session_id);
};

