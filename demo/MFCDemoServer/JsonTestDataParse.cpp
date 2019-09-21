#include "stdafx.h"
#include "JsonTestDataParse.h"
#include <common\utils.h>
#include <common\utf_gbk.h>
#include <common\base64.h>
#include <Poco\JSON\Object.h>
#include <Poco\Dynamic\Struct.h>
#include <Poco\JSON\Parser.h>
#include <Poco\UUIDGenerator.h>


JsonTestDataParse::JsonTestDataParse()
{
}


JsonTestDataParse::~JsonTestDataParse()
{
}


std::string JsonTestDataParse::MakeGetDeviceInfo(){
	Poco::JSON::Object root;

	root.set("mod", "get_device_info");
	root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
	root.set("type", JSON_REQ);

	std::stringstream ss;
	root.stringify(ss);

	return ss.str();
}

std::string JsonTestDataParse::MakeDownloadPerson(const std::string& imagePath,
	const std::vector<std::string>& tmplPaths){
	Poco::JSON::Object root;

	root.set("mod", "download_person");
	root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
	root.set("type", JSON_REQ);

	Poco::JSON::Object info;
	info.set("person_id", "94580943721930821");
	info.set("person_name", "yingjie");
	info.set("id_number", "123233754");

	byte* facebuff(nullptr);
	int size = utils::ReadFile(imagePath, &facebuff);
	if (size < 0){
		return "";
	}

	std::string base64_face = base64::encode(bytes((char*)facebuff, size));
	delete[]facebuff;

	info.set("face", base64_face);

	Poco::JSON::Array tmpls;

	for (int i = 0; i < tmplPaths.size(); i++){
		byte* tmplbuff(nullptr);
		int size = utils::ReadFile(tmplPaths[i], &tmplbuff);
		if (size < 0){
			return "";
		}

		std::string base64_tmpl = base64::encode(bytes((char*)tmplbuff, size));
		delete[]tmplbuff;

		tmpls.add(base64_tmpl);
	}
	info.set("tmpls", tmpls);

	root.set("person_info", info);

	std::stringstream ss;
	root.stringify(ss);

	return ss.str();
}

void JsonTestDataParse::Parse(const std::string& json, std::string& mod, std::string& session_id, int& type){
	try
	{
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result;
		result = parser.parse(json);
		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
		Poco::DynamicStruct ds = *object;

		mod = ds["mod"].toString();
		session_id = ds["session_id"].toString();
		type = ds["type"];
	}
	catch (Poco::Exception& e)
	{
		return;
	}
}

std::string JsonTestDataParse::MakeResult(std::string& mod, const std::string& session_id){
	Poco::JSON::Object root;

	root.set("mod", mod);
	root.set("session_id", session_id);
	root.set("type", JSON_ACK);
	root.set("ret", 0);
	root.set("msg", "");

	std::stringstream ss;
	root.stringify(ss);

	return ss.str();
}
