
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


#define PORT_BEGIN  39870

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
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND_SYNC, &CMFCDemoServerDlg::OnBnClickedButtonSendCommandSync)
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

	std::string title = utils::StrFormat("服务端演示程序 -库版本: V%s", SS_GetLibVersion());
	SetWindowTextA(GetSafeHwnd(), title.c_str());

	m_cmbCommand.AddString(L"获取设备信息");
	m_cmbCommand.AddString(L"下载人员");
	m_cmbCommand.SetCurSel(0);

	int ret = SS_Initialize();
	if (ret < 0){
		ShowMessage(utils::StrFormat("初始化失败， err = %s", SS_StrError(ret)));
		return TRUE;
	}

	ret = SS_SetCallback(connected_callback, disconnected_callback, error_callback, recvframe_callback);
	if (ret < 0){
		ShowMessage(utils::StrFormat("设置回调失败， err = %s", SS_StrError(ret)));
		return TRUE;
	}

	for (int i = 0; i < 10; i++){
		int port = PORT_BEGIN + i;
		SS_SERVER server;
		ret = SS_StartServerBindAddr("", PORT_BEGIN + i, &server);
		if (ret < 0){
			ShowMessage(utils::StrFormat("启动服务(port = %d)失败， err = %s", port, SS_StrError(ret)));
			return TRUE;
		}

		ServerInfo info;
		info.server = server;
		info.server_ip = "0.0.0.0";
		info.server_port = port;
		m_servers.Add(info);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDemoServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

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

	stringvec listclient;

	m_clients.Lock();
	m_servers.Lock();

	ClientList::List& clients = m_clients.Data();
	for (ClientList::List::const_iterator it = clients.begin();
		it != clients.end();
		++it){
		std::string str;

		//get server info
		bool find = false;
		ServerList::List& servers = m_servers.Data();
		for (ServerList::List::const_iterator its = servers.begin();
			its != servers.end();
			++its){
			if (it->server == its->server){
				find = true;
				str = utils::StrFormat("S(%d)", its->server_port);
				break;
			}
		}

		assert(find);

		str += utils::StrFormat("_C(%s:%d)", it->client_ip.c_str(), it->client_port);

		listclient.push_back(str);
	}
	m_servers.Unlock();
	m_clients.Unlock();

	for (int i = 0; i < listclient.size(); i++){
		m_cmbClientList.AddString(utils::s2ws(utils::StrFormat("%d_%s", i, listclient[i].c_str())).c_str());
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


std::string CMFCDemoServerDlg::GetClientInfo(SS_SESSION session){
	m_clients.Lock();
	ClientInfo cinfo;
	bool find = false;
	ClientList::List& clients = m_clients.Data();
	for (ClientList::List::const_iterator it = clients.begin();
		it != clients.end();
		++it){
		if (it->session == session){
			cinfo = *it;
			find = true;
			break;
		}
	}
	assert(find);
	m_clients.Unlock();


	m_servers.Lock();

	ServerInfo sinfo;
	find = false;
	ServerList::List& servers = m_servers.Data();
	for (ServerList::List::const_iterator its = servers.begin();
		its != servers.end();
		++its){
		if (its->server == cinfo.server){
			sinfo = *its;
			find = true;
			break;
		}
	}
	assert(find);
	m_servers.Unlock();

	return utils::StrFormat("S(%d)_C(%s:%d)", sinfo.server_port, cinfo.client_ip.c_str(), cinfo.client_port);
}

afx_msg LRESULT CMFCDemoServerDlg::OnCallbackMsg(WPARAM wParam, LPARAM lParam){
	return 0;
}

void CALLBACK CMFCDemoServerDlg::connected_callback(SS_SESSION server, SS_SESSION session, 
	const char* client_ip, int port){

	ClientInfo info;
	info.server = server;
	info.session = session;
	info.client_ip = client_ip;
	info.client_port = port;
	m_pThis->m_clients.Add(info);
	
	m_pThis->InitClientList();

	m_pThis->ShowMessage(utils::StrFormat("设备连接"));
}

void CALLBACK CMFCDemoServerDlg::disconnected_callback(SS_SESSION session){

	ClientInfo info;
	info.session = session;

	m_pThis->m_clients.Remove(info);

	m_pThis->InitClientList();

	m_pThis->ShowMessage(utils::StrFormat("设备断开"));
}

void CALLBACK CMFCDemoServerDlg::error_callback(SS_SESSION session, int error_code){
	m_pThis->ShowMessage(utils::StrFormat("设备错误"));
}

void CALLBACK CMFCDemoServerDlg::recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type){
	std::string client_ip = m_pThis->GetClientInfo(session);
	switch (type){
	case SS_FRAME_STRING:{
							 std::string str((char*)data, len);
							 //处理请求
							 std::string mod;
							 std::string session_id;
							 int type;
							 JsonTestDataParse::Parse(str, mod, session_id, type);

							 if (type == JSON_REQ){
								 m_pThis->appendRecvMsg(utils::StrFormat("******************%s [string] [%d byte]*****************",
									 client_ip.c_str(), len));
								 m_pThis->appendRecvMsg(str);

								 std::string ackjson = JsonTestDataParse::MakeResult(mod, session_id);

								 int ret = m_pThis->SendStringFrame(session, ackjson);
								 if (ret < 0){
									 m_pThis->ShowMessage(utils::StrFormat("发送数据失败， err = %s", SS_StrError(ret)));
									 return;
								 }

								 m_pThis->appendSendMsg(utils::StrFormat("******************%s [string] [%d byte]*****************",
									 client_ip.c_str(), ackjson.size()));
								 m_pThis->appendSendMsg(ackjson);
							 }
							 else{
								 if (m_pThis->_sync_request){
									 int ret = m_pThis->_convertSync.SetEvent(session_id, str);
									 if (ret < 0){
										 m_pThis->ShowMessage("通知事件失败");
									 }
								 }
								 else{
									 m_pThis->appendRecvMsg(utils::StrFormat("******************%s [string] [%d byte]*****************",
										 client_ip.c_str(), len));
									 m_pThis->appendRecvMsg(str);
								 }
							 }
	}
		break;
	case SS_FRAME_BINARY:{
							 m_pThis->appendRecvMsg(utils::StrFormat("******************%s [binary] [%d byte]*****************",
								 client_ip.c_str(), len));
							 std::string str((char*)data, len);
							 m_pThis->appendRecvMsg(str);
	}
		break;
	default:assert(false); break;
	}
}

void CMFCDemoServerDlg::OnBnClickedButtonOpenImagePath()
{
	CString filepath;
	CFileDialog dlg(TRUE, NULL, _T(""), OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("图片文件 All files (*.*)|*.*||"), NULL);

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
	_sync_request = false;

	int cur_client = m_cmbClientList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端！");
		return;
	}

	int cur_command = m_cmbCommand.GetCurSel();

	std::string json;
	std::string session_id;
	switch (cur_command)
	{
	case 0: json = JsonTestDataParse::MakeGetDeviceInfo(session_id); break;
	case 1: json = JsonTestDataParse::MakeDownloadPerson(m_imagePath, m_tmplPaths, session_id); break;
	default: assert(false);  break;
	}

	if (json.empty()){
		ShowMessage("构造JSON请求数据为空(下载人员需要选择图片和模板)！");
		return;
	}

	int ret = SendStringFrame(m_clients[cur_client].session, json);
	if (ret < 0){
		ShowMessage(utils::StrFormat("发送数据失败， err = %s", SS_StrError(ret)));
		return;
	}

	appendSendMsg(utils::StrFormat("******************%s [string] [%d byte]*****************", 
		m_clients[cur_client].client_ip.c_str(), json.size()));
	appendSendMsg(json);
}


