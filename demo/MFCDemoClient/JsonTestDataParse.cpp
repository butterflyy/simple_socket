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

std::string JsonTestDataParse::MakeUploadAccessInfo(){
	Poco::JSON::Object root;
	
	root.set("mod", "upload_access_info");
	root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
	root.set("type", JSON_REQ);


	Poco::JSON::Array access_list;
	for (int i = 0; i < 10; i++){
		Poco::JSON::Object access;
		access.set("person_id", utils::StrFormat("231%d", i));
		access.set("time", "2019-6-7 11:23:10");
		access.set("role", 2);
		access.set("open_door_type", 1);

		access_list.add(access);
	}

	root.set("access_list", access_list);


	std::stringstream ss;
	root.stringify(ss);

	return ss.str();
}

std::string JsonTestDataParse::MakeUploadDeviceStatus(){
	Poco::JSON::Object root;

	root.set("mod", "upload_device_status");
	root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
	root.set("type", JSON_REQ);

	root.set("door_status", 4);

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

bool JsonTestDataParse::SaveDownloadPersonImage(const std::string& json, std::string& imagePath, std::vector<std::string>& tmplPaths){
	try
	{
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result;
		result = parser.parse(json);
		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
		Poco::DynamicStruct ds = *object;

		//save face image
		std::string base64_face = ds["person_info"]["face"].toString();

		bytes face;
		if (!base64::decode(base64_face, face)){
			return false;
		}

		imagePath = "d:\\face.bmp";
		int size = utils::WriteFile(imagePath, face);
		if (size != face.size()){
			return false;
		}
		
		//save templs
		int tmpsize = ds["person_info"]["tmpls"].size();
		for (int i = 0; i < tmpsize; i++){
			std::string base64_tmpl = ds["person_info"]["tmpls"][i].toString();
			bytes tmpl;
			if (!base64::decode(base64_tmpl, tmpl)){
				return false;
			}

			std::string tmplPath = utils::StrFormat("d:\\tmpl%d.tmp", i);
			int size = utils::WriteFile(tmplPath, tmpl);
			if (size != tmpl.size()){
				return false;
			}

			tmplPaths.push_back(tmplPath);
		}
	}
	catch (Poco::Exception& e)
	{
		return false;
	}

	return true;
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

std::string JsonTestDataParse::MakeResultDeviceInfo(const std::string& session_id){
	Poco::JSON::Object root;

	root.set("mod", "get_device_info");
	root.set("session_id", session_id);
	root.set("type", JSON_ACK);

	root.set("ret", 0);
	root.set("msg", "");

	Poco::JSON::Object info;
	info.set("uuid", "123119823234342");
	info.set("device_ip", "192.168.1.156");
	info.set("device_port", 1545);
	info.set("server_ip", "192.168.1.166");
	info.set("server_port", 39877);
	info.set("address", "ÃÅ¿Ú¿¼ÇÚ»úÆ÷");

	root.set("device_info", info);


	std::stringstream ss;
	root.stringify(ss);

	return ss.str();
}