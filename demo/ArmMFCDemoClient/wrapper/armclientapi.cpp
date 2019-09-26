#include "armclientapi.h"
#include <whsarmclient.h>

#include "utils.h"
//#include "utf_gbk.h"
//#include "base64.h"
#include "message_.h"

#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/JSON/Parser.h>
#include <Poco/UUIDGenerator.h>



#define TMPL_SIZE 8192

#define JSON_REQ  1
#define JSON_ACK  2


static void mystrcpy(char* des, const char* src, int max_len){
	int len = (int)strlen(src);
	if (len >= max_len){
		len = max_len - 1;
	}
	memcpy(des, src, len);
	des[len] = 0;
};

struct CommandData{
	armnet_command_type type;
	std::string mod;
	std::string session_id;

	armnet_person_info person_info;
	std::vector<std::string> remove_persons;

	armnet_identify_result identify_result;

	armnet_device_config device_config;

	union
	{
		armnet_device_info device_info;

		char device_time[32];

		armnet_user_command user_command;
		int device_status;
		armnet_person_role person_role;
		armnet_person_tmp_pwd person_tmp_pwd;
	};
	armnet_result_code result;
};

class Wrapper;
Wrapper* wrapper = nullptr;

class Wrapper{
public:
	static void CALLBACK disconnected_callback(){
		//同步回调过程，避免阻塞
		wrapper->_armnet_error_cb(ARMNET_REMOTE_HOST_CLOSE);
	}

	static void CALLBACK error_callback(int error_code){
		//同步回调过程，避免阻塞
		wrapper->_armnet_error_cb(armnet_error_code(TransNetError(error_code)));
	}

	static int TransNetError(int err){
		switch (err)
		{
		case SC_SUCCESS: return ARMNET_SUCCESS;
		case SC_ERROR: return ARMNET_ERROR;
		case SC_INVALID_PARAM: return ARMNET_INVALID_PARAM;
		case SC_NETWORK_ERROR: return ARMNET_NETWORK_ERROR;
		case SC_NETWORK_DISCONNECTED: return ARMNET_REMOTE_HOST_CLOSE;
		case SC_NETWORK_TIMEOUT: return ARMNET_NETWORK_TIMEOUT;
		case SC_PAYLOAD_TOO_BIG: return ARMNET_PACKAGE_ERROR;
		case SC_FRAME_ERROR: return ARMNET_PACKAGE_ERROR;
		default: assert(false); return -1;
		}
	}

	static void CALLBACK recvframe_callback(const unsigned char* data, int len, int type){
		//同步回调过程，避免阻塞
		assert(type == SC_FRAME_STRING);

		//解析数据
		std::string json((char*)data, len);
		wrapper->parseData(json);
	}

