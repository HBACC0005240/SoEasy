#pragma once
#if _MSC_VER >= 1600          ///1600Ϊvs2010�������汾 
#pragma execution_character_set("utf-8")        ///c++11����  ����ִ�б��� 
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
	//����
	static void GetAllProcess(QMap<qint64,QString>& processInfo);
	static QString GetPathByProcessID(DWORD pid);
	//������ʾ �� ���ش���
	static void WindowTransparentShow(HWND dstHwnd,int maxShow=255,int speed=15);
	static void WindowTransparentFade(HWND dstHwnd, int minShow = 0, int speed = -15);
	//����ָ������
	static bool ActiveWnd(HWND hwnd);
	//���ý���	
	static bool SetFocusToWnd(HWND hwnd);
	static bool IsVisibleWnd(HWND hwnd);
	//��ʾ����
	static bool SetWindowShow(HWND hwnd);
	//���ش���
	static bool SetWindowHide(HWND hwnd);


	//���������ھ��
	static HWND FindMainWindow(unsigned long process_id);
	static BOOL IsMainWindow(HWND handle);
	static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

	//д�ı���ָ������ hwnd���ھ�� ��ʼ����x y д������szText ��ɫcolor �����С fontsize �������� fontName �Ӵ�bold б�� italic �»��� underLIne
	static bool WriteTextToWnd(HWND hwnd, int x, int y, const char* szText, DWORD color,int fontSize,char* fontName,bool bold,bool italic,bool underLine);

	//��ָ�����ھ���Ĵ����з��Ͱ�����Ϣ(�޷���ֵ) nKey����ֵ nStatus��ʽ 1���� 2���·ſ� 3���� 4�ſ�  bInherit�Ƿ񴫵ݸ��Ӵ���
	static void SendKeyToWnd(HWND hwnd, int nKey, int nStatus=0, bool bInherit = false);
	static BOOL CALLBACK EnumChildWindowsCallBack(HWND hwnd, LPARAM lParam);


	//�������
	static DWORD GetProcessIDFromWnd(HWND hwnd);   
	static char* ReadMemoryStrFromWnd(HWND hwnd,const char* szAddress,const char* offset1, const char* offset2, const char* offset3, const char* offset4);
	static char* ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress,int nLen);
	static char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen);
	static int   ReadMemoryIntFromWnd(HWND hwnd,const char* szAddress);
	static int   ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress);

	//����
	static char* GetKeyTextFromKey(int nKey);


	//���
	static void MouseClickedEvent(int nType,int nCtrl);

	//ʱ��
	static void SuperTimeDelay(int timeDelay,int timeType);
	static char* HexTextToDecimalText(const char* szHex);
	char* MyItoa(int num, char* str, int radix);
//	int HexToDecimalInt();
};

