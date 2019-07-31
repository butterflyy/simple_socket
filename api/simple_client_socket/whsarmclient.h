#ifndef __SC_API__
#define __SC_API__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(SC_API_EXPORTS)
# define SC_API
#else
# define SC_API __declspec(dllimport)
#endif
# define CALLBACK    __stdcall
# define WINAPI      __stdcall
#else
# define SC_API
# define CALLBACK
# define WINAPI
#endif


/**
* whsarmclient 库版本号
* 头文件声明的库版本号， 通过 SC_GetLibVersion 接口获取实际加载库的版本号。
*/
#define SCAPI_VERSION "1.0.0"

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	enum sc_error_code {
		/** 操作成功 */
		SC_SUCCESS = 0,

		/** 一般错误 */
		SC_ERROR = -1,

		/** 参数无效 */
		SC_INVALID_PARAM = -2,

		/** 网络错误 */
		SC_NETWORK_ERROR = -3,

		/** 网络断开 */
		SC_NETWORK_DISCONNECTED = -4,

		/** 网络超时 */
		SC_NETWORK_TIMEOUT = -5,

		/** 数据帧超过1MB */
		SC_PAYLOAD_TOO_BIG = -6,

		/** 数据帧错误 */
		SC_FRAME_ERROR = -7,
	};

	/** 数据帧类型 */
	enum sc_frame_type{
		/* 文本数据 */
		SC_FRAME_STRING = 1,

		/* 二进制数据 */
		SC_FRAME_BINARY = 2,
	};

	/**
	* 描述： 连接断开后，触发此事件回调。
	*/
	typedef void(CALLBACK *sc_disconnected_callback)();


	/**
	* 描述： 连接出现错误后，触发此事件回调。
	* 参数： error_code 错误码，参考 sc_error_code。
	*/
	typedef void(CALLBACK *sc_error_callback)(int error_code);

	/**
	* 描述： 接受到服务器数据帧后，触发此事件回调。
	* 参数： data 数据帧的指针。
	* 参数： len 数据帧长度。
	* 参数： type 数据帧类型， 参考 sc_frame_type。
	*/
	typedef void(CALLBACK *sc_recvframe_callback)(const unsigned char* data, int len, int type);


	/**
	* 描述： 初始化资源，必须在其他所有的API之前调用。
	* 返回： 0 成功，其他值失败，参考 sc_error_code。
	*/
	SC_API int WINAPI SC_Initialize();

	/**
	* 描述： 释放设备资源，和 SC_Initialize 对应，程序结束时调用，关闭所有连接并释放资源。
	*/
	SC_API void WINAPI SC_Finalize();

	/**
	* 描述： 获取库版本号。
	* 返回： 版本号描述字符串，例如 1.3.2。
	*/
	SC_API const char* WINAPI SC_GetLibVersion();

	/**
	* 描述： 返回错误码的简单字符串描述。
	* 参数： error_code 错误码，参考 sc_error_code。
	* 返回： 错误字符串描述，例如 -1，返回 “SC_ERROR”。
	*/
	SC_API const char* WINAPI SC_StrError(int error_code);

	/**
	* 描述： 设置回调函数，SC_Initialize 成功后调用。
	* 参数： on_disconnected 连接断开回调函数指针。
	* 参数： on_error 连接错误回调函数指针。
	* 参数： on_recvframe 服务器数据帧回调函数指针。
	* 返回： 0 成功，其他值失败，参考 sc_error_code。
	*/
	SC_API int WINAPI SC_SetCallback(sc_disconnected_callback on_disconnected,
		sc_error_callback on_error,
		sc_recvframe_callback on_recvframe);

	/**
	* 描述： 连接服务器。
	* 参数： ip 服务器IP地址。
	* 参数： port 服务器端口号。
	* 返回： 0 成功，其他值失败，参考 sc_error_code。
	*/
	SC_API int WINAPI SC_ConnectToHost(const char* ip, int port);

	/**
	* 描述： 断开连接。
	*/
	SC_API void WINAPI SC_DisconnectFromHost();

	/**
	* 描述： 发送数据给服务器。
	* 参数： data 数据帧的指针。
	* 参数： len 数据帧长度。
	* 参数： type 数据帧类型， 参考 sc_frame_type。
	* 返回： 0 成功，其他值失败，参考 sc_error_code。
	*/
	SC_API int WINAPI SC_SendFrame(const unsigned char* data, int len, int type);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __SC_API__