	void parseData(const std::string& json){
		try
		{
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var result;
			result = parser.parse(json);
			Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
			Poco::DynamicStruct ds = *object;

			std::string mod = ds["mod"].toString();
			std::string session_id = ds["session_id"].toString();
			int type = ds["type"];

			wrapper->_commandData.mod = mod;
			wrapper->_commandData.session_id = session_id;

			std::string ackjson;
			if (mod == "set_device_info"){
				wrapper->_commandData.type = ARMNET_CMD_SET_DEVICE_INFO;

				armnet_device_info device_info;
				memset(&device_info, 0, sizeof(armnet_device_info));

				mystrcpy(device_info.device_ip, ds["device_info"]["device_ip"].toString().c_str(), 32);
				device_info.device_port, ds["device_info"]["device_port"];
				mystrcpy(device_info.uuid, ds["device_info"]["server_ip"].toString().c_str(), 32);
				device_info.server_port = ds["device_info"]["server_port"];
				mystrcpy(device_info.address, ds["device_info"]["address"].toString().c_str(), 32);
				mystrcpy(device_info.community_code, ds["device_info"]["community_code"].toString().c_str(), 32);

				memcpy(&wrapper->_commandData.device_info, &device_info, sizeof(armnet_device_info));
			}
			else if (mod == "get_device_info"){
				wrapper->_commandData.type = ARMNET_CMD_GET_DEVICE_INFO;
			}
			else if (mod == "set_device_time"){
				wrapper->_commandData.type = ARMNET_CMD_SET_DEVICE_TIME;
			}
			else if (mod == "set_device_config"){
				wrapper->_commandData.type = ARMNET_CMD_SET_DEVICE_CONFIG;

				wrapper->_commandData.device_config.workModel = ds["device_config"]["workModel"];
				wrapper->_commandData.device_config.validModel = ds["device_config"]["validModel"];
				wrapper->_commandData.device_config.passwdTimeout = ds["device_config"]["passwdTimeout"];
				wrapper->_commandData.device_config.passwdRetryCount = ds["device_config"]["passwdRetryCount"];
				wrapper->_commandData.device_config.validTimeout = ds["device_config"]["validTimeout"];
				wrapper->_commandData.device_config.weigenOutput = ds["device_config"]["weigenOutput"];
			}
			else if (mod == "get_device_config"){
				wrapper->_commandData.type = ARMNET_CMD_GET_DEVICE_CONFIG;
			}
			else if (mod == "send_user_command"){
				wrapper->_commandData.type = ARMNET_CMD_SEND_USER_COMMAND;
			}
			else if (mod == "get_device_status"){
				wrapper->_commandData.type = ARMNET_CMD_GET_DEVICE_STATUS;
			}
			else if (mod == "download_person"){
				wrapper->_commandData.type = ARMNET_CMD_DOWNLOAD_PERSON;

				wrapper->_commandData.person_info.person_id = ds["person_info"]["person_id"].toString();
				wrapper->_commandData.person_info.is_admin = ds["person_info"]["is_admin"];
				wrapper->_commandData.person_info.regist_time = ds["person_info"]["regist_time"].toString();
				wrapper->_commandData.person_info.position = ds["person_info"]["position"].toString();
				wrapper->_commandData.person_info.department = ds["person_info"]["department"].toString();
				wrapper->_commandData.person_info.org = ds["person_info"]["org"].toString();
				wrapper->_commandData.person_info.address = ds["person_info"]["address"].toString();
				wrapper->_commandData.person_info.nation = ds["person_info"]["nation"].toString();
				wrapper->_commandData.person_info.id_card = ds["person_info"]["id_card"].toString();
				wrapper->_commandData.person_info.sex = ds["person_info"]["sex"];
				wrapper->_commandData.person_info.card_no = ds["person_info"]["card_no"].toString();
				wrapper->_commandData.person_info.name = ds["person_info"]["name"].toString();;
				wrapper->_commandData.person_info.password = ds["person_info"]["password"].toString();
				wrapper->_commandData.person_info.alarm_flag = ds["person_info"]["alarm_flag"];

				wrapper->_commandData.person_info.has_feature = true;
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["FaceFeature"].toString();
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["FaceImage"].toString();
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["LeftIrisFeature"].toString();
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["LeftIrisImage"].toString();
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["RightIrisFeature"].toString();
				wrapper->_commandData.person_info.feature.FaceFeature = ds["person_feature"]["RightIrisImage"].toString();
			}
			else if (mod == "update_person"){
				wrapper->_commandData.type = ARMNET_CMD_UPDATE_PERSON;
			}
			else if (mod == "remove_person"){
				wrapper->_commandData.type = ARMNET_CMD_REMOVE_PERSON;

				wrapper->_commandData.remove_persons.clear();

				int size = ds["person_ids"].size();
				for (int i = 0; i < size; i++){
					std::string id = ds["person_ids"][i].toString();
					wrapper->_commandData.remove_persons.push_back(id);
				}
			}
			else if (mod == "set_person_role"){
				wrapper->_commandData.type = ARMNET_CMD_SET_PERSON_ROLE;
			}
			else if (mod == "set_person_tmp_pwd"){
				wrapper->_commandData.type = ARMNET_CMD_SET_PERSON_TMP_PWD;
			}
			else if (mod == "start_capture"){
				wrapper->_commandData.type = ARMNET_CMD_SEND_USER_COMMAND;
				wrapper->_commandData.user_command.type = 23;
			}
			else if (mod == "collect_feature_complete"){

			}
			else if (mod == "send_identify_result"){
				wrapper->_commandData.type = ARMNET_CMD_IDENTIFY_RESULT;

				wrapper->_commandData.identify_result.identifyId = ds["identify_id"].toString();
				wrapper->_commandData.identify_result.recog_result = ds["recog_result"];
				wrapper->_commandData.identify_result.validType = ds["validType"];
				wrapper->_commandData.identify_result.recogTime = ds["recogTime"].toString();
				wrapper->_commandData.identify_result.surplusPwd = ds["surplusPwd"];

				wrapper->_commandData.identify_result.personInfo.person_id = ds["person_info"]["person_id"].toString();
				wrapper->_commandData.identify_result.personInfo.is_admin = ds["person_info"]["is_admin"];
				wrapper->_commandData.identify_result.personInfo.regist_time = ds["person_info"]["regist_time"].toString();
				wrapper->_commandData.identify_result.personInfo.position = ds["person_info"]["position"].toString();
				wrapper->_commandData.identify_result.personInfo.department = ds["person_info"]["department"].toString();
				wrapper->_commandData.identify_result.personInfo.org = ds["person_info"]["org"].toString();
				wrapper->_commandData.identify_result.personInfo.address = ds["person_info"]["address"].toString();
				wrapper->_commandData.identify_result.personInfo.nation = ds["person_info"]["nation"].toString();
				wrapper->_commandData.identify_result.personInfo.id_card = ds["person_info"]["id_card"].toString();
				wrapper->_commandData.identify_result.personInfo.sex = ds["person_info"]["sex"];
				wrapper->_commandData.identify_result.personInfo.card_no = ds["person_info"]["card_no"].toString();
				wrapper->_commandData.identify_result.personInfo.name = ds["person_info"]["name"].toString();;
				wrapper->_commandData.identify_result.personInfo.password = ds["person_info"]["password"].toString();
				wrapper->_commandData.identify_result.personInfo.alarm_flag = ds["person_info"]["alarm_flag"];
																 
				wrapper->_commandData.identify_result.personInfo.has_feature = false;
			}
			else{
				utils::OutputDebug("unknow command!!");
			}


			_command_cb(&_commandData);
		}
		catch (Poco::Exception& e)
		{
			utils::OutputDebug(e.displayText().c_str());
			return;
		}
	}

