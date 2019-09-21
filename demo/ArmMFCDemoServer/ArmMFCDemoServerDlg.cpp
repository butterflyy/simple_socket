
// ArmMFCDemoServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ArmMFCDemoServer.h"
#include "ArmMFCDemoServerDlg.h"
#include "afxdialogex.h"

#include <common\utils.h>
#include <common\utf_gbk.h>
#include <common\base64.h>
#include <common\SignalManager.h>
#include <common\message_.h>

#include <Poco\JSON\Object.h>
#include <Poco\Dynamic\Struct.h>
#include <Poco\JSON\Parser.h>
#include <Poco\UUIDGenerator.h>
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
*/

// CAboutDlg dialog used for App About

#define PORT  49003

#define CAPTURE_IMAGE_DIR "C:\\Capture"

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

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAboutApi();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {
}

void CAboutDlg::OnBnClickedButtonAboutApi()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT_API, &CAboutDlg::OnBnClickedButtonAboutApi)
END_MESSAGE_MAP()


// CArmMFCDemoServerDlg dialog


CArmMFCDemoServerDlg* CArmMFCDemoServerDlg::m_pThis = nullptr;

CArmMFCDemoServerDlg::CArmMFCDemoServerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CArmMFCDemoServerDlg::IDD, pParent),
	HelperDlg(this){
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pThis = this;
}

void CArmMFCDemoServerDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_cmbDeviceList);
}

BEGIN_MESSAGE_MAP(CArmMFCDemoServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
	ON_MESSAGE(WM_CALLBACK_MSG, OnCallbackMsg)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CArmMFCDemoServerDlg::OnCbnSelchangeComboDeviceList)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_CLIENT, &CArmMFCDemoServerDlg::OnBnClickedButtonDisconnectClient)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEVICE_INFO, &CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceInfo)
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND, &CArmMFCDemoServerDlg::OnBnClickedButtonSendCommand)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_PERSON, &CArmMFCDemoServerDlg::OnBnClickedButtonDownloadPerson)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_PERSON, &CArmMFCDemoServerDlg::OnBnClickedButtonUpdatePerson)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PERSON, &CArmMFCDemoServerDlg::OnBnClickedButtonRemovePerson)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_TMPLS, &CArmMFCDemoServerDlg::OnBnClickedButtonOpenTmpls)
	ON_BN_CLICKED(IDC_BUTTON_SET_PERSON_ROLE, &CArmMFCDemoServerDlg::OnBnClickedButtonSetPersonRole)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEVICE_TIME, &CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceTime)
	ON_BN_CLICKED(IDC_BUTTON_GET_DEVICE_STATUS, &CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceStatus)
	ON_BN_CLICKED(IDC_BUTTON_GET_DEVICE_CONFIG, &CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceConfig)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEVICE_CONFIG, &CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceConfig)
	ON_BN_CLICKED(IDC_BUTTON_SET_PERSON_TMP_PWD, &CArmMFCDemoServerDlg::OnBnClickedButtonSetPersonTmpPwd)
	ON_BN_CLICKED(IDC_BUTTON_GET_DEVICE_INFO, &CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceInfo)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_IMAGE_PATH, &CArmMFCDemoServerDlg::OnBnClickedButtonOpenImagePath)
END_MESSAGE_MAP()


// CArmMFCDemoServerDlg message handlers

BOOL CArmMFCDemoServerDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);

    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);

        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

	//show version
	std::string title = utils::StrFormat("ARM服务端演示程序 -库版本: V%s", SS_GetLibVersion());
	SetWindowTextA(GetSafeHwnd(), title.c_str());

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

	ret = SS_StartServer(PORT);
	if (ret < 0){
		ShowMessage(utils::StrFormat("启动服务失败， err = %s", SS_StrError(ret)));
		return TRUE;
	}

	SetDlgText(IDC_EDIT_HEART_JUMP_PERIOD, "56");
	SetDlgText(IDC_EDIT_KEEP_OPEN_TIMEOUT, "78");
	SetDlgText(IDC_EDIT_KEEP_OPEN_RANGE, "12:32-11:22");
	SetDlgText(IDC_EDIT_TMP_PWD_TIMEOUT, "60");


	SetDlgText(IDC_EDIT_PERSON_ID, "123");
	SetDlgText(IDC_EDIT_PERSON_NAME, "张三");
	SetDlgText(IDC_EDIT_ID_NUMBER, "4420987198402657007");
	SetDlgText(IDC_EDIT_CARD_NUMBER, "123456789");
	SetDlgText(IDC_EDIT_KEY_NUMBER, "FFFFFFAAAAA");
	SetDlgText(IDC_EDIT_DEPARTMENT, "hongshi11");
	SetDlgInt(IDC_EDIT_ROLE, 1);
	SetDlgText(IDC_EDIT_TMP_PWD, "8956452");

	m_imagePath = "d:\\test.png";
	SetDlgText(IDC_EDIT_IMAGE_PATH, "d:\\test.png");

	m_tmplPaths.push_back("d:\\test1.tmp");
	m_tmplPaths.push_back("d:\\test2.tmp");
	SetDlgText(IDC_EDIT_TMPLS, "d:\\test1.tmp\r\nd:\\test2.tmp");


	SetDlgInt(IDC_EDIT_COMMAND_TYPE, 15);
	SetDlgText(IDC_EDIT_COMMAND_VALUE, "测试命令内容");

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CArmMFCDemoServerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CArmMFCDemoServerDlg::OnPaint() {
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CArmMFCDemoServerDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}


