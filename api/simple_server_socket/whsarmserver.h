#ifndef __SS_API__
#define __SS_API__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(SS_API_EXPORTS)
# define SS_API
#else
# define SS_API __declspec(dllimport)
#endif
# define CALLBACK    __stdcall
# define WINAPI      __stdcall
#else
# define SS_API
# define CALLBACK
# define WINAPI
#endif


/**
* whsarmserver 库版本号
* 头文件声明的库版本号， 通过 SS_GetLibVersion 接口获取实际加载库的版本号。
*/
#define SSAPI_VERSION "1.2.0"

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	/** 错误码 */
	enum ss_error_code {
		/** 操作成功 */
		SS_SUCCESS = 0,

		/** 一般错误 */
		SS_ERROR = -1,

		/** 参数无效 */
		SS_INVALID_PARAM = -2,

		/** 网络错误*/
		SS_NETWORK_ERROR = -3,

		/** 网络断开 */
		SS_NETWORK_DISCONNECTED = -4,

		/** 网络超时 */
		SS_NETWORK_TIMEOUT = -5,

		/** 数据帧过大，默认最大不超过5MB，通过配置文件可以修改 */
		SS_PAYLOAD_TOO_BIG = -6,

		/** 数据帧错误 */
		SS_FRAME_ERROR = -7,
	};

	/** 数据帧类型 */
	enum ss_frame_type{
		/* 文本数据 */
		SS_FRAME_STRING = 1,

		/* 二进制数据 */
		SS_FRAME_BINARY = 2,
	};

	/** 服务器对象
	 *  当监听不同的端口，启动多个服务器时，标识不同的服务器。
	 *  通过比较这个值，可以区分不同的服务器。
	 */
	typedef void* SS_SERVER;

	/** 客户端连接会话对象
	 *  多个客户端连接服务器后，标识不同的客户端。
	 *  通过比较这个值，可以区分不同的客户端。
	 */
	typedef void* SS_SESSION;


	/**
	* 描述： 客户端连接后，触发此事件回调。
	* 参数： server 服务器对象。
	* 参数： session 客户端连接会话对象。
	* 参数： client_ip 客户端IP地址。
	* 参数： client_port 客户端端口号。
	*/
	typedef void(CALLBACK *ss_connected_callback)(SS_SERVER server, SS_SESSION session, const char* client_ip, int client_port);

	/**
	* 描述： 客户端断开后，触发此事件回调。
	* 参数： session 客户端连接会话对象。
	*/
	typedef void(CALLBACK *ss_disconnected_callback)(SS_SESSION session);


	/**
	* 描述： 连接出现错误后，触发此事件回调。
	* 参数： session 客户端连接会话对象。
	* 参数： error_code 错误码，参考 ss_error_code。
	*/
	typedef void(CALLBACK *ss_error_callback)(SS_SESSION session, int error_code);

	/**
	* 描述： 接受到客户端数据帧后，触发此事件回调。
	* 参数： session 客户端连接会话对象。
	* 参数： data 数据帧的指针。
	* 参数： len 数据帧长度。
	* 参数： type 数据帧类型， 参考 ss_frame_type。
	*/
	typedef void(CALLBACK *ss_recvframe_callback)(SS_SESSION session, const unsigned char* data, int len, int type);


	/**
	* 描述： 初始化资源，必须在其他所有的API之前调用。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_Initialize();

	/**
	* 描述： 释放设备资源，和 SS_Initialize 对应，程序结束时调用，关闭所有连接并释放资源。
	*/
	SS_API void WINAPI SS_Finalize();

	/**
	* 描述： 获取库版本号。
	* 返回： 版本号描述字符串，例如 1.3.2。
	*/
	SS_API const char* WINAPI SS_GetLibVersion();

	/**
	* 描述： 返回错误码的简单字符串描述。
	* 参数： error_code 错误码，参考 ss_error_code。
	* 返回： 错误字符串描述，例如 -1，返回 “SS_ERROR”。
	*/
	SS_API const char* WINAPI SS_StrError(int error_code);

	/**
	* 描述： 设置回调函数，SS_Initialize 成功后调用。
	* 参数： on_connected 客户端连接函数指针。
	* 参数： on_disconnected 客户端断开回调函数指针。
	* 参数： on_error 客户端连接错误回调函数指针。
	* 参数： on_recvframe 客户端数据帧回调函数指针。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_SetCallback(ss_connected_callback on_connected,
		ss_disconnected_callback on_disconnected,
		ss_error_callback on_error,
		ss_recvframe_callback on_recvframe);

	/**
	* 描述： 启动服务器监听服务。
	* 参数： port 服务器端口号。
	* 参数： server [out] 服务器对象，启动服务器成功后获取到的服务器对象。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_StartServer(int port, SS_SERVER* server);


	/**
	* 描述： 关闭服务器监听服务。
	* 参数： server 服务器对象。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_StopServer(SS_SERVER server);


	/**
	* 描述： 断开客户端连接。
	* 参数： session 客户端连接会话对象。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_DisconnectClient(SS_SESSION session);

	/**
	* 描述： 发送数据给客户端。
	*       线程安全，支持并发发送数据操作。
	* 参数： session 客户端连接会话对象。
	* 参数： data 数据帧的指针。
	* 参数： len 数据帧长度，默认最大不超过5MB，通过配置文件可以修改。
	* 参数： type 数据帧类型， 参考 ss_frame_type。
	* 返回： 0 成功，其他值失败，参考 ss_error_code。
	*/
	SS_API int WINAPI SS_SendFrame(SS_SESSION session, const unsigned char* data, int len, int type);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __SS_API__
