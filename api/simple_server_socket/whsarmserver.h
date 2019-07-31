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
* whsarmserver ��汾��
* ͷ�ļ������Ŀ�汾�ţ� ͨ�� SS_GetLibVersion �ӿڻ�ȡʵ�ʼ��ؿ�İ汾�š�
*/
#define SSAPI_VERSION "1.0.0"

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	/** ������ */
	enum ss_error_code {
		/** �����ɹ� */
		SS_SUCCESS = 0,

		/** һ����� */
		SS_ERROR = -1,

		/** ������Ч */
		SS_INVALID_PARAM = -2,

		/** �������*/
		SS_NETWORK_ERROR = -3,

		/** ����Ͽ� */
		SS_NETWORK_DISCONNECTED = -4,

		/** ���糬ʱ */
		SS_NETWORK_TIMEOUT = -5,

		/** ����֡����1MB */
		SS_PAYLOAD_TOO_BIG = -6,

		/** ����֡���� */
		SS_FRAME_ERROR = -7,
	};

	/** ����֡���� */
	enum ss_frame_type{
		/* �ı����� */
		SS_FRAME_STRING = 1,

		/* ���������� */
		SS_FRAME_BINARY = 2,
	};

	/** ���ӻỰ����ָ�� */
	typedef void* SS_SESSION;


	/**
	* ������ �ͻ������Ӻ󣬴������¼��ص���
	* ������ session �ͻ������ӻỰ����
	* ������ client_ip �ͻ���IP��ַ��
	*/
	typedef void(CALLBACK *ss_connected_callback)(SS_SESSION session, const char* client_ip);

	/**
	* ������ �ͻ��˶Ͽ��󣬴������¼��ص���
	* ������ session �ͻ������ӻỰ����
	*/
	typedef void(CALLBACK *ss_disconnected_callback)(SS_SESSION session);


	/**
	* ������ ���ӳ��ִ���󣬴������¼��ص���
	* ������ session �ͻ������ӻỰ����
	* ������ error_code �����룬�ο� ss_error_code��
	*/
	typedef void(CALLBACK *ss_error_callback)(SS_SESSION session, int error_code);

	/**
	* ������ ���ܵ��ͻ�������֡�󣬴������¼��ص���
	* ������ session �ͻ������ӻỰ����
	* ������ data ����֡��ָ�롣
	* ������ len ����֡���ȡ�
	* ������ type ����֡���ͣ� �ο� ss_frame_type��
	*/
	typedef void(CALLBACK *ss_recvframe_callback)(SS_SESSION session, const unsigned char* data, int len, int type);


	/**
	* ������ ��ʼ����Դ���������������е�API֮ǰ���á�
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� ss_error_code��
	*/
	SS_API int WINAPI SS_Initialize();

	/**
	* ������ �ͷ��豸��Դ���� SS_Initialize ��Ӧ���������ʱ���ã��ر��������Ӳ��ͷ���Դ��
	*/
	SS_API void WINAPI SS_Finalize();

	/**
	* ������ ��ȡ��汾�š�
	* ���أ� �汾�������ַ��������� 1.3.2��
	*/
	SS_API const char* WINAPI SS_GetLibVersion();

	/**
	* ������ ���ش�����ļ��ַ���������
	* ������ error_code �����룬�ο� ss_error_code��
	* ���أ� �����ַ������������� -1������ ��SS_ERROR����
	*/
	SS_API const char* WINAPI SS_StrError(int error_code);

	/**
	* ������ ���ûص�������SS_Initialize �ɹ�����á�
	* ������ on_connected �ͻ������Ӻ���ָ�롣
	* ������ on_disconnected �ͻ��˶Ͽ��ص�����ָ�롣
	* ������ on_error �ͻ������Ӵ���ص�����ָ�롣
	* ������ on_recvframe �ͻ�������֡�ص�����ָ�롣
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� ss_error_code��
	*/
	SS_API int WINAPI SS_SetCallback(ss_connected_callback on_connected,
		ss_disconnected_callback on_disconnected,
		ss_error_callback on_error,
		ss_recvframe_callback on_recvframe);

	/**
	* ������ ������������������
	* ������ port �������˿ںš�
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� ss_error_code��
	*/
	SS_API int WINAPI SS_StartServer(int port);


	/**
	* ������ �رշ�������������
	*/
	SS_API void WINAPI SS_StopServer();


	/**
	* ������ �Ͽ��ͻ������ӡ�
	* ������ session �ͻ������ӻỰ����
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� ss_error_code��
	*/
	SS_API int WINAPI SS_DisconnectClient(SS_SESSION session);

	/**
	* ������ �������ݸ��ͻ��ˡ�
	* ������ session �ͻ������ӻỰ����
	* ������ data ����֡��ָ�롣
	* ������ len ����֡���ȡ�
	* ������ type ����֡���ͣ� �ο� ss_frame_type��
	* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� ss_error_code��
	*/
	SS_API int WINAPI SS_SendFrame(SS_SESSION session, const unsigned char* data, int len, int type);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __SS_API__
