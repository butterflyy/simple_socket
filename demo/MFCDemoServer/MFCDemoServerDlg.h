
// MFCDemoServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <common\utils.h>
#include <common\SignalManager.h>
#include <whsarmserver.h>

#define WM_CALLBACK_MSG   WM_USER  + 101

enum CallbackCommand{
	COMMAND_CONNECTED,
	COMMAND_DISCONNECTED,
	COMMAND_ERROR,
	COMMAND_RECVFRAME,
};

struct CallbackMsg
{
	SS_SESSION session;
	union
	{
		char client_ip[20]; //connected client ip
		int error_code; // error code
		struct FrameData{  //recv frame data
			unsigned char* data;
			int len;
			int type;
		}frame_data;
	}udata;
};

struct ClientInfo{
	SS_SESSION session;
	std::string client_ip;
};

// CMFCDemoServerDlg dialog
class CMFCDemoServerDlg : public CDialogEx
{
// Construction
public:
	CMFCDemoServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCDEMOSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	static CMFCDemoServerDlg* m_pThis;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnCallbackMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSendCommand();
	afx_msg void OnBnClickedButtonOpenImagePath();
	afx_msg void OnBnClickedButtonOpenTmpls();
	afx_msg void OnBnClickedButtonClearRecv();
	afx_msg void OnBnClickedButtonClearSend();
	DECLARE_MESSAGE_MAP()

	void ShowMessage(const std::string& msg);
	void InitClientList();
	void appendRecvMsg(const std::string& msg);
	void appendSendMsg(const std::string& msg);
	std::string GetClientIp(SS_SESSION session);

	int SendStringFrame(SS_SESSION session, const std::string& str);
	int SendBinaryFrame(SS_SESSION session, const byte* data, int len);
public:
	CComboBox m_cmbClientList;
	CComboBox m_cmbCommand;
	std::vector<ClientInfo> m_clients;

	std::string m_imagePath;
	std::vector<std::string> m_tmplPaths;
public:
	static void CALLBACK connected_callback(SS_SESSION session, const char* client_ip);
	static void CALLBACK disconnected_callback(SS_SESSION session);
	static void CALLBACK error_callback(SS_SESSION session, int error_code);
	static void CALLBACK recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type);
};


inline void CMFCDemoServerDlg::ShowMessage(const std::string& msg){
	MessageBoxA(GetSafeHwnd(), msg.c_str(), "��ʾ", MB_OK);
}

inline int CMFCDemoServerDlg::SendStringFrame(SS_SESSION session, const std::string& str){
	return SS_SendFrame(session, (byte*)str.c_str(), str.size(), SS_FRAME_STRING);
}

inline int CMFCDemoServerDlg::SendBinaryFrame(SS_SESSION session, const byte* data, int len){
	return SS_SendFrame(session, data, len, SS_FRAME_BINARY);
}