void CArmMFCDemoServerDlg::OnDestroy() {
    CDialogEx::OnDestroy();

	SS_StopServer();

	SS_Finalize();
}

void CArmMFCDemoServerDlg::InitDeviceList(){
	m_cmbDeviceList.ResetContent();

	for (std::vector<ClientInfo>::const_iterator it = m_clients.begin();
		it != m_clients.end();
		++it){
		m_cmbDeviceList.AddString(utils::s2ws(it->client_ip).c_str());
	}

	m_cmbDeviceList.SetCurSel(0);
}

afx_msg LRESULT CArmMFCDemoServerDlg::OnCallbackMsg(WPARAM wParam, LPARAM lParam){
	CallbackMsg* pMsg = (CallbackMsg*)lParam;
	switch (wParam)
	{
	case COMMAND_CONNECTED:{
							   ClientInfo info;
							   info.session = pMsg->session;
							   info.client_ip = pMsg->udata.client_ip;
							   m_clients.push_back(info);
							   InitDeviceList();

							   ShowMessage(utils::StrFormat("设备连接"));
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
								  InitDeviceList();

								  ShowMessage(utils::StrFormat("设备断开"));
	}
		break;
	case COMMAND_ERROR:{
						   ShowMessage(utils::StrFormat("设备错误"));
	}
		break;
	case COMMAND_RECVFRAME:{
							   if (pMsg->udata.frame_data.type == SS_FRAME_BINARY){ //没有二进制类型数据
								   assert(false);
								   ShowMessage("数据类型错误");
								   return 0;
							   }

							   //解析数据
							   std::string json((char*)pMsg->udata.frame_data.data, pMsg->udata.frame_data.len);
							   ParseData(json);
	}
		break;
	default:
		assert(false);
		break;
	}

	delete pMsg;
	return 0;
}

void CArmMFCDemoServerDlg::ParseData(const std::string& json){
	try
	{
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result;
		result = parser.parse(json);
		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
		Poco::DynamicStruct ds = *object;

		std::string mod = ds["mod"].toString();
		std::string session_id = ds["session_id"].toString();
		int type = ds["type"];

		std::string ackjson;
		if (mod == "set_device_info"){
			ShowMessage("设置设备信息成功");
		}
		else if (mod == "get_device_info"){
			utils::OutputDebug(json.c_str());
			SetDlgText(IDC_EDIT_DEVICE_UUID, ds["device_info"]["uuid"].toString());
			SetDlgText(IDC_EDIT_DEVICE_IP, ds["device_info"]["device_ip"].toString());
			SetDlgInt(IDC_EDIT_DEVICE_PORT, ds["device_info"]["device_port"]);
			SetDlgText(IDC_EDIT_SERVER_IP, ds["device_info"]["server_ip"].toString());
			SetDlgInt(IDC_EDIT_SERVER_PORT, ds["device_info"]["server_port"]);
			SetDlgText(IDC_EDIT_ADDRESS, utf_gbk::toGbk(ds["device_info"]["address"].toString()));
			SetDlgText(IDC_EDIT_COMMUNITY_CODE, ds["device_info"]["community_code"].toString());
			SetDlgText(IDC_EDIT_SOFTWARE_VERSION, ds["device_info"]["software_version"].toString());
			SetDlgText(IDC_EDIT_HARDWARE_VERSION, ds["device_info"]["hardware_version"].toString());
		}
		else if (mod == "set_device_time"){
		}
		else if (mod == "get_device_config"){
		}
		else if (mod == "get_device_config"){
		}
		else if (mod == "send_user_command"){
		}
		else if (mod == "get_device_status"){
		}
		else if (mod == "download_person"){

		}
		else if (mod == "update_person"){
		}
		else if (mod == "remove_person"){

		}
		else if (mod == "set_person_role"){

		}
		else if (mod == "set_person_tmp_pwd"){

		}
		else if (mod == "upload_access_info"){
			std::string str_record = utils::StrFormat("收到考勤记录\r\n 人员ID: %s\r\n 时间: %s\r\n 身份证号: %s\r\n 门禁卡号: %s\r\n 黑白名单: %d\r\n 开门方式: %d\r\n 类型: %d\r\n 小区编号: %s\r\n ",
				ds["access_info"]["person_id"].toString().c_str(),
				ds["access_info"]["time"].toString().c_str(),
				ds["access_info"]["id_number"].toString().c_str(),
				ds["access_info"]["card_number"].toString().c_str(),
				int(ds["access_info"]["role"]),
				int(ds["access_info"]["open_door_type"]),
				int(ds["access_info"]["in_out_type"]),
				ds["access_info"]["community_code"].toString().c_str());
			ShowMessage(str_record);
		}
		else{
			utils::OutputDebug("unknow command!!");
		}
	}
	catch (Poco::Exception& e)
	{
		ShowMessage(e.displayText());
		return;
	}
}

