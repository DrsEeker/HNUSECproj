
// MicrophoneController.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号

#pragma comment( lib, "gdiplus.lib" )
#include "gdiplus.h"
using namespace Gdiplus;

// CMicrophoneControllerApp:
// 有关此类的实现，请参阅 MicrophoneController.cpp
//

class CMicrophoneControllerApp : public CWinApp
{
public:
	CMicrophoneControllerApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	HANDLE m_hMutex; // 单例互斥量
	ULONG_PTR m_gdiplusToken;

	CImage img;
	CRect rect;

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMicrophoneControllerApp theApp;
