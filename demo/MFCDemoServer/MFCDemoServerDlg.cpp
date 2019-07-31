
// MFCDemoServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCDemoServer.h"
#include "MFCDemoServerDlg.h"
#include "afxdialogex.h"
#include "JsonTestDataParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define PORT  39877

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


// CMFCDemoServerDlg dialog


CMFCDemoServerDlg* CMFCDemoServerDlg::m_pThis = NULL;
CMFCDemoServerDlg::CMFCDemoServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCDemoServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pThis = this;
}

void CMFCDemoServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CLIENT, m_cmbClientList);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_cmbCommand);
}

BEGIN_MESSAGE_MAP(CMFCDemoServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CALLBACK_MSG, OnCallbackMsg)
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND, &CMFCDemoServerDlg::OnBnClickedButtonSendCommand)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_IMAGE_PATH, &CMFCDemoServerDlg::OnBnClickedButtonOpenImagePath)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_TMPLS, &CMFCDemoServerDlg::OnBnClickedButtonOpenTmpls)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RECV, &CMFCDemoServerDlg::OnBnClickedButtonClearRecv)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SEND, &CMFCDemoServerDlg::OnBnClickedButtonClearSend)
END_MESSAGE_MAP()


// CMFCDemoServerDlg message handlers

BOOL CMFCDemoServerDlg::OnInitDialog()
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

	std::string title = utils::StrFormat("�������ʾ���� -��汾: V%s", SS_GetLibVersion());
	SetWindowTextA(GetSafeHwnd(), title.c_str());

	m_cmbCommand.AddString(L"��ȡ�豸��Ϣ");
	m_cmbCommand.AddString(L"������Ա");
	m_cmbCommand.SetCurSel(0);

	int ret = SS_Initialize();
	if (ret < 0){
		ShowMessage(utils::StrFormat("��ʼ��ʧ�ܣ� err = %s", SS_StrError(ret)));
		return TRUE;
	}

	ret = SS_SetCallback(connected_callback, disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0){
		ShowMessage(utils::StrFormat("���ûص�ʧ�ܣ� err = %s", SS_StrError(ret)));
		return TRUE;
	}

	ret = SS_StartServer(PORT);
	if (ret < 0){
		ShowMessage(utils::StrFormat("��������ʧ�ܣ� err = %s", SS_StrError(ret)));
		return TRUE;
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDemoServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	SS_StopServer();

	SS_Finalize();
}


void CMFCDemoServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCDemoServerDlg::OnPaint()
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
HCURSOR CMFCDemoServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDemoServerDlg::InitClientList(){
	m_cmbClientList.ResetContent();

	for (std::vector<ClientInfo>::const_iterator it = m_clients.begin();
		it != m_clients.end();
		++it){
		m_cmbClientList.AddString(utils::s2ws(it->client_ip).c_str());
	}

	m_cmbClientList.SetCurSel(0);
}

