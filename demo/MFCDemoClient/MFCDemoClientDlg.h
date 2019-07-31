
// MFCDemoClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <common\utils.h>
#include <whsarmclient.h>


#define WM_CALLBACK_MSG   WM_USER  + 101

enum CallbackCommand{
	COMMAND_DISCONNECTED,
	COMMAND_ERROR,
	COMMAND_RECVFRAME,
};

struct CallbackMsg
{
	union
	{
		int error_code; // error code
		struct FrameData{  //recv frame data
			unsigned char* data;
			int len;
			int type;
		}frame_data;
	}udata;
};


// CMFCDemoClientDlg dialog
class CMFCDemoClientDlg : public CDialogEx
{
// Construction
public:
	CMFCDemoClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCDEMOCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	static CMFCDemoClientDlg* m_pThis;

	CComboBox m_cmbCommand;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnCallbackMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonSendCommand();
	afx_msg void OnBnClickedButtonClearRecv();
	afx_msg void OnBnClickedButtonClearSend();
	DECLARE_MESSAGE_MAP()
public:
	void ShowMessage(const std::string& msg);
	void appendRecvMsg(const std::string& msg);
	void appendSendMsg(const std::string& msg);

	int SendStringFrame(const std::string& str);
	int SendBinaryFrame(const byte* data, int len);
public:
	static void CALLBACK disconnected_callback();
	static void CALLBACK error_callback(int error_code);
	static void CALLBACK recvframe_callback(const unsigned char* data, int len, int type);
};

inline void CMFCDemoClientDlg::ShowMessage(const std::string& msg){
	MessageBoxA(GetSafeHwnd(), msg.c_str(), "ÌבÊ¾", MB_OK);
}

inline int CMFCDemoClientDlg::SendStringFrame(const std::string& str){
	return SC_SendFrame((byte*)str.c_str(), str.size(), SC_FRAME_STRING);
}

inline int CMFCDemoClientDlg::SendBinaryFrame(const byte* data, int len){
	return SC_SendFrame(data, len, SC_FRAME_BINARY);
}