void CALLBACK CArmMFCDemoServerDlg::connected_callback(SS_SESSION session, const char* client_ip){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	strcpy_s(pMsg->udata.client_ip, client_ip);

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_CONNECTED, (LPARAM)pMsg);
}

void CALLBACK CArmMFCDemoServerDlg::disconnected_callback(SS_SESSION session){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_DISCONNECTED, (LPARAM)pMsg);
}

void CALLBACK CArmMFCDemoServerDlg::error_callback(SS_SESSION session, int error_code){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	pMsg->udata.error_code = error_code;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_ERROR, (LPARAM)pMsg);
}

void CALLBACK CArmMFCDemoServerDlg::recvframe_callback(SS_SESSION session, const unsigned char* data, int len, int type){
	//同步回调过程，避免阻塞
	CallbackMsg* pMsg = new CallbackMsg();
	memset(pMsg, 0, sizeof(CallbackMsg));
	pMsg->session = session;
	pMsg->udata.frame_data.data = new byte[len];
	memcpy(pMsg->udata.frame_data.data, data, len);
	pMsg->udata.frame_data.len = len;
	pMsg->udata.frame_data.type = type;

	m_pThis->PostMessage(WM_CALLBACK_MSG, COMMAND_RECVFRAME, (LPARAM)pMsg);
}

