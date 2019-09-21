
// ArmMFCDemoClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ArmMFCDemoClient.h"
#include "ArmMFCDemoClientDlg.h"
#include "afxdialogex.h"

#include <common\utils.h>
#include <common\utf_gbk.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TMPL_SIZE 8192

std::string device_id;

INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	using namespace Gdiplus;
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);

	if (size == 0) {
		return -1;
	}

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

	if (pImageCodecInfo == NULL) {
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for (size_t i = 0; i < num; i++) {
		if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[i].Clsid;
			free(pImageCodecInfo);
			return i;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

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


// CArmMFCDemoClientDlg dialog


CArmMFCDemoClientDlg* CArmMFCDemoClientDlg::m_pThis = NULL;

CArmMFCDemoClientDlg::CArmMFCDemoClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CArmMFCDemoClientDlg::IDD, pParent)
{
	m_pThis = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CArmMFCDemoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CArmMFCDemoClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CArmMFCDemoClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CArmMFCDemoClientDlg::OnBnClickedButtonDisconnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_ACCESS_RECORD, &CArmMFCDemoClientDlg::OnBnClickedButtonUploadAccessRecord)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_DEVICE_STATUS, &CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceCondition)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_DEVICE_HEARTBEAT, &CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceHeartbeat)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_DEVICE_COMMAND, &CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceCommand)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_PERSON, &CArmMFCDemoClientDlg::OnBnClickedButtonUploadPerson)
END_MESSAGE_MAP()


// CArmMFCDemoClientDlg message handlers

BOOL CArmMFCDemoClientDlg::OnInitDialog()
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


	//show version
	std::string title = utils::StrFormat("ARM设备客户端示例-库版本: V%s", ARMNET_GetLibVersion());
	SetWindowTextA(GetSafeHwnd(), title.c_str());

	// TODO: Add extra initialization here
	SetDlgText( IDC_EDIT_IP, "127.0.0.1");
	SetDlgText(IDC_EDIT_PORT, "49003");

	SetDlgText(IDC_EDIT_PERSON_ID, "562454145");
	SetDlgText(IDC_EDIT_TIME, "20170325125263Z");
	SetDlgText(IDC_EDIT_ID_NUMBER, "426584595262450445");
	SetDlgText(IDC_EDIT_CARD_NUMBER, "123856");
	SetDlgInt(IDC_EDIT_ROLE, 1);
	SetDlgInt(IDC_EDIT_OPEN_DOOR_TYPE, 0);
	SetDlgInt(IDC_EDIT_INOUT_TYPE, 1);
	SetDlgText(IDC_EDIT_COMMUNITY_CODE, "123asdfa");

	SetDlgText(IDC_EDIT_COMMUNITY_CODE2, "123asdfa");
	SetDlgInt(IDC_EDIT_DOOR, 1);

	SetDlgText(IDC_EDIT_COMMUNITY_CODE3, "123asdfa");
	SetDlgInt(IDC_EDIT_TIME2, 1);
	SetDlgInt(IDC_EDIT_STATUS, 1);

	SetDlgInt(IDC_EDIT_COMMAND_TYPE, 5);
	SetDlgText(IDC_EDIT_COMMAND_VALUE, "Device Define value!!!");
	
	device_id = utils::StrFormat("id%d", GetTickCount());

	int ret = ARMNET_Initialize();
	if (ret < 0){
		ShowMessage(utils::StrFormat("Fails to initialize api, error(%s)", ARMNET_StrError(ret)));
		return TRUE;
	}

	ARMNET_SetCallback(ErrorCallback, CommandCallback);

	OnBnClickedButtonConnect();


	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CArmMFCDemoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	ARMNET_Finalize();

}

void CArmMFCDemoClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CArmMFCDemoClientDlg::OnPaint()
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
HCURSOR CArmMFCDemoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::string CArmMFCDemoClientDlg::GetDlgText(UINT id){
	char buff[1024];
	GetDlgItemTextA(GetSafeHwnd(), id, buff, 1024);
	return std::string(buff);
}

