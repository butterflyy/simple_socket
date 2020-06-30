
// MFCDemoClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCDemoClient.h"
#include "MFCDemoClientDlg.h"
#include "afxdialogex.h"
#include "JsonTestDataParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PORT  49877

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDemoClientDlg dialog


CMFCDemoClientDlg* CMFCDemoClientDlg::m_pThis = NULL;
CMFCDemoClientDlg::CMFCDemoClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCDemoClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pThis = this;
}

void CMFCDemoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_cmbCommand);
}

BEGIN_MESSAGE_MAP(CMFCDemoClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CALLBACK_MSG, OnCallbackMsg)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMFCDemoClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CMFCDemoClientDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND, &CMFCDemoClientDlg::OnBnClickedButtonSendCommand)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RECV, &CMFCDemoClientDlg::OnBnClickedButtonClearRecv)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SEND, &CMFCDemoClientDlg::OnBnClickedButtonClearSend)
END_MESSAGE_MAP()


// CMFCDemoClientDlg message handlers

BOOL CMFCDemoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	std::string title = utils::StrFormat("客户端演示程序 -库版本: V%s", SC_GetLibVersion());
	SetWindowTextA(GetSafeHwnd(), title.c_str());

	SetDlgItemText(IDC_EDIT_IP, L"127.0.0.1");

	m_cmbCommand.AddString(L"上传考勤");
	m_cmbCommand.AddString(L"上传设备信息");
	m_cmbCommand.SetCurSel(0);


	int ret = SC_Initialize();
	if (ret < 0){
		ShowMessage(utils::StrFormat("初始化失败， err = %s", SC_StrError(ret)));
		return TRUE;
	}

	ret = SC_SetCallback(disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0){
		ShowMessage(utils::StrFormat("设置回调失败， err = %s", SC_StrError(ret)));
		return TRUE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDemoClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCDemoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCDemoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCDemoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	SC_Finalize();
}

void CMFCDemoClientDlg::appendRecvMsg(const std::string& msg){
	std::string allMsg;

	static char buff[10240];
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_RECV, buff, 10240);

	if (strlen(buff) <= 10140){
		allMsg = buff;
	}

	if (msg.size() >= 1000){
		std::string newmsg = msg.substr(0, 1000) + ".............";
		allMsg += newmsg;
	}
	else{
		allMsg += msg;
	}
	allMsg += "\r\n";

	SetDlgItemTextA(m_pThis->m_hWnd, IDC_EDIT_RECV, allMsg.c_str());
}

void CMFCDemoClientDlg::appendSendMsg(const std::string& msg){
	std::string allMsg;

	static char buff[10240];
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_SEND, buff, 10240);

	if (strlen(buff) <= 10140){
		allMsg = buff;
	}

	if (msg.size() >= 1000){
		std::string newmsg = msg.substr(0, 1000) + ".............";
		allMsg += newmsg;
	}
	else{
		allMsg += msg;
	}
	allMsg += "\r\n";

	SetDlgItemTextA(m_pThis->m_hWnd, IDC_EDIT_SEND, allMsg.c_str());
}

void CMFCDemoClientDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	char buff[100];
	GetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_IP, buff, 100);
	std::string ip(buff);
	if (ip.empty()){
		ShowMessage("IP地址为空！");
		return;
	}

	int ret = SC_ConnectToHost(ip.c_str(), PORT);
	if (ret < 0){
		ShowMessage(utils::StrFormat("连接服务器失败， err = %s", SC_StrError(ret)));
		return;
	}
}


void CMFCDemoClientDlg::OnBnClickedButtonDisconnect()
{
	// TODO: Add your control notification handler code here
	SC_DisconnectFromHost();
}