	armnet_error_callback _armnet_error_cb;
	command_callback _command_cb;

	CommandData _commandData;
	byte _face[1920 * 1080];
	byte _left_image[1920 * 1080];
	byte _left_code[TMPL_SIZE * 10];
	byte _right_image[1920 * 1080];
	byte _right_code[TMPL_SIZE * 10];
};


/**
* @brief Initialize the Device API. This function must to be
*     called before any other API function call.
* @return 0 on success, or a error code.
*/
ARMCLIENT_API int ARMNET_Initialize(){
	int ret = SC_Initialize();
	wrapper = new Wrapper();
	return ret;
}


/**
* @brief Finalize the Device API. This function must to be called before closing
*    the client application to release internally allocated resources.
*    All devices will be disconnected and the server shutdown.
* @return 0 on success, or a error code.
*/
ARMCLIENT_API void ARMNET_Finalize(){
	SC_Finalize();
	SAFE_DELETE(wrapper);
}


ARMCLIENT_API const char* ARMNET_GetLibVersion(){
	static std::string str;
	str = SC_GetLibVersion();
	return str.c_str();
}
/**
* @brief Set the callback function.
* @param on_error is the error callback function.
* @param on_command is the command callback function.
* @return 0 on success, or a error code.
*/
ARMCLIENT_API int ARMNET_SetCallback(armnet_error_callback on_error,
	command_callback on_command){
	wrapper->_armnet_error_cb = on_error;
	wrapper->_command_cb = on_command;

	int ret = SC_SetCallback(Wrapper::disconnected_callback, Wrapper::error_callback, Wrapper::recvframe_callback);
	if (ret < 0){
		utils::OutputDebug("设置回调失败");
		return ARMNET_ERROR;
	}

	return 0;
}