void CArmMFCDemoClientDlg::SetDlgText(UINT id, const std::string str){
	SetDlgItemTextA(GetSafeHwnd(), id, str.c_str());
}

int CArmMFCDemoClientDlg::GetDlgInt(UINT id){
	char buff[1024];
	GetDlgItemTextA(GetSafeHwnd(), id, buff, 1024);
	return atoi(buff);
}

void CArmMFCDemoClientDlg::SetDlgInt(UINT id, int i){
	SetDlgItemTextA(GetSafeHwnd(), id, utils::StrFormat("%d", i).c_str());
}

void CArmMFCDemoClientDlg::ShowMessage(const std::string& msg, int ret) const {
	if (ret == 0)
		MessageBoxA(m_pThis->GetSafeHwnd(), msg.c_str(), "ClientMessage", MB_OK);
	else
		MessageBoxA(m_pThis->GetSafeHwnd(), utils::StrFormat("%s\r\nError: %s(%d)",
		msg.c_str(), ARMNET_StrError(ret), ret).c_str(), "ClientMessage", MB_OK);
}
void CArmMFCDemoClientDlg::ErrorCallback(enum armnet_error_code error_code){
	m_pThis->ShowMessage(utils::StrFormat("Error: %s", ARMNET_StrError(error_code)));
}

void CArmMFCDemoClientDlg::appendMsg(const char* msg){
	std::string allMsg;

	static char buff[10240];
	GetDlgItemTextA(m_pThis->m_hWnd, IDC_EDIT_MESSAGE, buff, 10240);

	if (strlen(buff) <= 10140){
		allMsg = buff;
	}

	allMsg += msg;
	allMsg += "\r\n";

	SetDlgItemTextA(m_pThis->m_hWnd, IDC_EDIT_MESSAGE, allMsg.c_str());
}

