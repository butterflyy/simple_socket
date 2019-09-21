
// ArmMFCDemoServerDlg.h : header file
//

#pragma once

#include "afxwin.h"


#include <MFC\HelperDlg.h>
#include <whsarmserver.h>


#define TMPL_SIZE 8192

#define JSON_REQ  1
#define JSON_ACK  2


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


#define TMPL_SIZE 8192


// CArmMFCDemoServerDlg dialog
class CArmMFCDemoServerDlg : public CDialogEx , public HelperDlg{
// Construction
public:
    CArmMFCDemoServerDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
    enum { IDD = IDD_MFCDEMO_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

	static CArmMFCDemoServerDlg* m_pThis;

	void InitDeviceList();
    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnCallbackMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboDeviceList();
	afx_msg void OnBnClickedButtonDisconnectClient();
	void ParseData(const std::string& json);
private:

	static void CALLBACK connected_callback(SS_SESSION session, const char* client_ip);
	static void CALLBACK disconnected_callback(SS_SESSION session);
	static void CALLBACK error_callback(SS_SESSION session, int error_code);
	static void CALLBACK recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type);

private:
	std::string m_imagePath;
	std::vector<std::string> m_tmplPaths;

	std::vector<ClientInfo> m_clients;
public:
	CComboBox m_cmbDeviceList;
	afx_msg void OnBnClickedButtonSetDeviceInfo();
	afx_msg void OnBnClickedButtonGetDeviceInfo();
	afx_msg void OnBnClickedButtonSendCommand();
	afx_msg void OnBnClickedButtonDownloadPerson();
	afx_msg void OnBnClickedButtonUpdatePerson();
	afx_msg void OnBnClickedButtonRemovePerson();
	afx_msg void OnBnClickedButtonOpenTmpls();
	afx_msg void OnBnClickedButtonSetPersonRole();
	afx_msg void OnBnClickedButtonSetDeviceTime();
	afx_msg void OnBnClickedButtonGetDeviceStatus();
	afx_msg void OnBnClickedButtonGetDeviceConfig();
	afx_msg void OnBnClickedButtonSetDeviceConfig();
	afx_msg void OnBnClickedButtonSetPersonTmpPwd();
	afx_msg void OnBnClickedButtonOpenImagePath();
};
