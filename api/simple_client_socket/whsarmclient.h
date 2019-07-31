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
* whsarmclient ��汾��
* ͷ�ļ������Ŀ�汾�ţ� ͨ�� SC_GetLibVersion �ӿڻ�ȡʵ�ʼ��ؿ�İ汾�š�
*/
#define SCAPI_VERSION "1.0.0"

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	enum sc_error_code {
		/** �����ɹ� */
		SC_SUCCESS = 0,

		/** һ����� */
		SC_ERROR = -1,

		/** ������Ч */
		SC_INVALID_PARAM = -2,

		/** ������� */
		SC_NETWORK_ERROR = -3,

		/** ����Ͽ� */
		SC_NETWORK_DISCONNECTED = -4,

		/** ���糬ʱ */
		SC_NETWORK_TIMEOUT = -5,

		/** ����֡����1MB */
		SC_PAYLOAD_TOO_BIG = -6,

		/** ����֡���� */
		SC_FRAME_ERROR = -7,
	};

	/** ����֡���� */
	enum sc_frame_type{
		/* �ı����� */
		SC_FRAME_STRING = 1,

		/* ���������� */
		SC_FRAME_BINARY = 2,
	};

	/**
	* ������ ���ӶϿ��󣬴������¼��ص���
	*/
	typedef void(CALLBACK *sc_disconnected_callback)();


	/**
	* ������ ���ӳ��ִ���󣬴������¼��ص���
	* ������ error_code �����룬�ο� sc_error_code��
	*/
	typedef void(CALLBACK *sc_error_callback)(int error_code);

	/**
	* ������ ���ܵ�����������֡�󣬴������¼��ص���
	* ������ data ����֡��ָ�롣
	* ������ len ����֡���ȡ�
	* ������ type ����֡���ͣ� �ο� sc_frame_type��
	*/
	typedef void(CALLBACK *sc_recvframe_callback)(const unsigned char* data, int len, int type);


	/**
	* ������ ��ʼ����Դ���������������е�API֮ǰ���á�
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� sc_error_code��
	*/
	SC_API int WINAPI SC_Initialize();

	/**
	* ������ �ͷ��豸��Դ���� SC_Initialize ��Ӧ���������ʱ���ã��ر��������Ӳ��ͷ���Դ��
	*/
	SC_API void WINAPI SC_Finalize();

	/**
	* ������ ��ȡ��汾�š�
	* ���أ� �汾�������ַ��������� 1.3.2��
	*/
	SC_API const char* WINAPI SC_GetLibVersion();

	/**
	* ������ ���ش�����ļ��ַ���������
	* ������ error_code �����룬�ο� sc_error_code��
	* ���أ� �����ַ������������� -1������ ��SC_ERROR����
	*/
	SC_API const char* WINAPI SC_StrError(int error_code);

	/**
	* ������ ���ûص�������SC_Initialize �ɹ�����á�
	* ������ on_disconnected ���ӶϿ��ص�����ָ�롣
	* ������ on_error ���Ӵ���ص�����ָ�롣
	* ������ on_recvframe ����������֡�ص�����ָ�롣
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� sc_error_code��
	*/
	SC_API int WINAPI SC_SetCallback(sc_disconnected_callback on_disconnected,
		sc_error_callback on_error,
		sc_recvframe_callback on_recvframe);

	/**
	* ������ ���ӷ�������
	* ������ ip ������IP��ַ��
	* ������ port �������˿ںš�
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� sc_error_code��
	*/
	SC_API int WINAPI SC_ConnectToHost(const char* ip, int port);

	/**
	* ������ �Ͽ����ӡ�
	*/
	SC_API void WINAPI SC_DisconnectFromHost();

	/**
	* ������ �������ݸ���������
	* ������ data ����֡��ָ�롣
	* ������ len ����֡���ȡ�
	* ������ type ����֡���ͣ� �ο� sc_frame_type��
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� sc_error_code��
	*/
	SC_API int WINAPI SC_SendFrame(const unsigned char* data, int len, int type);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __SC_API__
