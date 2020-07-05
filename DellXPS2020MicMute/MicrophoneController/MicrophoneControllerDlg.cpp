
// MFCApplication1Dlg.cpp: 实现文件//

#include "stdafx.h"
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
	DDX_Control(pDX, IDC_BUTTON2, m_btn);
}

BEGIN_MESSAGE_MAP(CMicrophoneControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HOTKEY()
	ON_MESSAGE(WM_SHOWTASK, &CMicrophoneControllerDlg::onShowTask) // 显示程序的消息映射
	ON_WM_TIMER()
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

	// 初始化设备
	initDevice();
	// 初始化页面
	initView();

	// 初始化热键程序
	// GetDlgItem(IDC_STATIC)->SetWindowText(_T("麦克风未静音"));
	RegisterHotKey(this->GetSafeHwnd(), 1001, MOD_NOREPEAT, VK_F9); 	// 注册热键

	// 气泡提示
	SetTimer(1, 1500, NULL);// 启动ID为1的定时器，定时时间为1.5秒
	ShowToolTip(_T("提示"), _T("启动程序"), NIIF_NONE, 100);
	// 设置图标
	muteStatus = getMuteStatus();
	if (muteStatus) {
		m_btn.SetImage(1);
	}
	else {
		m_btn.SetImage(0);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 初始化视图
void CMicrophoneControllerDlg::initView()
{
	// 固定窗体大小
	ModifyStyle(m_hWnd, WS_THICKFRAME, 0, 0);
	// 设置窗口位置
	int  cx = GetSystemMetrics(SM_CXSCREEN);
	int  cy = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&wndTopMost, cx / 2 - 160, cy / 2 + 80, 0, 0, SWP_NOSIZE /*| SWP_NOMOVE*/);

	// 透明窗体
	COLORREF maskColor = RGB(30, 30, 30);
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
	if (hInst)
	{
		typedef BOOL(WINAPI* MYFUNC)(HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun) {
			fun(this->GetSafeHwnd(), maskColor, 200, LWA_ALPHA | LWA_COLORKEY);
		}
		FreeLibrary(hInst);
	}

	toTray(); // 最小化到托盘
	// ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW); // 隐藏任务栏图标
	ModifyStyleEx(WS_CAPTION, WS_EX_TOOLWINDOW); // 隐藏任务栏图标
	
	// 按钮设置
	m_btn.LoadStdImage(IDB_MICROPHONE, _T("PNG"));
	m_btn.LoadAltImage(IDB_MICROPHONE_MUTE, _T("PNG"));
	m_btn.EnableToggle(TRUE);
}

void CMicrophoneControllerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
		////////在这里添加下面代码		
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(30, 30, 30));

		CDialog::OnPaint();
	}
}

// 热键消息处理
void CMicrophoneControllerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nHotKeyId) {
	case 1001:
		if (!muteStatus) {
			mute(TRUE);
			//ShowToolTip(_T("提示"), _T("已静音"), NIIF_NONE, 500);// 静音时长在win10下不能用
			m_btn.SetImage(1); // ALT_TYPE 静音
			modifyTray(IDR_MAINFRAME_MUTE);
			muteStatus = true;
		}
		else {
			mute(FALSE);
			//ShowToolTip(_T("提示"), _T("已解除静音"), NIIF_NONE, 500);
			m_btn.SetImage(0); // STD_TYPE 未静音
			modifyTray(IDR_MAINFRAME);
			muteStatus = false;
		}
		break;
	default:
		break;
	}

	SetTimer(1, 1500, NULL);// 启动ID为1的定时器，定时时间为1秒
	this->ShowWindow(SW_SHOW);
	this->ShowWindow(SW_RESTORE);

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
	if (FAILED(hr)) {
		AfxMessageBox(_T("初始化麦克风设备失败"));
		exit(0);
	}
}

// 静音控制程序
void CMicrophoneControllerDlg::mute(BOOL flag)
{
	m_pRenderSimpleVol->SetMute(flag, NULL); // 麦克风静音 flag=TRUE 静音，flag=FALSE 不静音
}

// 获取静音状态
BOOL CMicrophoneControllerDlg::getMuteStatus()
{
	BOOL bMute = 0;
	HRESULT hr =  m_pRenderSimpleVol->GetMute(&bMute);
	return bMute;
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
		menu.AppendMenu(MF_STRING, WM_DESTROY, TEXT("退出"));
		//确定弹出式菜单的位置  
		menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
		//资源回收  
		HMENU hmenu = menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
	} break;
	case WM_LBUTTONDBLCLK://双击左键的处理  
	{
		SetTimer(1, 1500, NULL);// 启动ID为1的定时器，定时时间为3秒  
		this->ShowWindow(SW_SHOW);//简单的显示主窗口
		this->ShowWindow(SW_RESTORE);
	} break;
	}  return 0;
}

// 气泡提示
BOOL CMicrophoneControllerDlg::ShowToolTip(LPCTSTR szTitle, LPCTSTR szMsg, DWORD dwInfoFlags, UINT uTimeout)
{
	NOTIFYICONDATA m_notify;
	m_notify.cbSize = sizeof(m_notify);
	m_notify.hWnd = m_hWnd;
	m_notify.uID = ID;

	m_notify.uFlags = NIF_INFO;
	m_notify.uTimeout = uTimeout; // Note that uTimeout is valid only in Windows 2000 and Windows XP.
	m_notify.dwInfoFlags = dwInfoFlags;

	strcpy(m_notify.szInfoTitle, szTitle);
	strcpy(m_notify.szInfo, szMsg);

	return Shell_NotifyIcon(NIM_MODIFY, &m_notify);
}


// 最小化托盘
void CMicrophoneControllerDlg::toTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;  
	nid.uID = ID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称  
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy_s(nid.szTip, _T("麦克风控制程序")); // 信息提示条为“麦克风控制程序”  
	ShowWindow(SW_HIDE); // 隐藏主窗口  
	Shell_NotifyIcon(NIM_ADD, &nid); // 在托盘区添加图标  
}

// 修改托盘图标
void CMicrophoneControllerDlg::modifyTray(int IDR)
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = ID;
	nid.uFlags = NIF_ICON;
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR));
	strcpy_s(nid.szTip, _T("麦克风控制程序")); // 信息提示条为“麦克风控制程序”  
	// ShowWindow(SW_HIDE); // 隐藏主窗口  
	Shell_NotifyIcon(NIM_MODIFY, &nid); // 修改图标  
}

// 删除托盘中图标
void CMicrophoneControllerDlg::deleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = ID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	// nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称
	// nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	// strcpy_s(nid.szTip, _T("麦克风控制程序"));//信息提示条为"麦克风控制程序”
	Shell_NotifyIcon(NIM_DELETE, &nid); // 在托盘中删除图标
}

// 定时器消息处理
void CMicrophoneControllerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		toTray();// 最小化
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
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
	// 删除定时器
	KillTimer(1);
}