void CMFCDemoServerDlg::appendRecvMsg(const std::string& msg){
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

void CMFCDemoServerDlg::appendSendMsg(const std::string& msg){
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


std::string CMFCDemoServerDlg::GetClientIp(SS_SESSION session){
	for (std::vector<ClientInfo>::const_iterator it = m_clients.begin();
		it != m_clients.end();
		++it){
		if (it->session == session){
			return it->client_ip;
		}
	}
	assert(false);
	return "";
}

afx_msg LRESULT CMFCDemoServerDlg::OnCallbackMsg(WPARAM wParam, LPARAM lParam){
	CallbackMsg* pMsg = (CallbackMsg*)lParam;
	switch (wParam)
	{
	case COMMAND_CONNECTED:{
							   ClientInfo info;
							   info.session = pMsg->session;
							   info.client_ip = pMsg->udata.client_ip;
							   m_clients.push_back(info);
							   InitClientList();
							   
							   ShowMessage(utils::StrFormat("�豸����"));
	}
		break;
	case COMMAND_DISCONNECTED:{
								  std::vector<ClientInfo> clients;
								  for (std::vector<ClientInfo>::const_iterator it = m_clients.begin();
									  it != m_clients.end();
									  ++it){
									  if (it->session != pMsg->session){
										  clients.push_back(*it);
									  }
								  }

								  m_clients = clients;
								  InitClientList();

								  ShowMessage(utils::StrFormat("�豸�Ͽ�"));
	}
		break;
	case COMMAND_ERROR:{
						   ShowMessage(utils::StrFormat("�豸����"));
	}
		break;
	case COMMAND_RECVFRAME:{
							   std::string client_ip = GetClientIp(pMsg->session);
							   switch (pMsg->udata.frame_data.type){
							   case SS_FRAME_STRING:{
									appendRecvMsg(utils::StrFormat("******************%s [string] [%d byte]*****************", 
										client_ip.c_str(), pMsg->udata.frame_data.len));
									std::string str((char*)pMsg->udata.frame_data.data, pMsg->udata.frame_data.len);
									appendRecvMsg(str);

									//��������
									std::string mod;
									std::string session_id;
									int type;
									JsonTestDataParse::Parse(str, mod, session_id, type);

									if (type == JSON_REQ){
										std::string ackjson = JsonTestDataParse::MakeResult(mod, session_id);

										int ret = SendStringFrame(pMsg->session, ackjson);
										if (ret < 0){
											ShowMessage(utils::StrFormat("��������ʧ�ܣ� err = %s", SS_StrError(ret)));
											return 0;
										}

										appendSendMsg(utils::StrFormat("******************%s [string] [%d byte]*****************", 
											client_ip.c_str(), ackjson.size()));
										appendSendMsg(ackjson);
									}

							   }
								   break;
							   case SS_FRAME_BINARY:{
									appendRecvMsg(utils::StrFormat("******************%s [binary] [%d byte]*****************", 
										client_ip.c_str(), pMsg->udata.frame_data.len));
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

void CALLBACK CMFCDemoServerDlg::connected_callback(SS_SESSION session, const char* client_ip){
	//ͬ���ص����̣���������
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	strcpy_s(pMsg->udata.client_ip, client_ip);

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_CONNECTED, (LPARAM)pMsg);
}

void CALLBACK CMFCDemoServerDlg::disconnected_callback(SS_SESSION session){
	//ͬ���ص����̣���������
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_DISCONNECTED, (LPARAM)pMsg);
}

void CALLBACK CMFCDemoServerDlg::error_callback(SS_SESSION session, int error_code){
	//ͬ���ص����̣���������
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	pMsg->udata.error_code = error_code;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_ERROR, (LPARAM)pMsg);
}

void CALLBACK CMFCDemoServerDlg::recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type){
	//ͬ���ص����̣���������
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	pMsg->udata.frame_data.data = new byte[len];
	memcpy(pMsg->udata.frame_data.data, data, len);
	pMsg->udata.frame_data.len = len;
	pMsg->udata.frame_data.type = type;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_RECVFRAME, (LPARAM)pMsg);
}

void CMFCDemoServerDlg::OnBnClickedButtonOpenImagePath()
{
	CString filepath;
	CFileDialog dlg(TRUE, NULL, _T(""), OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ͼƬ�ļ� All files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK) {
		filepath = dlg.GetPathName();

		m_imagePath = utils::ws2s(filepath.GetBuffer());

		SetDlgItemText(IDC_EDIT_IMAGE_PATH, filepath);
	}
}


void CMFCDemoServerDlg::OnBnClickedButtonOpenTmpls()
{
	m_tmplPaths.clear();

	CFileDialog dlg(TRUE, NULL, _T("*.tmp"), OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Template files (*.tmp)|*.tmp|All files (*.*)|*.*||"), NULL);
	dlg.m_ofn.nMaxFile = 500 * MAX_PATH;
	WCHAR* ch = new WCHAR[dlg.m_ofn.nMaxFile];
	dlg.m_ofn.lpstrFile = ch;
	ZeroMemory(dlg.m_ofn.lpstrFile, sizeof(WCHAR)* dlg.m_ofn.nMaxFile);

	std::string str;
	if (dlg.DoModal() == IDOK) {
		POSITION pos = dlg.GetStartPosition();

		while (pos) {
			CString filepath = dlg.GetNextPathName(pos);
			m_tmplPaths.push_back(utils::ws2s(filepath.GetBuffer()));
			str += utils::ws2s(filepath.GetBuffer());
			str += "\r\n";
		}

		SetDlgItemTextA(GetSafeHwnd(), IDC_EDIT_TMPLS, str.c_str());
	}
}

void CMFCDemoServerDlg::OnBnClickedButtonClearRecv()
{
	SetDlgItemText(IDC_EDIT_RECV, L"");
}


void CMFCDemoServerDlg::OnBnClickedButtonClearSend()
{
	SetDlgItemText(IDC_EDIT_SEND, L"");
}

void CMFCDemoServerDlg::OnBnClickedButtonSendCommand(){
	int cur_client = m_cmbClientList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("��ѡ��ͻ��ˣ�");
		return;
	}

	int cur_command = m_cmbCommand.GetCurSel();

	std::string json;
	switch (cur_command)
	{
	case 0: json = JsonTestDataParse::MakeGetDeviceInfo(); break;
	case 1: json = JsonTestDataParse::MakeDownloadPerson(m_imagePath, m_tmplPaths); break;
	default: assert(false);  break;
	}

	if (json.empty()){
		ShowMessage("����JSON��������Ϊ��(������Ա��Ҫѡ��ͼƬ��ģ��)��");
		return;
	}

	int ret = SendStringFrame(m_clients[cur_client].session, json);
	if (ret < 0){
		ShowMessage(utils::StrFormat("��������ʧ�ܣ� err = %s", SS_StrError(ret)));
		return;
	}

	appendSendMsg(utils::StrFormat("******************%s [string] [%d byte]*****************", 
		m_clients[cur_client].client_ip.c_str(), json.size()));
	appendSendMsg(json);
}