//void CArmMFCDemoServerDlg::ArmnetConnectedCallback(const char* device_id, ARMNET_DEVICE_RECORD record){
//	ASSERT(device_id && record);
//
//	DeviceInfo info;
//	memset(&info, 0, sizeof(DeviceInfo));
//	strcpy(info.id, device_id);
//	strcpy(info.device_ip, ARMNET_GetDeviceIp(record));
//	info.device_port = ARMNET_GetDevicePort(record);
//	strcpy(info.server_ip, ARMNET_GetServerIp(record));
//	info.server_port = ARMNET_GetServerPort(record);
//	strcpy(info.address, utf_gbk::toGbk(ARMNET_GetDeviceAddress(record)).c_str());
//	strcpy(info.community_code, ARMNET_GetDeviceCommunityCode(record));
//	strcpy(info.software_version, ARMNET_GetDeviceSoftwareVersion(record));
//	strcpy(info.hardware_version, ARMNET_GetDeviceHardwareVersion(record));
//
//	m_pThis->m_deviceList.push_back(info);
//
//	m_pThis->InitDeviceList();
//
//	m_pThis->ShowMessage(utils::StrFormat("设备 ID: %s 连接.", device_id));
//}
//
//void CArmMFCDemoServerDlg::ArmnetDisconnectedCallback(const char* device_id){
//	ASSERT(device_id);
//
//	for (std::list<DeviceInfo>::const_iterator it = m_pThis->m_deviceList.begin();
//		it != m_pThis->m_deviceList.end();
//		++it){
//		if (it->id == std::string(device_id)){
//			m_pThis->m_deviceList.erase(it);
//			break;
//		}
//	}
//
//	m_pThis->InitDeviceList();
//
//	m_pThis->ShowMessage(utils::StrFormat("设备 ID: %s 断开连接.", device_id));
//}
//
//void CArmMFCDemoServerDlg::ArmnetErrorCallback(const char* device_id, int error_code){
//	ASSERT(device_id);
//	m_pThis->ShowMessage(utils::StrFormat("设备 ID: %s 错误.", device_id), error_code);
//}
//
//void CArmMFCDemoServerDlg::ArmnetAccessRecordCallback(const char* device_id, ARMNET_ACCESS_RECORD record){
//	std::string str_open_door_type;
//	switch (ARMNET_GetAccessOpenDoorType(record))
//	{
//	case 0:
//		str_open_door_type = "虹膜";
//		break;
//	case 1:
//		str_open_door_type = "刷卡";
//		break;
//	case 2:
//		str_open_door_type = "密码";
//		break;
//	default:
//		str_open_door_type = "其他";
//		break;
//	}
//
//	std::string str_record = utils::StrFormat("设备（%s）收到考勤记录\r\n 人员ID: %s\r\n 时间: %s\r\n 身份证号: %s\r\n 门禁卡号: %s\r\n 黑白名单: %d\r\n 开门方式: %s\r\n 类型: %s\r\n 小区编号: %s\r\n ",
//		device_id,
//		ARMNET_GetAccessPersonID(record),
//		ARMNET_GetAccessTime(record),
//		ARMNET_GetAccessIdNumber(record),
//		ARMNET_GetAccessCardNumber(record),
//		ARMNET_GetAccessRole(record),
//		str_open_door_type.c_str(),
//		ARMNET_GetAccessInOutType(record) == 0 ? "进门" : "出门",
//		ARMNET_GetAccessCommunityCode(record));
//
//	m_pThis->ShowMessage(str_record);
//}
//
//void CArmMFCDemoServerDlg::ArmnetDeviceConditionCallback(const char* device_id,
//	const char* community_code, int door_status){
//	std::string str = utils::StrFormat("设备（%s）状态更新\r\n 小区编码: %s\r\n 门状态: %d\r\n",
//		device_id,
//		community_code, 
//		door_status);
//
//	m_pThis->ShowMessage(str);
//}
//
//void CArmMFCDemoServerDlg::ArmnetPersonInfoCallback(const char* device_id, ARMNET_PERSON_RECORD record){
//	int ret = utils::WriteFile("d:\\rface.png", ARMNET_GetFaceImage(record), ARMNET_GetPersonImageLength(record));
//	if (ret < 0){
//		m_pThis->ShowMessage("保存图片文件失败！");
//		return;
//	}
//	ret = utils::WriteFile("d:\\rtcode1.png", ARMNET_GetPersonTemplateImage(record, 0), ARMNET_GetPersonTemplateImageLength(record, 0));
//	if (ret < 0){
//		m_pThis->ShowMessage("保存模板图片文件失败！");
//		return;
//	}
//	ret = utils::WriteFile("d:\\rtmp1.tmp", ARMNET_GetPersonTemplateCode(record, 0), ARMNET_GetPersonTemplateCodeLength(record, 0));
//	if (ret < 0){
//		m_pThis->ShowMessage("保存模板文件失败！");
//		return;
//	}
//
//	ret = utils::WriteFile("d:\\rtcode2.png", ARMNET_GetPersonTemplateImage(record, 1), ARMNET_GetPersonTemplateImageLength(record, 1));
//	if (ret < 0){
//		m_pThis->ShowMessage("保存模板图片文件失败！");
//		return;
//	}
//	ret = utils::WriteFile("d:\\rtmp2.tmp", ARMNET_GetPersonTemplateCode(record, 1), ARMNET_GetPersonTemplateCodeLength(record, 1));
//	if (ret < 0){
//		m_pThis->ShowMessage("保存模板文件失败！");
//		return;
//	}
//
//	std::string str_info = utils::StrFormat(
//		"设备（%s）收到人员记录\r\n 人员ID: %s\r\n 姓名: %s\r\n 身份证号: %s\r\n 门禁卡号: %s\r\n 秘钥信息: %s\r\n 部门编号: %s\r\n \
//黑白名单: %d\r\n 图片大小: %d（%d * %d）\r\n 图片保存路径（d:\\rface.png）模板保存路径（d:\\rtcode1.png, d:\\rtmp1.tmp, d:\\rtcode2.png, d:\\rtmp2.tmp） \r\n ",
//		device_id,
//		ARMNET_GetPersonID(record),
//		ARMNET_GetPersonName(record),
//		ARMNET_GetPersonIdNumber(record),
//		ARMNET_GetPersonCardNumber(record),
//		ARMNET_GetPersonKeyNumber(record),
//		ARMNET_GetPersonDepartmentNumber(record),
//		ARMNET_GetPersonRole(record),
//		ARMNET_GetPersonImageLength(record),
//		ARMNET_GetPersonImageWidth(record),
//		ARMNET_GetPersonImageHeight(record));
//
//	m_pThis->ShowMessage(str_info);
//}
//
//void CArmMFCDemoServerDlg::ArmnetDeviceHeartbeatCallback(const char* device_id,
//	const char* community_code, const char* time, int status){
//	std::string str = utils::StrFormat("设备（%s）心跳包\r\n 小区编码: %s\r\n 时间: %s\r\n 状态: %d\r\n ",
//		device_id,
//		community_code,
//		time, 
//		status);
//
//	m_pThis->ShowMessage(str);
//}
//
//void CArmMFCDemoServerDlg::ArmnetDeviceCommandCallback(const char* device_id, int type, const char* value){
//	std::string str = utils::StrFormat("设备（%s）自定义命令\r\n 类型: %d\r\n 值: %s\r\n ",
//		device_id,
//		type,
//		value);
//
//	m_pThis->ShowMessage(str);
//}

