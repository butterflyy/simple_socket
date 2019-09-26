
// ArmMFCDemoClientDlg.h : header file
//

#pragma once

#include <armclientapi.h>
#include <string>

// CArmMFCDemoClientDlg dialog
class CArmMFCDemoClientDlg : public CDialogEx
{
// Construction
public:
	CArmMFCDemoClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCDEMO_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	static CArmMFCDemoClientDlg* m_pThis;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();

private:
	std::string GetDlgText(UINT id);
	void SetDlgText(UINT id, const std::string str);
	int GetDlgInt(UINT id);
	void SetDlgInt(UINT id, int i);
	void ShowMessage(const std::string& msg, int ret = 0) const;

private:
	static void ErrorCallback(enum armnet_error_code error_code);

	static void CommandCallback(ARMNET_COMMAND_DATA command);

	static void appendMsg(const char* msg);
public:
	afx_msg void OnBnClickedButtonUploadAccessRecord();
	afx_msg void OnBnClickedButtonUploadDeviceCondition();
	afx_msg void OnBnClickedButtonUploadDeviceHeartbeat();
	afx_msg void OnBnClickedButtonUploadDeviceCommand();
	afx_msg void OnBnClickedButtonUploadPerson();
	afx_msg void OnBnClickedButtonUploadCapture();
	afx_msg void OnBnClickedButtonUploadMatchResult();
	afx_msg void OnBnClickedButtonUploadPwd();
};
