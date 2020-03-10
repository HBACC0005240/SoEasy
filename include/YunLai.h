#pragma once
#if _MSC_VER >= 1600          ///1600Ϊvs2010�������汾 
#pragma execution_character_set("utf-8")        ///c++11����  ����ִ�б��� 
#endif 

#include <windows.h>
#include <windef.h>
#include <qwindowdefs_win.h>
#include <string>
#include <functional>
#include "MINT.h"
//#include <experimental/filesystem>
//#include <experimental/scope>
using namespace std;
//using namespace experimental;
#define MAX_MEMORY_TEXT_SIZE 10000


class virtual_buffer_t
{
public:
	virtual_buffer_t();
	virtual_buffer_t(size_t size);
	~virtual_buffer_t();
	void * GetSpace(size_t needSize);
	size_t GetLength() { return m_cbSize; }
	void * GetBuffer() { return m_pBuffer; }

	void * m_pBuffer;
	size_t m_cbSize;
};

class crt_buffer_t
{
public:
	crt_buffer_t();
	crt_buffer_t(size_t size);
	~crt_buffer_t();
	void * GetSpace(size_t needSize);
	size_t GetLength() { return m_cbSize; }
	void * GetBuffer() { return m_pBuffer; }

	void * m_pBuffer;
	size_t m_cbSize;
};
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

	//ȡɫ��
	static int  GetColorDepth();

	//ȡ��ɫ
	static DWORD GetScreenColor(int x, int y, HWND hwnd=nullptr);
	static DWORD GetScreenColorCapture(int x,int y,HWND hwnd=nullptr);
	static POINT FindBMPColor(uchar* pData, DWORD nColor,int nDVal,POINT nFindPoint);
	static POINT FindScreenColor(DWORD nColor,int nLeftX,int nLeftY, int nWidth,int nHeight,int nDVal,POINT nFindPoint,HWND hwnd=nullptr);
	
	//�������
	static DWORD GetProcessIDFromWnd(HWND hwnd);   
	static char* ReadMemoryStrFromWnd(HWND hwnd,const char* szAddress,const char* offset1, const char* offset2, const char* offset3, const char* offset4);
	static char* ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress,int nLen);
	static char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen);
	static int   ReadMemoryIntFromWnd(HWND hwnd,const char* szAddress);
	static int   ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress);
	static void  WriteMemoryIntToWnd(HWND hwnd, const char* szAddress,int nVal);
	static void  WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal);
	static long  ForceOpenProcess(DWORD dwDesiredAccess, bool bInhert, DWORD ProcessId);
	static bool  ForceCloseProcess(DWORD hProcessID, DWORD nExitStatus);

	//dllģ��
	static HMODULE GetLibraryHandle(const char* szLib);
	static FARPROC GetFunAddress(const char* szLib,const char* szFun);

	//����
	static char* GetKeyTextFromKey(int nKey);
	static void KeyClickedEvent();
	static int MakeKeyLParam(int VirtualKey, int flag);
	static void SimKeyClick(UINT vk_Code, BOOL bDown);
	//���
	static void MouseClickedEvent(int nType=0,int nCtrl=0);
	//������, �ɿ�, �ɿ�:Ϊ���  1 #���   2 #�Ҽ�   3 #�м�
	//.���� ����, ������, �ɿ�, �ɿ�:Ϊ����  1 #����   2 #˫��   3 #����  4 #�ſ�
	static void SimMouseClick(int nType = 0, int nCtrl = 0);
	//ʱ��
	static void SuperTimeDelay(int timeDelay,int timeType);
	static char* HexTextToDecimalText(const char* szHex);
	char* MyItoa(int num, char* str, int radix);

	//��ͼ
	using SnapshotCallback = std::function<void(void *, size_t, int, int, int)>;
	virtual_buffer_t m_snapshot_buffer;
	static uchar* ScreenCapture(HWND hwnd,int nLeft=0,int nTop=0,int nWidth=0,int nHeight=0,bool bClent=false);
	static HBITMAP CopyScreenToBitmap(HWND pHwnd);
	static bool SaveBitmapToFile(HBITMAP   hBitmap, string szfilename);
//	static bool DIBSnapshot(HWND hWnd, int scale, const SnapshotCallback &callback);
//	static bool DIBToCvMat(cv::Mat &mat, void *pBuffer, size_t cbBuffer, int width, int height, int bbp);
//	int HexToDecimalInt();
};