void CArmMFCDemoServerDlg::OnCbnSelchangeComboDeviceList()
{
	// TODO: Add your control notification handler code here
	//int cur = m_cmbDeviceList.GetCurSel();
	//int index = 0;

	//DeviceInfo info;

	//for (std::list<DeviceInfo>::const_iterator it = m_deviceList.begin();
	//	it != m_deviceList.end();
	//	++it){
	//	if (cur == index){
	//		info = *it;
	//		break;
	//	}
	//	index++;
	//}

	//m_currentDeviceId = info.id;

	//SetDlgText(IDC_EDIT_DEVICE_IP, info.device_ip);
	//SetDlgInt(IDC_EDIT_DEVICE_PORT, info.device_port);
	//SetDlgText(IDC_EDIT_SERVER_IP, info.server_ip);
	//SetDlgInt(IDC_EDIT_SERVER_PORT, info.server_port);
	//SetDlgText(IDC_EDIT_ADDRESS, info.address);
	//SetDlgText(IDC_EDIT_COMMUNITY_CODE, info.community_code);
	//SetDlgText(IDC_EDIT_SOFTWARE_VERSION, info.software_version);
	//SetDlgText(IDC_EDIT_HARDWARE_VERSION, info.hardware_version);
}

void CArmMFCDemoServerDlg::OnBnClickedButtonDisconnectClient()
{
	// TODO: Add your control notification handler code here
	int cur_client = m_cmbDeviceList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端设备！");
		return;
	}

	int ret = SS_DisconnectClient(m_clients[cur_client].session);
	if (ret < 0) {
		ShowMessage(utils::StrFormat("失败， err = %s", SS_StrError(ret)));
		return;
	}

	std::vector<ClientInfo> clients;

	for (std::vector<ClientInfo>::const_iterator it = m_clients.begin();
		it != m_clients.end();
		++it){
		if (it->session != m_clients[cur_client].session){
			clients.push_back(*it);
		}
	}

	m_clients = clients;
	m_pThis->InitDeviceList();
}



void CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceInfo()
{
	int cur_client = m_cmbDeviceList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端设备！");
		return;
	}

	std::string device_ip = GetDlgText(IDC_EDIT_DEVICE_IP);
	int device_port = GetDlgInt(IDC_EDIT_DEVICE_PORT);
	std::string server_ip = GetDlgText(IDC_EDIT_SERVER_IP);
	int server_port = GetDlgInt(IDC_EDIT_SERVER_PORT);
	std::string address = GetDlgText(IDC_EDIT_ADDRESS);
	std::string community_code = GetDlgText(IDC_EDIT_COMMUNITY_CODE);

	if (std::string(device_ip).empty() ||
		std::string(server_ip).empty() ||
		std::string(address).empty() ||
		std::string(community_code).empty()){
		ShowMessage("信息不能为空");
		return;
	}

	try{
		Poco::JSON::Object root;

		root.set("mod", "set_device_info");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_REQ);

		Poco::JSON::Object info;
		info.set("device_ip", device_ip);
		info.set("device_port", device_port);
		info.set("server_ip", server_ip);
		info.set("server_port", server_port);
		info.set("address", "address");
		info.set("community_code", "community_code");

		root.set("device_info", info);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		int ret = SS_SendFrame(m_clients[cur_client].session, (byte*)str.c_str(), str.size(), SS_FRAME_STRING);
		if (ret < 0){
			ShowMessage(utils::StrFormat("请求失败， err = %s", SS_StrError(ret)));
			return;
		}
	}
	catch (Poco::Exception& e){
		ShowMessage(e.displayText());
	}
}

void CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceInfo()
{
	int cur_client = m_cmbDeviceList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端设备！");
		return;
	}

	try{
		Poco::JSON::Object root;

		root.set("mod", "get_device_info");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_REQ);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		int ret = SS_SendFrame(m_clients[cur_client].session, (byte*)str.c_str(), str.size(), SS_FRAME_STRING);
		if (ret < 0){
			ShowMessage(utils::StrFormat("请求失败， err = %s", SS_StrError(ret)));
			return;
		}
	}
	catch (Poco::Exception& e){
		ShowMessage(e.displayText());
	}

	//ARMNET_DEVICE_RECORD record = ARMNET_GetDeviceInfo(m_currentDeviceId.c_str());
	//if (!record){
	//	ShowMessage("获取设备信息失败");
	//	return;
	//}

	//DeviceInfo info;
	//memset(&info, 0, sizeof(DeviceInfo));
	//strcpy(info.id, m_currentDeviceId.c_str());
	//strcpy(info.device_ip, ARMNET_GetDeviceIp(record));
	//info.device_port = ARMNET_GetDevicePort(record);
	//strcpy(info.server_ip, ARMNET_GetServerIp(record));
	//info.server_port = ARMNET_GetServerPort(record);
	//strcpy(info.address, utf_gbk::toGbk(ARMNET_GetDeviceAddress(record)).c_str());
	//strcpy(info.community_code, ARMNET_GetDeviceCommunityCode(record));
	//strcpy(info.software_version, ARMNET_GetDeviceSoftwareVersion(record));
	//strcpy(info.hardware_version, ARMNET_GetDeviceHardwareVersion(record));

	//SetDlgText(IDC_EDIT_DEVICE_IP, info.device_ip);
	//SetDlgInt(IDC_EDIT_DEVICE_PORT, info.device_port);
	//SetDlgText(IDC_EDIT_SERVER_IP, info.server_ip);
	//SetDlgInt(IDC_EDIT_SERVER_PORT, info.server_port);
	//SetDlgText(IDC_EDIT_ADDRESS, info.address);
	//SetDlgText(IDC_EDIT_COMMUNITY_CODE, info.community_code);
	//SetDlgText(IDC_EDIT_SOFTWARE_VERSION, info.software_version);
	//SetDlgText(IDC_EDIT_HARDWARE_VERSION, info.hardware_version);
}


void CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceTime()
{
	// TODO: Add your control notification handler code here
	//time_t now;
	//time(&now);
	//tm* tnow;
	//tnow = localtime(&now);

	//std::string strtime = utils::StrFormat("%04d%02d%02d%02d%02d%02dZ",
	//	tnow->tm_year + 1900,
	//	tnow->tm_mon + 1,
	//	tnow->tm_mday,
	//	tnow->tm_hour,
	//	tnow->tm_min,
	//	tnow->tm_sec);

	//int ret = ARMNET_SetDeviceTime(m_currentDeviceId.c_str(), strtime.c_str());
	//if (ret < 0){
	//	ShowMessage("同步时间失败", ret);
	//}
}


void CArmMFCDemoServerDlg::OnBnClickedButtonSendCommand()
{
	//int type = GetDlgInt(IDC_EDIT_COMMAND_TYPE);
	//std::string value = GetDlgText(IDC_EDIT_COMMAND_VALUE);
	//assert(value.length() < 128);

	//int ret = ARMNET_SendUserCommand(m_currentDeviceId.c_str(), type, value.c_str());
	//if (ret < 0){
	//	ShowMessage("发送命令失败", ret);
	//}
}


