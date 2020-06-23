## 麦克风控制小程序

## 环境
Visual Studio 2019

## 语言
c++, MFC 

## 功能
启动程序后自动最小化到托盘
按下F9能够静音，再按下F9就能接触静音（或者手动也可以）

在MicrophoneControllerDlg.cpp的123行中可以修改热键
``` c++
RegisterHotKey(this->GetSafeHwnd(), 1001, MOD_NOREPEAT, VK_F9); 	// 注册热键
```
最后一个参数用来指定热键，参数选择在https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

或者使用组合键https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey

## 参考链接
Core Audio api
https://blog.csdn.net/zdy_ruoshui/article/details/17138665