void CArmMFCDemoClientDlg::CommandCallback(ARMNET_COMMAND_DATA command){
	std::string strcmd;

	armnet_command_type command_type = (armnet_command_type)ARMNET_GetCommandType(command);
	switch (command_type)
	{
	case ARMNET_CMD_SET_DEVICE_INFO:
		strcmd = "设置设备信息";
		break;
	case ARMNET_CMD_GET_DEVICE_INFO:
		strcmd = "获取设备信息";
		break;
	case ARMNET_CMD_SET_DEVICE_TIME:
		strcmd = "设置设备时间";
		break;
	case ARMNET_CMD_SET_DEVICE_CONFIG:
		strcmd = "配置设备";
		break;
	case ARMNET_CMD_GET_DEVICE_CONFIG:
		strcmd = "获取设备配置";
		break;
	case ARMNET_CMD_SEND_USER_COMMAND:
		strcmd = "发送自定义命令";
		break;
	case ARMNET_CMD_GET_DEVICE_STATUS:
		strcmd = "获取设备状态";
		break;
	case ARMNET_CMD_DOWNLOAD_PERSON:
		strcmd = "下载人员";
		break;
	case ARMNET_CMD_UPDATE_PERSON:
		strcmd = "更新人员";
		break;
	case ARMNET_CMD_REMOVE_PERSON:
		strcmd = "删除人员";
		break;
	case ARMNET_CMD_SET_PERSON_ROLE:
		strcmd = "设置黑白名单";
		break;
	case ARMNET_CMD_SET_PERSON_TMP_PWD:
		strcmd = "设置设备临时密码";
		break;
	default:
		ASSERT(false);
		break;
	}

	appendMsg(utils::StrFormat("接受到命令 : %s", strcmd.c_str()).c_str());

	int ret = 0;
	switch (command_type)
	{
	case ARMNET_CMD_SET_DEVICE_INFO:{
										const armnet_device_info* info = ARMNET_GetDeviceInfo(command);
										assert(info);
										std::string str_info = utils::StrFormat("设备ID：%s\r\n 设备IP：%s\r\n 设备端口：%d\r\n 服务器IP：%s\r\n 服务器端口：%d\r\n 安装位置：%s\r\n 小区编号：%s",
											info->uuid, info->device_ip, info->device_port, info->server_ip, info->server_port,
											info->address, info->community_code);
										
										appendMsg(str_info.c_str());
										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_GET_DEVICE_INFO:{
										armnet_device_info info;
										strcpy(info.uuid, device_id.c_str());
										strcpy(info.device_ip, "192.168.100.100");
										info.device_port = 49003;
										strcpy(info.server_ip, "192.168.100.59");
										info.server_port = 49000;
										strcpy(info.address, utf_gbk::toUtf8("小区答红识技术的故事").c_str());
										strcpy(info.community_code, "123654789");
										strcpy(info.software_version, "V3.2.1");
										strcpy(info.hardware_version, "V5.8.9");
										ret = ARMNET_SetDeviceInfo(command, &info);
										assert(ret == 0);
										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_SET_DEVICE_TIME:{
										const char* time = ARMNET_GetDeviceTime(command);
										assert(time);
										std::string str_info = utils::StrFormat("设备时间：%s\r\n", time);

										appendMsg(str_info.c_str());
										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_SET_DEVICE_CONFIG:{
										const armnet_device_config* config = ARMNET_GetDeviceConfig(command);
										assert(config);
										std::string str_info = utils::StrFormat("心跳时间间隔：%s\r\n 常开时间报警：%s\r\n 常开时间间隔：%s\r\n 临时密码超时：%s\r\n ",
											config->heart_jump_period, config->keep_open_timeout, config->keep_open_range, config->tmp_pwd_timeout);

										appendMsg(str_info.c_str());
										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_GET_DEVICE_CONFIG:{
										armnet_device_config config;
										strcpy(config.heart_jump_period, "20");
										strcpy(config.keep_open_timeout, "30");
										strcpy(config.keep_open_range, "02:00-12:00");
										strcpy(config.tmp_pwd_timeout, "56");
										ret = ARMNET_SetDeviceConfig(command, &config);
										assert(ret == 0);
										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_SEND_USER_COMMAND:{
										  const armnet_user_command* user_command = ARMNET_GetUserCommand(command);
										  assert(user_command);
										  std::string str_info = utils::StrFormat("命令类型：%d\r\n 命令值：%s\r\n",
											  user_command->type, user_command->value);
										  appendMsg(str_info.c_str());
										  ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										  assert(ret == 0);
	}
		break;
	case ARMNET_CMD_GET_DEVICE_STATUS:{
										  ret = ARMNET_SetDeviceStatus(command, 3);
										  assert(ret == 0);
										  ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										  assert(ret == 0);
	}
		break;
	case ARMNET_CMD_DOWNLOAD_PERSON:{
										const armnet_person_info* info = ARMNET_GetDownloadPersonInfo(command);
										assert(info);
										ret = utils::WriteFile("d:\\face.png", info->face.data.get_data(), info->face.data.length);

										ret = utils::WriteFile("d:\\tmp1.tmp", info->tmpl1.code.get_data(), info->tmpl1.code.length);
										assert(ret == TMPL_SIZE);
										ret = utils::WriteFile("d:\\tmp2.tmp", info->tmpl2.code.get_data(), info->tmpl2.code.length);
										assert(ret == TMPL_SIZE);
										std::string str_info = utils::StrFormat("人员ID：%s\r\n 人员姓名：%s\r\n 身份证号码：%s\r\n 门禁卡号码：%s\r\n 密钥信息：%s\r\n 部门编号：%s\r\n 黑白名单：%d\r\n 图片保存路径（d:\\face.png）模板保存路径（d:\\tmp1.tmp, d:\\tmp2.tmp）",
											info->person_id, utf_gbk::toGbk(info->person_name).c_str(), info->id_number, info->card_number,
											info->key_number, info->department_number, info->role);

										appendMsg(str_info.c_str());

										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_UPDATE_PERSON:{
									  const armnet_person_info* info = ARMNET_GetUpdatePersonInfo(command);
									  assert(info);
									  ret = utils::WriteFile("d:\\face.png", info->face.data.get_data(), info->face.data.length);

									  ret = utils::WriteFile("d:\\tmp1.tmp", info->tmpl1.code.get_data(), info->tmpl1.code.length);
									  assert(ret == TMPL_SIZE);
									  ret = utils::WriteFile("d:\\tmp2.tmp", info->tmpl2.code.get_data(), info->tmpl2.code.length);
									  assert(ret == TMPL_SIZE);
									  std::string str_info = utils::StrFormat("人员ID：%s\r\n 人员姓名：%s\r\n 身份证号码：%s\r\n 门禁卡号码：%s\r\n 密钥信息：%s\r\n 部门编号：%s\r\n 黑白名单：%d\r\n 图片保存路径（d:\\face.png）模板保存路径（d:\\tmp1.tmp, d:\\tmp2.tmp）",
										  info->person_id, info->person_name, info->id_number, info->card_number,
										  info->key_number, info->department_number, info->role);

									  appendMsg(str_info.c_str());

									  ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_DEVICE_ERROR);
									  assert(ret == 0);
	}
		break;
	case ARMNET_CMD_REMOVE_PERSON:{
									  const char* person_id = ARMNET_GetRemovePersonInfo(command);
									  assert(person_id);
									  std::string str_info = utils::StrFormat("人员ID：%s",person_id);

									  appendMsg(str_info.c_str());

									  ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
									  assert(ret == 0);
	}
		break;
	case ARMNET_CMD_SET_PERSON_ROLE:{
										const armnet_person_role* info = ARMNET_GetPersonRole(command);
										assert(info);
										std::string str_info = utils::StrFormat("人员ID：%s\r\n role = %d", info->person_id, info->role);

										appendMsg(str_info.c_str());

										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	case ARMNET_CMD_SET_PERSON_TMP_PWD:{
										const armnet_person_tmp_pwd* pwd = ARMNET_GetPersonTmpPwd(command);
										assert(pwd);
										std::string str_info = utils::StrFormat("人员ID：%s\r\n pwd = %s", pwd->person_id, pwd->pwd);

										appendMsg(str_info.c_str());

										ret = ARMNET_NotifyCommandResult(command, ARMNET_RESULT_OK);
										assert(ret == 0);
	}
		break;
	default:
		ASSERT(false);
		break;
	}

	appendMsg("*******************************************");
}

void CArmMFCDemoClientDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	char ipBuff[1024] = { 0 };
	GetDlgItemTextA(m_hWnd, IDC_EDIT_IP, ipBuff, 1024);
	char portBuff[1024] = { 0 };
	GetDlgItemTextA(m_hWnd, IDC_EDIT_PORT, portBuff, 1024);
	
	std::string ip(ipBuff);
	int port = atoi(portBuff);


	int ret = ARMNET_ConnectToHost(ip.c_str(), port);
	if (ret < 0){
		ShowMessage(utils::StrFormat("Connect to host failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}
}


void CArmMFCDemoClientDlg::OnBnClickedButtonDisconnect()
{
	// TODO: Add your control notification handler code here
	ARMNET_DisconnectFromHost();
}

void CArmMFCDemoClientDlg::OnBnClickedButtonUploadAccessRecord()
{
	// TODO: Add your control notification handler code here
	armnet_access_info info;
	strcpy(info.person_id, GetDlgText(IDC_EDIT_PERSON_ID).c_str());
	strcpy(info.time, GetDlgText(IDC_EDIT_TIME).c_str());
	strcpy(info.id_number, GetDlgText(IDC_EDIT_ID_NUMBER).c_str());
	strcpy(info.card_number, GetDlgText(IDC_EDIT_CARD_NUMBER).c_str());
	info.role = GetDlgInt(IDC_EDIT_ROLE);
	info.open_door_type = GetDlgInt(IDC_EDIT_OPEN_DOOR_TYPE);
	info.in_out_type = GetDlgInt(IDC_EDIT_INOUT_TYPE);
	strcpy(info.community_code, GetDlgText(IDC_EDIT_COMMUNITY_CODE).c_str());

	if (std::string(info.person_id).empty() ||
		std::string(info.time).empty() ||
		std::string(info.card_number).empty()){
		ShowMessage("信息不能为空");
		return;
	}

	int ret = ARMNET_UploadAccessRecord(&info);
	if (ret < 0){
		ShowMessage(utils::StrFormat("Upload failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}
}


void CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceCondition()
{
	// TODO: Add your control notification handler code here
	armnet_device_condition condition;
	strcpy(condition.community_code, GetDlgText(IDC_EDIT_COMMUNITY_CODE2).c_str());
	condition.door_status = GetDlgInt(IDC_EDIT_DOOR);

	int ret = ARMNET_UploadDeviceCondition(&condition);
	if (ret < 0){
		ShowMessage(utils::StrFormat("Upload failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}
}


void CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceHeartbeat()
{
	// TODO: Add your control notification handler code here
	armnet_device_heartbeat heartbeat;
	strcpy(heartbeat.community_code, GetDlgText(IDC_EDIT_COMMUNITY_CODE3).c_str());
	strcpy(heartbeat.time, GetDlgText(IDC_EDIT_TIME2).c_str());
	heartbeat.status = GetDlgInt(IDC_EDIT_STATUS);

	int ret = ARMNET_UploadDeviceHeartbeat(&heartbeat);
	if (ret < 0){
		ShowMessage(utils::StrFormat("Upload failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}
}


void CArmMFCDemoClientDlg::OnBnClickedButtonUploadDeviceCommand()
{
	// TODO: Add your control notification handler code here
	armnet_device_command command;
	command.type = GetDlgInt(IDC_EDIT_COMMAND_TYPE);
	strcpy(command.value, GetDlgText(IDC_EDIT_COMMAND_VALUE).c_str());

	int ret = ARMNET_UploadDeviceCommand(&command);
	if (ret < 0){
		ShowMessage(utils::StrFormat("Upload failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}
}


void CArmMFCDemoClientDlg::OnBnClickedButtonUploadPerson()
{
	// TODO: Add your control notification handler code here
	armnet_person_info info;
	memset(&info, 0, sizeof(armnet_person_info));

	strcpy(info.person_id, "ir23123");
	strcpy(info.person_name, "的拉菲");
	strcpy(info.id_number, "67643");
	strcpy(info.card_number, "TY342342");
	strcpy(info.key_number, "123456789");
	strcpy(info.department_number, "未来科技城123");
	info.role = 2;

	info.tmpl1.code.set_data(new byte[TMPL_SIZE]);
	info.tmpl1.code.length = TMPL_SIZE;
	int ret = utils::ReadFile("d:\\tmp1.tmp", info.tmpl1.code.get_data(), TMPL_SIZE);
	if (ret < 0){
		ShowMessage("读模板失败");
		return;
	}

	byte* image1(nullptr);
	ret = utils::ReadFile("d:\\code1.png", &image1);
	if (ret < 0){
		ShowMessage("读图片失败");
		return;
	}
	info.tmpl1.image.data.length = ret;
	info.tmpl1.image.data.set_data(image1);

	info.tmpl2.code.set_data(new byte[TMPL_SIZE]);
	info.tmpl2.code.length = TMPL_SIZE;
	ret = utils::ReadFile("d:\\tmp2.tmp", info.tmpl2.code.get_data(), TMPL_SIZE);
	if (ret < 0){
		ShowMessage("读模板失败");
		return;
	}

	byte* image2(nullptr);
	ret = utils::ReadFile("d:\\code2.png", &image2);
	if (ret < 0){
		ShowMessage("读图片失败");
		return;
	}
	info.tmpl2.image.data.length = ret;
	info.tmpl2.image.data.set_data(image2);

	byte* face(nullptr);
	ret = utils::ReadFile("d:\\face.png", &face);
	if (ret < 0){
		ShowMessage("读图片失败");
		return;
	}
	info.face.data.length = ret;
	info.face.data.set_data(face);

	ret = ARMNET_UploadPersonInfo(&info);
	delete[] info.tmpl1.code.get_data();
	delete[] info.tmpl2.code.get_data();
	delete[] info.tmpl1.image.data.get_data();
	delete[] info.tmpl2.image.data.get_data();
	delete[] info.face.data.get_data();

	if (ret < 0){
		ShowMessage(utils::StrFormat("Upload failed, error(%s)", ARMNET_StrError(ret)));
		return;
	}

}