void CArmMFCDemoServerDlg::OnBnClickedButtonDownloadPerson()
{
	int cur_client = m_cmbDeviceList.GetCurSel();
	if (cur_client == -1){
		ShowMessage("请选择客户端设备！");
		return;
	}

	std::string person_id = GetDlgText(IDC_EDIT_PERSON_ID);
	std::string person_name = GetDlgText(IDC_EDIT_PERSON_NAME);
	std::string id_number = GetDlgText(IDC_EDIT_ID_NUMBER);
	std::string card_number = GetDlgText(IDC_EDIT_CARD_NUMBER);
	std::string key_number = GetDlgText(IDC_EDIT_KEY_NUMBER);
	std::string department_number = GetDlgText(IDC_EDIT_DEPARTMENT);
	int role = GetDlgInt(IDC_EDIT_ROLE);

	if (std::string(person_id).empty() ||
		std::string(person_name).empty() ||
		std::string(card_number).empty() ||
		std::string(key_number).empty()){
		ShowMessage("信息不能为空!");
		return;
	}

	if (m_tmplPaths.size() != 2){
		ShowMessage("模板只能有两个!");
		return;
	}

	try{
		Poco::JSON::Object root;

		root.set("mod", "download_person");
		root.set("session_id", Poco::UUIDGenerator::defaultGenerator().createOne().toString());
		root.set("type", JSON_REQ);

		Poco::JSON::Object person_info;
		person_info.set("person_id", person_id);
		person_info.set("person_name", utf_gbk::toUtf8(person_name));
		person_info.set("id_number", "");
		person_info.set("card_number", card_number);
		person_info.set("key_number", key_number);
		person_info.set("department_number", department_number);
		person_info.set("role", role);

		Poco::JSON::Object face;
		byte *image = NULL;
		int size = utils::ReadFile(m_imagePath, &image);
		if (size == -1){
			ShowMessage("读取图片失败！");
			return;
		}
		face.set("width", 1231); //瞎填的
		face.set("height", 1231);
		face.set("format", 1);
		face.set("length", size);
		face.set("data", base64::encode(bytes((char*)image, size)));
		person_info.set("face", face);
		

		Poco::JSON::Object left_image;
		left_image.set("width", 0);
		left_image.set("height", 0);
		left_image.set("format", 0);
		left_image.set("length", 0);
		left_image.set("data", "");

		byte tmpl[TMPL_SIZE];
		size = utils::ReadFile(m_tmplPaths[0], tmpl, TMPL_SIZE);
		if (size != TMPL_SIZE){
			ShowMessage("读取模板失败！");
			return;
		}
		Poco::JSON::Object left_code;
		left_code.set("length", TMPL_SIZE);
		left_code.set("data", base64::encode(bytes((char*)tmpl, size)));

		person_info.set("left_image", left_image);
		person_info.set("left_code", left_code);


		Poco::JSON::Object right_image;
		right_image.set("width", 0);
		right_image.set("height", 0);
		right_image.set("format", 0);
		right_image.set("length", 0);
		right_image.set("data", "");

		size = utils::ReadFile(m_tmplPaths[1], tmpl, TMPL_SIZE);
		if (size != TMPL_SIZE){
			ShowMessage("读取模板失败！");
			return;
		}
		Poco::JSON::Object right_code;
		right_code.set("length", TMPL_SIZE);
		right_code.set("data", base64::encode(bytes((char*)tmpl, size)));

		person_info.set("right_image", right_image);
		person_info.set("right_code", right_code);

		root.set("person_info", person_info);

		std::stringstream ss;
		root.stringify(ss);

		std::string str = ss.str();

		int ret = SS_SendFrame(m_clients[cur_client].session, (byte*)str.c_str(), str.size(), SS_FRAME_STRING);
		if (ret < 0){
			ShowMessage(utils::StrFormat("请求失败， err = %s", SS_StrError(ret)));
			return;
		}
	}
	catch (Poco::Exception& e){
		ShowMessage(e.displayText());
	}
}


void CArmMFCDemoServerDlg::OnBnClickedButtonUpdatePerson()
{
	//PersonInfo info;
	//strcpy(info.person_id, GetDlgText(IDC_EDIT_PERSON_ID).c_str());
	//strcpy(info.person_name, GetDlgText(IDC_EDIT_PERSON_NAME).c_str());
	//strcpy(info.id_number, GetDlgText(IDC_EDIT_ID_NUMBER).c_str());
	//strcpy(info.card_number, GetDlgText(IDC_EDIT_CARD_NUMBER).c_str());
	//strcpy(info.key_number, GetDlgText(IDC_EDIT_KEY_NUMBER).c_str());
	//strcpy(info.department_number, GetDlgText(IDC_EDIT_DEPARTMENT).c_str());
	//info.role = GetDlgInt(IDC_EDIT_ROLE);

	//if (std::string(info.person_id).empty() ||
	//	std::string(info.person_name).empty() ||
	//	std::string(info.card_number).empty() ||
	//	std::string(info.key_number).empty()){
	//	ShowMessage("信息不能为空!");
	//	return;
	//}

	//if (m_tmplPaths.size() != 2){
	//	ShowMessage("模板只能有两个!");
	//	return;
	//}

	//ARMNET_PERSON_RECORD record = ARMNET_CreatePersonRecord(info.person_id, info.person_name, info.id_number,
	//	info.card_number, info.key_number, info.department_number, info.role);

	//byte *image = NULL;
	//int size = utils::ReadFile(m_imagePath, &image);
	//if (size == -1){
	//	ShowMessage("读取图片失败！");
	//	return;
	//}
	//ARMNET_SetFaceImage(record, image, size, 640, 483);//width, height没有用到，所以随便填的

	//byte tmpl[TMPL_SIZE];
	//for (int i = 0; i < 2; i++){
	//	int size = utils::ReadFile(m_tmplPaths[i], tmpl, TMPL_SIZE);
	//	if (size != TMPL_SIZE){
	//		ShowMessage("读取模板失败！");
	//		return;
	//	}

	//	ARMNET_InsertPersonTemplate(record, i, tmpl, TMPL_SIZE, 0, 0, 0, 0);
	//}

	//int ret = ARMNET_UpdatePerson(m_currentDeviceId.c_str(), record);
	//ARMNET_ReleasePersonRecord(record);
	//if (ret < 0){
	//	ShowMessage("更新人员失败", ret);
	//}
}