afx_msg LRESULT CMFCDemoClientDlg::OnCallbackMsg(WPARAM wParam, LPARAM lParam){
	CallbackMsg* pMsg = (CallbackMsg*)lParam;
	switch (wParam)
	{
	case COMMAND_DISCONNECTED:{
		ShowMessage(utils::StrFormat("设备断开"));
	}
		break;
	case COMMAND_ERROR:{
		ShowMessage(utils::StrFormat("设备错误"));
	}
		break;
	case COMMAND_RECVFRAME:{
		switch (pMsg->udata.frame_data.type){
		case SC_FRAME_STRING:{
				appendRecvMsg(utils::StrFormat("******************[string] [%d byte]*****************", pMsg->udata.frame_data.len));
				std::string str((char*)pMsg->udata.frame_data.data, pMsg->udata.frame_data.len);
				appendRecvMsg(str);

				//处理请求
				std::string mod;
				std::string session_id;
				int type;
				JsonTestDataParse::Parse(str, mod, session_id, type);
				if (type == JSON_REQ){
					std::string ackjson;

					if (mod == "get_device_info"){
						ackjson = JsonTestDataParse::MakeResultDeviceInfo(session_id);
					}
					else if (mod == "download_person"){
						std::string imagePath;
						std::vector<std::string> tmplPaths;

						if (!JsonTestDataParse::SaveDownloadPersonImage(str, imagePath, tmplPaths)){
							ShowMessage("解析下载人员数据错误！");
							return 0;
						}

						appendRecvMsg(utils::StrFormat("保存下载人员图片(d:\\face.bmp), 模板(d:\\tmpl0-%d.tmp)", tmplPaths.size()));
						ackjson = JsonTestDataParse::MakeResult(mod, session_id);
					}
					else{
						assert(false);
					}

					int ret = SendStringFrame(ackjson);
					if (ret < 0){
						ShowMessage(utils::StrFormat("发送数据失败， err = %s", SC_StrError(ret)));
						return 0;
					}

					appendSendMsg(utils::StrFormat("******************[string] [%d byte]*****************", ackjson.size()));
					appendSendMsg(ackjson);
				}

			}
			break;
		case SC_FRAME_BINARY:{
			appendRecvMsg(utils::StrFormat("******************[binary] [%d byte]*****************", pMsg->udata.frame_data.len));
			std::string str((char*)pMsg->udata.frame_data.data, pMsg->udata.frame_data.len);
			appendRecvMsg(str);
			}
			break;
		default:assert(false); break;
			}
		delete[] pMsg->udata.frame_data.data;
	}
		break;
	default:
		assert(false);
		break;
	}

	delete pMsg;
	return 0;
}

void CALLBACK CMFCDemoClientDlg::disconnected_callback(){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_DISCONNECTED, (LPARAM)pMsg);
}

void CALLBACK CMFCDemoClientDlg::error_callback(int error_code){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->udata.error_code = error_code;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_ERROR, (LPARAM)pMsg);
}

void CALLBACK CMFCDemoClientDlg::recvframe_callback(const unsigned char* data, int len, int type){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->udata.frame_data.data = new byte[len];
	memcpy(pMsg->udata.frame_data.data, data, len);
	pMsg->udata.frame_data.len = len;
	pMsg->udata.frame_data.type = type;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_RECVFRAME, (LPARAM)pMsg);
}

void CMFCDemoClientDlg::OnBnClickedButtonSendCommand()
{
	// TODO: Add your control notification handler code here
	int cur_command = m_cmbCommand.GetCurSel();

	std::string json;
	switch (cur_command)
	{
	case 0: json = JsonTestDataParse::MakeUploadAccessInfo(); break;
	case 1: json = JsonTestDataParse::MakeUploadDeviceStatus(); break;
	default: assert(false); break;
	}

	int ret = SendStringFrame(json);
	if (ret < 0){
		ShowMessage(utils::StrFormat("发送数据失败， err = %s", SC_StrError(ret)));
		return;
	}

	appendSendMsg(utils::StrFormat("******************[string] [%d byte]*****************", json.size()));
	appendSendMsg(json);
}


void CMFCDemoClientDlg::OnBnClickedButtonClearRecv()
{
	SetDlgItemText(IDC_EDIT_RECV, L"");
}


void CMFCDemoClientDlg::OnBnClickedButtonClearSend()
{
	SetDlgItemText(IDC_EDIT_SEND, L"");
}