/**
* @brief  Returns a constant string with a short description of the given error code.
* @param  error_code the error code whose description is desired.
* @return  a short description of the error code.
*/
ARMCLIENT_API const char* ARMNET_StrError(int error_code){
	switch (error_code)
	{
	case ARMNET_SUCCESS:
		return "ARMNET_SUCCESS";
	case ARMNET_ERROR:
		return "ARMNET_ERROR";
	case ARMNET_INVALID_PARAM:
		return "ARMNET_INVALID_PARAM";
	case ARMNET_REMOTE_HOST_CLOSE:
		return "ARMNET_REMOTE_HOST_CLOSE";
	case ARMNET_NETWORK_ERROR:
		return "ARMNET_NETWORK_ERROR";
	case ARMNET_NETWORK_TIMEOUT:
		return "ARMNET_NETWORK_TIMEOUT";
	case ARMNET_PACKAGE_ERROR:
		return "ARMNET_PACKAGE_ERROR";
	default:
		return "**ARMNET_UNKNOW**";
	}
}

/**
* @brief Connect to host.
* @param ip is the host ip address.
* @param port is the host port.
* @return 0 on success, or a error code.
*/
ARMCLIENT_API int ARMNET_ConnectToHost(const char* ip, int port){
	return Wrapper::TransNetError(SC_ConnectToHost(ip, port)); 
}

/**
* @brief Disconnect from host.
*/
ARMCLIENT_API void ARMNET_DisconnectFromHost(){
	SC_DisconnectFromHost();
}

/**
* 获取命令类型
*/
ARMCLIENT_API int ARMNET_GetCommandType(ARMNET_COMMAND_DATA command){
	CommandData* pCommand = reinterpret_cast<CommandData*>(command);
	if (!pCommand){
		return ARMNET_INVALID_PARAM;
	}
	return pCommand->type;
}

