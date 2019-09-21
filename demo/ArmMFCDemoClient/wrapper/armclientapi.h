/*
* (C) Wuhan Hongshi Technologies. Copyright (c) 2016
*
* NOTICE: The intellectual and technical concepts and all information and source codes contained herein
* are, and remains the property of the copyright holders and may be covered by Patents,
* patents in process, and are protected by trade secret or copyright law.
*
* This code is proprietary code. Redistribution and use of source and binary forms, with or without modifications
* and any form of dissemination of this information or reproduction of this material, even partially, is strictly
* forbidden unless prior written permission obtained from the author and copyright holders.
*/

#ifndef __ARMCLIENT_API__
#define __ARMCLIENT_API__

#ifdef WIN32	
#ifdef ARMCLIENT_API_EXPORTS
#define ARMCLIENT_API
#else
#define ARMCLIENT_API
#endif
#else
#define ARMCLIENT_API
#include <stddef.h> //for size_t
#endif
#include <string.h>

/** armclientapi version */
//#define ARMCLIENTAPI_VERSION "3.5.8"


#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	enum armnet_error_code {
		/** Success (no error) */
		ARMNET_SUCCESS = 0,

		/** General error */
		ARMNET_ERROR = -1,

		/** Invalid parameter */
		ARMNET_INVALID_PARAM = -2,

		ARMNET_REMOTE_HOST_CLOSE = -3,

		ARMNET_NETWORK_ERROR = -4,

		ARMNET_NETWORK_TIMEOUT = -5,

		ARMNET_PACKAGE_ERROR = -6,
	};

	/**
	* Device State Command
	*/

	enum armnet_command_type
	{
		ARMNET_CMD_SET_DEVICE_INFO,

		ARMNET_CMD_GET_DEVICE_INFO,

		ARMNET_CMD_SET_DEVICE_TIME,

		ARMNET_CMD_SET_DEVICE_CONFIG,

		ARMNET_CMD_GET_DEVICE_CONFIG,

		ARMNET_CMD_SEND_USER_COMMAND,

		ARMNET_CMD_GET_DEVICE_STATUS,

		ARMNET_CMD_DOWNLOAD_PERSON,

		ARMNET_CMD_UPDATE_PERSON,

		ARMNET_CMD_REMOVE_PERSON,

		ARMNET_CMD_SET_PERSON_ROLE,

		ARMNET_CMD_SET_PERSON_TMP_PWD,
	};

	enum armnet_result_code{
		ARMNET_RESULT_OK,

		ARMNET_RESULT_DEVICE_ERROR,

		ARMNET_RESULT_UNHANDLE_ERROR = 99,
	};

	#pragma pack (push, 1)

	struct armnet_device_info{
		char uuid[32];
		char device_ip[32];
		unsigned int device_port;
		char server_ip[32];
		unsigned int server_port;
		char address[256];
		char community_code[64];
		char software_version[32];
		char hardware_version[32];
	};


	struct armnet_device_config{
		char heart_jump_period[32];
		char keep_open_timeout[32];
		char keep_open_range[256];
		char tmp_pwd_timeout[32];
	};

	struct armnet_data{
		unsigned int length;
		//data pointer data, do not operate directly!
		//use get_data or set_data function to handle date pointer.
		unsigned char ptr_data[8];

		unsigned char* get_data() const{
			unsigned __int64 ptr = 0;
			memcpy(&ptr, ptr_data, 8);
			return  (unsigned char*)ptr;
		}
		void set_data(unsigned char* data){
			unsigned __int64 ptr = (unsigned __int64)data;
			memcpy(ptr_data, &ptr, 8);
		}
	};

	struct armnet_image{
		unsigned int width;
		unsigned int height;
		unsigned char format;
		armnet_data data;
	};

	struct armnet_tmpl{
		armnet_image image;
		armnet_data code;
	};

	struct armnet_person_info{
		char person_id[32];
		char person_name[32];
		char id_number[32];
		char card_number[32];
		char key_number[32];
		char department_number[32];
		unsigned char role;
		armnet_image face;
		armnet_tmpl tmpl1;
		armnet_tmpl tmpl2;
	};

	struct armnet_access_info{
		char person_id[32];
		char time[32];
		char id_number[32];
		char card_number[32];
		unsigned char role;
		unsigned char open_door_type;
		unsigned char in_out_type;
		char community_code[64];
	};

	struct armnet_user_command{
		unsigned int type;
		char  value[128];
	};

	struct armnet_device_condition{
		char community_code[64];
		unsigned int door_status;
	};

	struct armnet_person_role{
		char person_id[32];
		unsigned char role;
	};

	struct armnet_person_tmp_pwd{
		char person_id[32];
		char pwd[32];
	};

	struct armnet_device_heartbeat{
		char community_code[64];
		char time[32];
		unsigned int status;
	};

	struct armnet_device_command{
		unsigned int type;
		char value[128];
	};

	#pragma pack (pop)

	typedef void* ARMNET_COMMAND_DATA;

	/**
	* @brief This callback is called when a error happened.
	* @param error_code is the last error code.
	*/
	typedef void(*armnet_error_callback)(enum armnet_error_code error_code);


	/**
	* @brief This callback is called when a command happened.
	* @param command_type is command type.
	*/
	typedef void(*command_callback)(ARMNET_COMMAND_DATA command);


	/**
	* @brief Initialize the Device API. This function must to be
	*     called before any other API function call.
	* @return 0 on success, or a error code.
	*/
	ARMCLIENT_API int ARMNET_Initialize();


	/**
	* @brief Finalize the Device API. This function must to be called before closing
	*    the client application to release internally allocated resources.
	*    All devices will be disconnected and the server shutdown.
	* @return 0 on success, or a error code.
	*/
	ARMCLIENT_API void ARMNET_Finalize();


	ARMCLIENT_API const char* ARMNET_GetLibVersion();
	/**
	* @brief Set the callback function.
	* @param on_error is the error callback function.
	* @param on_command is the command callback function.
	* @return 0 on success, or a error code.
	*/
	ARMCLIENT_API int ARMNET_SetCallback(armnet_error_callback on_error,
		command_callback on_command);

	/**
	* @brief  Returns a constant string with a short description of the given error code.
	* @param  error_code the error code whose description is desired.
	* @return  a short description of the error code.
	*/
	ARMCLIENT_API const char* ARMNET_StrError(int error_code);

	/**
	* @brief Connect to host.
	* @param ip is the host ip address.
	* @param port is the host port.
	* @return 0 on success, or a error code.
	*/
	ARMCLIENT_API int ARMNET_ConnectToHost(const char* ip, int port);

	/**
	* @brief Disconnect from host.
	*/
	ARMCLIENT_API void ARMNET_DisconnectFromHost();

	/**
	* 获取命令类型
	*/
	ARMCLIENT_API int ARMNET_GetCommandType(ARMNET_COMMAND_DATA command);

	/**
	* 设置设备信息命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_device_info* ARMNET_GetDeviceInfo(ARMNET_COMMAND_DATA command);

	/**
	* 获取设备信息命令对应的数据
	*/
	ARMCLIENT_API int ARMNET_SetDeviceInfo(ARMNET_COMMAND_DATA command, const struct armnet_device_info* info);

	/**
	* 设置设备时间命令对应的数据
	*/
	ARMCLIENT_API const char* ARMNET_GetDeviceTime(ARMNET_COMMAND_DATA command);

	/**
	* 获取设备配置命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_device_config* ARMNET_GetDeviceConfig(ARMNET_COMMAND_DATA command);

	/**
	* 设置设备配置命令对应的数据
	*/
	ARMCLIENT_API int ARMNET_SetDeviceConfig(ARMNET_COMMAND_DATA command, const struct armnet_device_config* config);

	/**
	* 发送自定义命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_user_command* ARMNET_GetUserCommand(ARMNET_COMMAND_DATA command);

	/**
	* 获取设备状态命令对应的数据
	*/
	ARMCLIENT_API int ARMNET_SetDeviceStatus(ARMNET_COMMAND_DATA command, int status);

	/**
	* 下载人员信息命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_person_info* ARMNET_GetDownloadPersonInfo(ARMNET_COMMAND_DATA command);
	
	/**
	* 更新人员信息命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_person_info* ARMNET_GetUpdatePersonInfo(ARMNET_COMMAND_DATA command);
	
	/**
	* 删除人员命令对应的数据
	*/
	ARMCLIENT_API const char* ARMNET_GetRemovePersonInfo(ARMNET_COMMAND_DATA command);
	
	/**
	* 设置人员黑白名单命令对应的数据
	*/
	ARMCLIENT_API const struct armnet_person_role* ARMNET_GetPersonRole(ARMNET_COMMAND_DATA command);

	/**
	* 设置用户临时密码
	*/
	ARMCLIENT_API const struct armnet_person_tmp_pwd* ARMNET_GetPersonTmpPwd(ARMNET_COMMAND_DATA command);

	/**
	*******************************重要***************************************
	* 返回处理结果，所有的命令处理完成后，都需要调用此接口！！！
	*/
	ARMCLIENT_API int ARMNET_NotifyCommandResult(ARMNET_COMMAND_DATA command, enum armnet_result_code result_code);

	/**
	* 上传考勤记录
	*/
	ARMCLIENT_API int ARMNET_UploadAccessRecord(const struct armnet_access_info* info);

	/**
	* 上传设备状态
	*/
	ARMCLIENT_API int ARMNET_UploadDeviceCondition(const struct armnet_device_condition* condition);

	/**
	* 上传人员信息记录
	*/
	ARMCLIENT_API int ARMNET_UploadPersonInfo(const struct armnet_person_info* info);

	/**
	* 上传设备心跳记录
	*/
	ARMCLIENT_API int ARMNET_UploadDeviceHeartbeat(const struct armnet_device_heartbeat* heartbeat);

	/**
	* 上传设备自定义命令
	*/
	ARMCLIENT_API int ARMNET_UploadDeviceCommand(const struct armnet_device_command* command);


#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __ARMCLIENT_API__