void CArmMFCDemoServerDlg::OnBnClickedButtonRemovePerson()
{
	//char person_id[32];
	//strcpy(person_id, GetDlgText(IDC_EDIT_PERSON_ID).c_str());
	//if (std::string(person_id).empty()){
	//	ShowMessage("信息不能为空!");
	//	return;
	//}

	//int ret = ARMNET_DeletePerson(m_currentDeviceId.c_str(), person_id);
	//if (ret < 0){
	//	ShowMessage("删除人员失败", ret);
	//}
}


void CArmMFCDemoServerDlg::OnBnClickedButtonOpenTmpls()
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

		SetDlgText(IDC_EDIT_TMPLS, str);
	}
}

void CArmMFCDemoServerDlg::OnBnClickedButtonOpenImagePath()
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

void CArmMFCDemoServerDlg::OnBnClickedButtonSetPersonRole()
{
	//char person_id[32];
	//byte role;

	//strcpy(person_id, GetDlgText(IDC_EDIT_PERSON_ID).c_str());
	//role = GetDlgInt(IDC_EDIT_ROLE);

	//if (std::string(person_id).empty()){
	//	ShowMessage("信息不能为空!");
	//	return;
	//}

	//int ret = ARMNET_SetPersonRole(m_currentDeviceId.c_str(), person_id, role);
	//if (ret < 0){
	//	ShowMessage("设置黑白名单失败", ret);
	//}
}

void CArmMFCDemoServerDlg::OnBnClickedButtonSetPersonTmpPwd()
{
	//char person_id[32];
	//char pwd[32];

	//strcpy(person_id, GetDlgText(IDC_EDIT_PERSON_ID).c_str());
	//strcpy(pwd, GetDlgText(IDC_EDIT_TMP_PWD).c_str());

	//if (std::string(person_id).empty()
	//	|| std::string(pwd).empty()){
	//	ShowMessage("信息不能为空!");
	//	return;
	//}

	//int ret = ARMNET_SetPersonTmpPwd(m_currentDeviceId.c_str(), person_id, pwd);
	//if (ret < 0){
	//	ShowMessage("设置临时密码失败", ret);
	//}
}



void CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceStatus()
{
	//int status = ARMNET_GetDeviceStatus(m_currentDeviceId.c_str());
	//if (status < 0){
	//	ShowMessage("获取设备状态失败", status);
	//	return;
	//}

	//ShowMessage(utils::StrFormat("设备状态值： %d", status));
}


void CArmMFCDemoServerDlg::OnBnClickedButtonGetDeviceConfig()
{
	//ARMNET_CONFIG_RECORD record = ARMNET_GetDeviceConfig(m_currentDeviceId.c_str());
	//if (!record){
	//	ShowMessage("获取设备配置失败");
	//	return;
	//}

	//SetDlgText(IDC_EDIT_HEART_JUMP_PERIOD, ARMNET_GetConfigHeartJumpPeriod(record));
	//SetDlgText(IDC_EDIT_KEEP_OPEN_TIMEOUT, ARMNET_GetConfigKeepOpenTimeout(record));
	//SetDlgText(IDC_EDIT_KEEP_OPEN_RANGE, ARMNET_GetConfigKeepOpenRange(record));
	//SetDlgText(IDC_EDIT_TMP_PWD_TIMEOUT, ARMNET_GetConfigTmpPwdTimeout(record));
}


void CArmMFCDemoServerDlg::OnBnClickedButtonSetDeviceConfig()
{
	//DeviceConfig config;
	//strcpy(config.heart_jump_period, GetDlgText(IDC_EDIT_HEART_JUMP_PERIOD).c_str());
	//strcpy(config.keep_open_timeout, GetDlgText(IDC_EDIT_KEEP_OPEN_TIMEOUT).c_str());
	//strcpy(config.keep_open_range, GetDlgText(IDC_EDIT_KEEP_OPEN_RANGE).c_str());
	//strcpy(config.tmp_pwd_timeout, GetDlgText(IDC_EDIT_TMP_PWD_TIMEOUT).c_str());

	//if (std::string(config.heart_jump_period).empty() ||
	//	std::string(config.keep_open_timeout).empty() ||
	//	std::string(config.keep_open_range).empty() ||
	//	std::string(config.tmp_pwd_timeout).empty()){
	//	ShowMessage("信息不能为空!");
	//	return;
	//}

	//ARMNET_CONFIG_RECORD record = ARMNET_CreateConfigRecord(config.heart_jump_period, config.keep_open_timeout,
	//	config.keep_open_range, config.tmp_pwd_timeout);

	//int ret = ARMNET_SetDeviceConfig(m_currentDeviceId.c_str(), record);
	//if (ret < 0){
	//	ShowMessage("配置设备失败", ret);
	//}
	//ARMNET_ReleaseConfigRecord(record);
}