/**
* 设置设备信息命令对应的数据
*/
ARMCLIENT_API const struct armnet_device_info* ARMNET_GetDeviceInfo(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 获取设备信息命令对应的数据
*/
ARMCLIENT_API int ARMNET_SetDeviceInfo(ARMNET_COMMAND_DATA command, const struct armnet_device_info* info){
	CommandData* pCommand = reinterpret_cast<CommandData*>(command);
	if (!pCommand){
		return ARMNET_INVALID_PARAM;
	}

	memcpy(&pCommand->device_info, info, sizeof(armnet_device_info));

	return 0;
}

/**
* 设置设备时间命令对应的数据
*/
ARMCLIENT_API const char* ARMNET_GetDeviceTime(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 获取设备配置命令对应的数据
*/
ARMCLIENT_API const struct armnet_device_config* ARMNET_GetDeviceConfig(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 设置设备配置命令对应的数据
*/
ARMCLIENT_API int ARMNET_SetDeviceConfig(ARMNET_COMMAND_DATA command, const struct armnet_device_config* config){
	return 0;
}

/**
* 发送自定义命令对应的数据
*/
ARMCLIENT_API const struct armnet_user_command* ARMNET_GetUserCommand(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 获取设备状态命令对应的数据
*/
ARMCLIENT_API int ARMNET_SetDeviceStatus(ARMNET_COMMAND_DATA command, int status){
	return 0;
}

/**
* 下载人员信息命令对应的数据
*/
ARMCLIENT_API const struct armnet_person_info* ARMNET_GetDownloadPersonInfo(ARMNET_COMMAND_DATA command){
	CommandData* pCommand = reinterpret_cast<CommandData*>(command);
	if (!pCommand){
		return nullptr;
	}

	return &pCommand->person_info;
}

/**
* 更新人员信息命令对应的数据
*/
ARMCLIENT_API const struct armnet_person_info* ARMNET_GetUpdatePersonInfo(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 删除人员命令对应的数据
*/
ARMCLIENT_API const std::vector<std::string>* ARMNET_GetRemovePersonInfo(ARMNET_COMMAND_DATA command){
	CommandData* pCommand = reinterpret_cast<CommandData*>(command);
	if (!pCommand){
		return nullptr;
	}

	return &pCommand->remove_persons;
}

/**
* 设置人员黑白名单命令对应的数据
*/
ARMCLIENT_API const struct armnet_person_role* ARMNET_GetPersonRole(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 设置用户临时密码
*/
ARMCLIENT_API const struct armnet_person_tmp_pwd* ARMNET_GetPersonTmpPwd(ARMNET_COMMAND_DATA command){
	return 0;
}

/**
* 获取人脸虹膜识别返回
*/
ARMCLIENT_API const struct armnet_identify_result* ARMNET_GetIdentifyResult(ARMNET_COMMAND_DATA command){
	CommandData* pCommand = reinterpret_cast<CommandData*>(command);
	if (!pCommand){
		return nullptr;
	}

	return &pCommand->identify_result;
}

/**
*******************************重要***************************************
* 返回处理结果，所有的命令处理完成后，都需要调用此接口！！！
*/
ARMCLIENT_API int ARMNET_NotifyCommandResult(ARMNET_COMMAND_DATA command, enum armnet_result_code result_code){
	assert(command);

	CommandData* pData = static_cast<CommandData*>(command);
	try{
		Poco::JSON::Object root;

		root.set("mod", pData->mod);
		root.set("session_id", pData->session_id);
		root.set("type", JSON_ACK);

		root.set("ret", int(result_code));
		root.set("msg", "");

		if (result_code == ARMNET_RESULT_OK){//copy data
			switch (pData->type)
			{
			case ARMNET_CMD_SET_DEVICE_INFO:
				break;
			case ARMNET_CMD_GET_DEVICE_INFO:
			{
											   Poco::JSON::Object info;
											   info.set("uuid", std::string(pData->device_info.uuid));
											   info.set("device_ip", std::string(pData->device_info.device_ip));
											   info.set("device_port", pData->device_info.device_port);
											   info.set("server_ip", std::string(pData->device_info.server_ip));
											   info.set("server_port", pData->device_info.server_port);
											   info.set("address", std::string(pData->device_info.address));
											   info.set("community_code", std::string(pData->device_info.community_code));
											   info.set("software_version", std::string(pData->device_info.software_version));
											   info.set("hardware_version", std::string(pData->device_info.hardware_version));

											   root.set("device_info", info);
			}
				break;
			case ARMNET_CMD_SET_DEVICE_TIME:
				break;
			case ARMNET_CMD_SET_DEVICE_CONFIG:
				break;
			case ARMNET_CMD_GET_DEVICE_CONFIG:{
												  Poco::JSON::Object info;
												  info.set("workModel", pData->device_config.workModel);
												  info.set("validModel", pData->device_config.validModel);
												  info.set("passwdTimeout", pData->device_config.passwdTimeout);
												  info.set("passwdRetryCount", pData->device_config.passwdRetryCount);
												  info.set("validTimeout", pData->device_config.validTimeout);
												  info.set("weigenOutput", pData->device_config.weigenOutput);

												  root.set("device_config", info);
			}
				break;
			case ARMNET_CMD_SEND_USER_COMMAND:
				break;
			case ARMNET_CMD_GET_DEVICE_STATUS:
				break;
			case ARMNET_CMD_DOWNLOAD_PERSON:
				break;
			case ARMNET_CMD_UPDATE_PERSON:
				break;
			case ARMNET_CMD_REMOVE_PERSON:
				break;
			case ARMNET_CMD_SET_PERSON_ROLE:
				break;
			case ARMNET_CMD_SET_PERSON_TMP_PWD:
				break;
			default:
				break;
			}
		}

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		return Wrapper::TransNetError(SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING));
	}
	catch (Poco::Exception& e){
		utils::OutputDebug(e.displayText().c_str());
	}

	return ARMNET_SUCCESS;
}

/**
* 上传考勤记录
*/
ARMCLIENT_API int ARMNET_UploadAccessRecord(const struct armnet_access_info* info){
	try{
		Poco::JSON::Object root;
		root.set("mod", "upload_access_info");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_ACK);

		Poco::JSON::Object access_info;
		access_info.set("person_id", info->person_id);
		access_info.set("time", info->time);
		access_info.set("id_number", info->id_number);
		access_info.set("card_number", info->card_number);
		access_info.set("role", int(info->role));
		access_info.set("open_door_type", int(info->open_door_type));
		access_info.set("in_out_type", int(info->in_out_type));
		access_info.set("community_code", info->community_code);

		root.set("access_info", access_info);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		return Wrapper::TransNetError(SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING));
	}
	catch (Poco::Exception& e){
		utils::OutputDebug(e.displayText().c_str());
		return ARMNET_ERROR;
	}
}

/**
* 上传设备状态
*/
ARMCLIENT_API int ARMNET_UploadDeviceCondition(const struct armnet_device_condition* condition){
	return 0;
}

/**
* 上传人员信息记录
*/
ARMCLIENT_API int ARMNET_UploadPersonInfo(const struct armnet_person_info* info){
	try{
		Poco::JSON::Object root;

		root.set("mod", "collect_feature_complete");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_ACK);

		Poco::JSON::Object person_feature;
		person_feature.set("FaceFeature", info->feature.FaceFeature);
		person_feature.set("FaceImage", info->feature.FaceImage);
		person_feature.set("LeftIrisFeature", info->feature.LeftIrisFeature);
		person_feature.set("LeftIrisImage", info->feature.LeftIrisImage);
		person_feature.set("RightIrisFeature", info->feature.RightIrisFeature);
		person_feature.set("RightIrisImage", info->feature.RightIrisImage);

		root.set("person_feature", person_feature);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		return Wrapper::TransNetError(SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING));
	}
	catch (Poco::Exception& e){
		utils::OutputDebug(e.displayText().c_str());
		return ARMNET_ERROR;
	}
}

/**
* 上传设备心跳记录
*/
ARMCLIENT_API int ARMNET_UploadDeviceHeartbeat(const struct armnet_device_heartbeat* heartbeat){
	return 0;
}


ARMCLIENT_API int ARMNET_UploadIdentify(const struct armnet_identify_result* identify){
	try{
		Poco::JSON::Object root;

		root.set("mod", "cloud_identify");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_ACK);

		root.set("identifyId", identify->identifyId);
		root.set("time", identify->recogTime);

		Poco::JSON::Object person_feature;
		person_feature.set("FaceFeature", identify->personInfo.feature.FaceFeature);
		person_feature.set("FaceImage", identify->personInfo.feature.FaceImage);
		person_feature.set("LeftIrisFeature", identify->personInfo.feature.LeftIrisFeature);
		person_feature.set("LeftIrisImage", identify->personInfo.feature.LeftIrisImage);
		person_feature.set("RightIrisFeature", identify->personInfo.feature.RightIrisFeature);
		person_feature.set("RightIrisImage", identify->personInfo.feature.RightIrisImage);

		root.set("person_feature", person_feature);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		return Wrapper::TransNetError(SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING));
	}
	catch (Poco::Exception& e){
		utils::OutputDebug(e.displayText().c_str());
		return ARMNET_ERROR;
	}
}