void CMFCDemoServerDlg::OnBnClickedButtonSendCommandSync()
{
	_sync_request = true;

	// TODO: Add your control notification handler code here
	int cur_client = m_cmbClientList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端！");
		return;
	}

	int cur_command = m_cmbCommand.GetCurSel();

	std::string json;
	std::string session_id;
	switch (cur_command)
	{
	case 0: json = JsonTestDataParse::MakeGetDeviceInfo(session_id); break;
	case 1: json = JsonTestDataParse::MakeDownloadPerson(m_imagePath, m_tmplPaths, session_id); break;
	default: assert(false);  break;
	}

	if (json.empty()){
		ShowMessage("构造JSON请求数据为空(下载人员需要选择图片和模板)！");
		return;
	}

	int ret = SendStringFrame(m_clients[cur_client].session, json);
	if (ret < 0){
		ShowMessage(utils::StrFormat("发送数据失败， err = %s", SS_StrError(ret)));
		return;
	}

	//等待返回
	std::string data;
	ret = _convertSync.WaitEvent(session_id, &data, 5000);
	if (ret < 0){
		ShowMessage(utils::StrFormat("等待返回错误, code = %d", ret));
	}
	else if (ret == 1){
		ShowMessage("等待返回超时!");
	}
	else{ //success
		ShowMessage(data);
	}
}
