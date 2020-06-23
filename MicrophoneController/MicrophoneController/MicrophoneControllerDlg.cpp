
// MFCApplication1Dlg.cpp: 实现文件//


#include "pch.h"
#include "framework.h"
#include "MicrophoneController.h"
#include "MicrophoneControllerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SHOWTASK (WM_USER +1)

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CMicrophoneControllerDlg 对话框


CMicrophoneControllerDlg::CMicrophoneControllerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MICROPHONECONTROLLER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMicrophoneControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TO_TRAY, 最小化);
}

BEGIN_MESSAGE_MAP(CMicrophoneControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HOTKEY()
	ON_MESSAGE(WM_SHOWTASK, &CMicrophoneControllerDlg::onShowTask) // 显示程序的消息映射
	ON_BN_CLICKED(IDC_TO_TRAY, &CMicrophoneControllerDlg::OnBnClickedToTray)
	ON_BN_CLICKED(IDC_BUTTON1, &CMicrophoneControllerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMicrophoneControllerDlg 消息处理程序

BOOL CMicrophoneControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 初始化页面
	toTray(); // 最小化到托盘
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW); // 隐藏任务栏图标
	ShowWindow(SW_MINIMIZE); // 启动即隐藏页面

	// 初始化设备
	CMicrophoneControllerDlg::initDevice();

	// 初始化热键程序
	GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风未静音"));
	press = false;
	RegisterHotKey(this->GetSafeHwnd(), 1001, MOD_NOREPEAT, VK_F9); 	// 注册热键

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 热键消息处理
void CMicrophoneControllerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nHotKeyId) {
	case 1001:
	{
		if (!press) {
			// 未按下
			mute(TRUE);
			GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风已静音")); // 文本
			GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("解除静音")); // 按钮
			ShowToolTip(_T("提示"), _T("已静音"), NIIF_NONE, 500);
			press = true;
		}
		else {
			mute(FALSE);
			GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风未静音")); // 文本
			GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("静音")); // 按钮
			ShowToolTip(_T("提示"), _T("已解除静音"), NIIF_NONE, 500);
			press = false;
		}
	}break;
	default:
		break;
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

// 初始化设备
void CMicrophoneControllerDlg::initDevice()
{
	//IMMDeviceEnumerator* m_pIMMEnumerator = NULL;	// 设备接口列表
	//IMMDevice *pIMMDeivce = NULL;
	//IAudioEndpointVolume *m_pRenderSimpleVol = NULL;	// 麦克风音量控制接口

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator), (void**)&m_pIMMEnumerator);
		if (SUCCEEDED(hr)) {
			//strEndPointID为string类型的设备ID
		//if (strEndPointID.empty()){
			hr = m_pIMMEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pIMMDeivce);
			//}
			//else{
			//	hr = m_pIMMEnumerator->GetDevice(strEndPointID.c_str(), &pIMMDeivce);
			//}
			if (SUCCEEDED(hr)) {
				hr = pIMMDeivce->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&m_pRenderSimpleVol);
			}
		}
	}
}

// 静音控制程序
void CMicrophoneControllerDlg::mute(BOOL flag)
{
	m_pRenderSimpleVol->SetMute(flag, NULL); // 麦克风静音 flag=TRUE 静音，flag=FALSE 不静音
}

// 最小化托盘
void CMicrophoneControllerDlg::toTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;  nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称  
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(nid.szTip, _T("麦克风控制程序")); // 信息提示条为“麦克风控制程序”  
	ShowWindow(SW_HIDE); // 隐藏主窗口  
	Shell_NotifyIcon(NIM_ADD, &nid); // 在托盘区添加图标  
}

// 显示程序页面
LRESULT CMicrophoneControllerDlg::onShowTask(WPARAM wParam, LPARAM lParam)
//wParam接收的是图标的ID，而lParam接收的是鼠标的行为  
{
	if (wParam != IDR_MAINFRAME)
		return 1;
	switch (lParam)
	{
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭”  
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置  
		CMenu menu;
		menu.CreatePopupMenu();//声明一个弹出式菜单  

	   //增加菜单项“关闭”，点击则发送消息WM_DESTROY给主窗口（已隐藏），将程序结束。  
		menu.AppendMenu(MF_STRING, WM_DESTROY, TEXT("关闭"));
		//确定弹出式菜单的位置  
		menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
		//资源回收  
		HMENU hmenu = menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
	} break;
	case WM_LBUTTONDBLCLK://双击左键的处理  
	{
		this->ShowWindow(SW_SHOW);//简单的显示主窗口
		this->ShowWindow(SW_RESTORE);
		this->CenterWindow();
		deleteTray();
	} break;
	}  return 0;
}

// 删除托盘中图标
void CMicrophoneControllerDlg::deleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	// nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
	// nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	// strcpy_s(nid.szTip, _T("麦克风控制程序"));//信息提示条为"麦克风控制程序”
	Shell_NotifyIcon(NIM_DELETE, &nid); // 在托盘中删除图标
}

// 析构函数
void CMicrophoneControllerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	// 删除托盘图标
	deleteTray();
	// 解除热键注册
	UnregisterHotKey(this->GetSafeHwnd(), 1001);
}

// 最小化按钮点击事件处理
void CMicrophoneControllerDlg::OnBnClickedToTray()
{
	// TODO: 在此添加控件通知处理程序代码
	toTray();
}

// 手动静音按钮监听事件
void CMicrophoneControllerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!press) {
		// 未按下
		mute(TRUE);
		GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风已静音")); // 文本
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("解除静音")); // 按钮
		press = true;
	}
	else {
		mute(FALSE);
		GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风未静音")); // 文本
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("静音")); // 按钮
		press = false;
	}
}

// 气泡提示
BOOL CMicrophoneControllerDlg::ShowToolTip(LPCTSTR szTitle, LPCTSTR szMsg, DWORD dwInfoFlags, UINT uTimeout)
{
	NOTIFYICONDATA m_notify;
	m_notify.cbSize = sizeof(m_notify);
	m_notify.hWnd = m_hWnd;
	m_notify.uID = IDR_MAINFRAME;

	m_notify.uFlags = NIF_INFO;
	m_notify.uTimeout = uTimeout;
	m_notify.dwInfoFlags = dwInfoFlags;

	strcpy(m_notify.szInfoTitle, szTitle);
	strcpy(m_notify.szInfo, szMsg);

	return Shell_NotifyIcon(NIM_MODIFY, &m_notify);
}