ARMCLIENT_API int ARMNET_UploadIdentifyPassword(const struct armnet_identify_result* identify){
	try{
		Poco::JSON::Object root;

		root.set("mod", "cloud_identify_password");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_ACK);

		root.set("identifyId", identify->identifyId);
		root.set("time", identify->recogTime);
		root.set("password", identify->password);

		Poco::JSON::Object person_info;
		person_info.set("person_id", identify->personInfo.person_id);
		person_info.set("is_admin", identify->personInfo.is_admin);
		person_info.set("regist_time", identify->personInfo.regist_time);
		person_info.set("position", identify->personInfo.position);
		person_info.set("org", identify->personInfo.org);
		person_info.set("department", identify->personInfo.department);
		person_info.set("address", identify->personInfo.address);
		person_info.set("nation", identify->personInfo.nation);
		person_info.set("id_card", identify->personInfo.id_card);
		person_info.set("sex", identify->personInfo.sex);
		person_info.set("card_no", identify->personInfo.card_no);
		person_info.set("name", identify->personInfo.name);
		person_info.set("password", identify->personInfo.password);
		person_info.set("alarm_flag", identify->personInfo.alarm_flag);
		root.set("person_info", person_info);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		return Wrapper::TransNetError(SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING));
	}
	catch (Poco::Exception& e){
		utils::OutputDebug(e.displayText().c_str());
		return ARMNET_ERROR;
	}
}

/**
* 上传设备自定义命令
*/
ARMCLIENT_API int ARMNET_UploadDeviceCommand(const struct armnet_device_command* command){
	return 0;
}
