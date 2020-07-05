
// MFCApplication1Dlg.h: 头文件
//

#pragma once
#pragma warning(disable : 4996)

#include "GdipButton.h"
#include "Mmdeviceapi.h"
#include "Endpointvolume.h"
#include "Audioclient.h"
#include "string.h"

#define ID 7668
 
// CMicrophoneControllerDlg 对话框
class CMicrophoneControllerDlg : public CDialogEx
{
	// 构造
public:
	CMicrophoneControllerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MICROPHONECONTROLLER_DIALOG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2); // 消息处理程序

	void OnPaint();
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy(); // 析构函数

	BOOL muteStatus;  // 按下状态
	CGdipButton m_btn;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	IMMDeviceEnumerator* m_pIMMEnumerator = NULL;	// 设备接口列表
	IMMDevice* pIMMDeivce = NULL; // 设备
	IAudioEndpointVolume* m_pRenderSimpleVol = NULL;	// 麦克风音量控制接口

	void initView(); // 初始化view

	void initDevice(); // 初始化设备
	void mute(BOOL flag);// 静音函数
	BOOL getMuteStatus();// 获取静音状态

	void toTray(); // 最小化托盘
	void modifyTray(int IDR); // 修改托盘图标
	void deleteTray(); // 删除托盘中的图标

	LRESULT onShowTask(WPARAM wParam, LPARAM lParam); // 显示页面
	BOOL ShowToolTip(LPCTSTR szTitle, LPCTSTR szMsg, DWORD dwInfoFlags, UINT uTimeout); // 气泡提示

};
