#pragma once
#if _MSC_VER >= 1600          ///1600为vs2010编译器版本 
#pragma execution_character_set("utf-8")        ///c++11特性  设置执行编码 
#endif 

#include <windows.h>
#include <windef.h>
#include <qwindowdefs_win.h>
#include <string>
using namespace std;

#define MAX_MEMORY_TEXT_SIZE 10000
class YunLai
{
public:
	YunLai();
	~YunLai();
	struct handle_data
	{
		unsigned long process_id;
		HWND best_handle;
	};
	struct EnumChildWindowData
	{
		HWND hwnd;
		int status;
		int key;

	};
	//进程
	static void GetAllProcess(QMap<qint64,QString>& processInfo);
	static QString GetPathByProcessID(DWORD pid);
	//动画显示 及 隐藏窗口
	static void WindowTransparentShow(HWND dstHwnd,int maxShow=255,int speed=15);
	static void WindowTransparentFade(HWND dstHwnd, int minShow = 0, int speed = -15);
	//激活指定窗口
	static bool ActiveWnd(HWND hwnd);
	//设置焦点	
	static bool SetFocusToWnd(HWND hwnd);
	static bool IsVisibleWnd(HWND hwnd);
	//显示窗口
	static bool SetWindowShow(HWND hwnd);
	//隐藏窗口
	static bool SetWindowHide(HWND hwnd);


	//查找主窗口句柄
	static HWND FindMainWindow(unsigned long process_id);
	static BOOL IsMainWindow(HWND handle);
	static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

	//写文本到指定窗口 hwnd窗口句柄 起始坐标x y 写的内容szText 颜色color 字体大小 fontsize 字体名称 fontName 加粗bold 斜体 italic 下划线 underLIne
	static bool WriteTextToWnd(HWND hwnd, int x, int y, const char* szText, DWORD color,int fontSize,char* fontName,bool bold,bool italic,bool underLine);

	//向指定窗口句柄的窗口中发送按键消息(无返回值) nKey按键值 nStatus方式 1按键 2按下放开 3按下 4放开  bInherit是否传递给子窗口
	static void SendKeyToWnd(HWND hwnd, int nKey, int nStatus=0, bool bInherit = false);
	static BOOL CALLBACK EnumChildWindowsCallBack(HWND hwnd, LPARAM lParam);


	//进程相关
	static DWORD GetProcessIDFromWnd(HWND hwnd);   
	static char* ReadMemoryStrFromWnd(HWND hwnd,const char* szAddress,const char* offset1, const char* offset2, const char* offset3, const char* offset4);
	static char* ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress,int nLen);
	static char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen);
	static int   ReadMemoryIntFromWnd(HWND hwnd,const char* szAddress);
	static int   ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress);

	//键盘
	static char* GetKeyTextFromKey(int nKey);


	//鼠标
	static void MouseClickedEvent(int nType,int nCtrl);

	//时间
	static void SuperTimeDelay(int timeDelay,int timeType);
	static char* HexTextToDecimalText(const char* szHex);
	char* MyItoa(int num, char* str, int radix);
//	int HexToDecimalInt();
};

