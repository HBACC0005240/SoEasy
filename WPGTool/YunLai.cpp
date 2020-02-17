#include "YunLai.h"
#include <tlhelp32.h>
#include <psapi.h> 
#include <QString>
#include <QMap>
#include <QDebug>
#include "../include/ITPublic.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")

static HWND m_lastHwnd = nullptr;
#define FORMAT_PATH(path) path.replace('\\','/').toLower()

YunLai::YunLai()
{
}

YunLai::~YunLai()
{

}

void YunLai::GetAllProcess(QMap<qint64, QString>& processInfo)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//��ȡ�����б�ʧ��
		return;
	}
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		QString exeName = (QString::fromUtf16(reinterpret_cast<const unsigned short *>(pe32.szExeFile)));
		/*QString exePath = GetPathByProcessID(pe32.th32ProcessID);
		exePath = FORMAT_PATH(exePath);
		if (exePath.isEmpty())
		{

		}
		else
		{
			processInfo[pe32.th32ProcessID] = exePath;
		}*/
		processInfo[pe32.th32ProcessID] = exeName;
		bMore = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}
QString YunLai::GetPathByProcessID(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
	{

		return "";
	}
	WCHAR filePath[MAX_PATH];
	DWORD ret = GetModuleFileNameEx(hProcess, NULL, filePath, MAX_PATH);
	QString file = QString::fromStdWString(filePath);
	CloseHandle(hProcess);
	return ret == 0 ? "" : file;
}
void YunLai::WindowTransparentShow(HWND dstHwnd, int maxShow /*= 255*/, int speed/* = 15*/)
{
	long rtn = GetWindowLong(dstHwnd, GWL_EXSTYLE);	//    'ȡ�Ĵ���ԭ�ȵ���ʽ
	rtn = rtn | WS_EX_LAYERED;						//    'ʹ����������µ���ʽWS_EX_LAYERED
	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '���µ���ʽ��������
	for (int i=0;i< maxShow;i+=speed)
	{
		SetLayeredWindowAttributes(dstHwnd, 0, i, LWA_ALPHA); //'�Ѵ������óɰ�͸����ʽ,�ڶ���������ʾ͸���̶�	'ȡֵ��Χ0--255,Ϊ0ʱ����һ��ȫ͸���Ĵ�����		
	}
	SetLayeredWindowAttributes(dstHwnd, 0, 255, LWA_ALPHA);
//	'�Ѵ������óɰ�͸����ʽ,�ڶ���������ʾ͸���̶�
//	'ȡֵ��Χ0--255,Ϊ0ʱ����һ��ȫ͸���Ĵ�����
}

void YunLai::WindowTransparentFade(HWND dstHwnd, int minShow /*= 0*/, int speed /*= -15*/)
{
	long rtn = GetWindowLong(dstHwnd, GWL_EXSTYLE);	//    'ȡ�Ĵ���ԭ�ȵ���ʽ
	rtn = rtn | WS_EX_LAYERED;						//    'ʹ����������µ���ʽWS_EX_LAYERED
	SetWindowLong(dstHwnd, GWL_EXSTYLE, rtn);		//     '���µ���ʽ��������
	for (int i = 255; i > minShow; i += speed)
	{
		SetLayeredWindowAttributes(dstHwnd, 0, i, LWA_ALPHA); //'�Ѵ������óɰ�͸����ʽ,�ڶ���������ʾ͸���̶�	'ȡֵ��Χ0--255,Ϊ0ʱ����һ��ȫ͸���Ĵ�����		
	}
	SetLayeredWindowAttributes(dstHwnd, 0, 0, LWA_ALPHA);
}

HWND YunLai::FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

BOOL YunLai::IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL YunLai::EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}

bool YunLai::WriteTextToWnd(HWND hwnd, int x, int y,const char* szText, DWORD color, int fontSize, char* fontName, bool bold, bool italic, bool underLine)
{
	HDC pDC = GetDC(hwnd);
	char szFontName[32] = "";
	strcpy(szFontName, fontName);
	if (strlen(fontName) < 1)
		strcpy(szFontName,"����");
	LOGFONTA struFont;
	memset(&struFont, 0, sizeof(LOGFONTA));
	struFont.lfHeight = fontSize;
	if (bold)
		struFont.lfWeight = 900;
	else
		struFont.lfWeight = 0;
	if (italic)
		struFont.lfItalic = 1;
	else
		struFont.lfItalic = 0;
	if (underLine)
		struFont.lfUnderline = 1;
	else
		struFont.lfUnderline = 0;
	struFont.lfCharSet = 134;
	strcpy(struFont.lfFaceName, szFontName);
	HFONT hFont= CreateFontIndirectA(&struFont);
	HGDIOBJ hGDIObj= SelectObject(pDC, hFont);
	int backMode = SetBkMode(pDC, 1);
	COLORREF oldColor = SetTextColor(pDC, color);
	TextOutA(pDC, x, y, szText, strlen(szText));
	SetBkMode(pDC, backMode);			//��ԭ��ȥ
	SetTextColor(pDC, oldColor);
	SelectObject(pDC, hGDIObj);
	DeleteObject(hFont);
	ReleaseDC(hwnd, pDC);
	return true;
}

void YunLai::SendKeyToWnd(HWND hwnd, int nKey, int nStatus, bool bInherit /*= false*/)
{
	EnumChildWindowData data;
	data.hwnd = hwnd;
	data.status = nStatus;
	data.key = nKey;
	EnumChildWindowsCallBack(hwnd, (LPARAM)&data);
	if (bInherit)
		EnumChildWindows(hwnd, EnumChildWindowsCallBack, (LPARAM)&data);
}
BOOL YunLai::EnumChildWindowsCallBack(HWND hwnd, LPARAM lParam)
{
	EnumChildWindowData& data = *(EnumChildWindowData*)lParam;
	if (hwnd != nullptr)
	{	
		switch (data.status)
		{
			case 1:PostMessage(hwnd, 258, data.key, 0); break;
			case 2:
			{
				PostMessage(hwnd, 260, data.key, 0);
				PostMessage(hwnd, 261, data.key, 0);
				break;
			}
			case 3:PostMessage(hwnd, 256, data.key, 0);	break;
			case 4:PostMessage(hwnd, 257, data.key, 0);	break;
			default:
			{
				PostMessage(hwnd, 256, data.key, 0);
				PostMessage(hwnd, 257, data.key, 0);
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}

char* YunLai::ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen)
{
	bool bRet = false;
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress,NULL,16);
	char pLen[MAX_MEMORY_TEXT_SIZE]="";
	if(nLen > MAX_MEMORY_TEXT_SIZE)
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
	else
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, nLen, 0);
//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
	return pLen;
}
char* YunLai::ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, int nLen)
{
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	return ReadMemoryStrFromProcessID(hProcessID,szAddress, nLen);
}
char* YunLai::ReadMemoryStrFromWnd(HWND hwnd, const char* szAddress, const char* offset1, const char* offset2, const char* offset3, const char* offset4)
{
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);

	/*int nAddress = ReadMemoryIntFromProcessID(hProcessID, HexTextToDecimalText(szAddress));
	if (strlen(offset1) < 1)
		return nAddress;
	*	��ֵ(�����ֲ�����_3400, �ڴ������(�����ֲ�����_3397, ת��ʮ����ʮ(��ַ)))
		.�����(�Ƿ�Ϊ��(ƫ��1))
		����(�����ֲ�����_3400)
		.��������
		��ֵ(�����ֲ�����_3400, �ڴ������(�����ֲ�����_3397, ���(�����ֲ�����_3400, ת��ʮ����ʮ(ƫ��1))))
		.�����(�Ƿ�Ϊ��(ƫ��2))
		����(�����ֲ�����_3400)
		.��������
		��ֵ(�����ֲ�����_3400, �ڴ������(�����ֲ�����_3397, ���(�����ֲ�����_3400, ת��ʮ����ʮ(ƫ��2))))
		.�����(�Ƿ�Ϊ��(ƫ��3))
		����(�����ֲ�����_3400)
		.��������
		��ֵ(�����ֲ�����_3400, �ڴ������(�����ֲ�����_3397, ���(�����ֲ�����_3400, ת��ʮ����ʮ(ƫ��3))))
		.�����(�Ƿ�Ϊ��(ƫ��4))
		����(�����ֲ�����_3400)
		.��������
		��ֵ(�����ֲ�����_3400, �ڴ������(�����ֲ�����_3397, ���(�����ֲ�����_3400, ת��ʮ����ʮ(ƫ��4))))
		����(�����ֲ�����_3400)*/
	return "";
}

int YunLai::ReadMemoryIntFromWnd(HWND hwnd, const char* szAddress)
{
	//PROCESS_ALL_ACCESS = ����Ȩ�� = 2035711
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	int memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID), pAddress, &memoryData, 4, 0);
	//qDebug() << hProcessID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

int YunLai::ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress)
{
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	int memoryData = 0;
	bool bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, &memoryData, 4, 0);
	//qDebug() << processID << szAddress << memoryData << bRet << pAddress;
	return memoryData;
}

char* YunLai::GetKeyTextFromKey(int nKey)
{
	switch (nKey)
	{
	case VK_F1:return "F1";
	case VK_F2:return "F2";
	case VK_F3:return "F3";
	case VK_F4:return "F4";
	case VK_F5:return "F5";
	case VK_F6:return "F6";
	case VK_F7:return "F7";
	case VK_F8:return "F8";
	case VK_F9:return "F9";
	case VK_F10:return "F10";
	case VK_F11:return "F11";
	case VK_F12:return "F12";
	default:;break;
	}
	return "";
}
//������, �ɿ�, �ɿ�:Ϊ���  1 #���   2 #�Ҽ�   3 #�м�
//.���� ����, ������, �ɿ�, �ɿ�:Ϊ����  1 #����   2 #˫��   3 #����  4 #�ſ�
void YunLai::MouseClickedEvent(int nType, int nCtrl)
{	
	DWORD mouseDown = 2;//MOUSEEVENTF_LEFTDOWN
	DWORD mouseUp = 4;//MOUSEEVENTF_LEFTUP
	switch (nType)
	{
	case 1:
	{
		mouseDown = 2;
		mouseUp = 4;
		break;
	}
	case 2:
	{
		mouseDown = 8;
		mouseUp = 16;
		break;
	}
	case 3:
	{
		mouseDown = 32;
		mouseUp = 64;
		break;
	}
	default:
		break;
	}
	switch (nCtrl)
	{
	case 2:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		uint clickTime= GetDoubleClickTime();
		Sleep(clickTime);
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		break;
	}
	case 3:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);	break;
	}
	case 4:
	{
		mouse_event(mouseUp, 0, 0, 0, 0);	break;
	}
	case 1:
	default:
	{
		mouse_event(mouseDown, 0, 0, 0, 0);
		mouse_event(mouseUp, 0, 0, 0, 0);
		break;
	}
	}
}
//������ʱ, , , �߾�����ʱ, cpuռ�õ�, ���ڲ�����, һ��������ʱ����(�޷���ֵ)
//.���� ��ʱ���, ������, , 1000΢�� = 1���� �� 1000���� = 1��
//.���� ��ʱ��λ, ������, �ɿ�, �ɿ�:����  0 ����  1 ΢��  2 ��  3 ��  4 Сʱ  5 ��
void YunLai::SuperTimeDelay(int timeDelay, int timeType)
{
	int nTimeUnit = 0;
	if (timeType == 0)
		nTimeUnit = 1;
	else if (timeType == 1)//΢�� ֱ�ӵ���
	{
		HANDLE hHandle = CreateWaitableTimerA(0, false, 0);
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = timeDelay * (-10);
		SetWaitableTimer(hHandle, &liDueTime, 0, 0, 0, false);
		if (MsgWaitForMultipleObjects(1, &hHandle, false, -1, 255) != 0)
		{
			//		�����¼�()
		}
		CloseHandle(hHandle);
	}
	else if(timeType==2)
		nTimeUnit = 1000;
	else if (timeType == 3)
		nTimeUnit = 1000*60;
	else if (timeType == 4)
		nTimeUnit = 1000*60*60;
	else if (timeType == 5)
		nTimeUnit = 1000 * 60 * 60*24;
	for (int i=0;i<nTimeUnit;++i)
	{
		HANDLE hHandle = CreateWaitableTimerA(0, false, 0);
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = timeDelay * (-10)*1000;
		SetWaitableTimer(hHandle, &liDueTime, 0, 0, 0, false);
		if (MsgWaitForMultipleObjects(1, &hHandle, false, INFINITE, QS_ALLINPUT) != 0)
		{
			//		�����¼�()
		}
		CloseHandle(hHandle);
		break;
	}
}

char* YunLai::HexTextToDecimalText(const char* szHex)
{
	int nAdd = atoi(szHex);
	char szAddr[25];
	itoa(nAdd,szAddr,16);
	return szAddr;
}

char* YunLai::MyItoa(int num, char* str, int radix)
{/*������*/
	char index[] = "0123456789ABCDEF";
	unsigned unum;/*�м����*/
	int i = 0, j, k;
	/*ȷ��unum��ֵ*/
	if (radix == 10 && num < 0)/*ʮ���Ƹ���*/
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;/*�������*/
	/*ת��*/
	do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	/*����*/
	if (str[0] == '-')
		k = 1;/*ʮ���Ƹ���*/
	else
		k = 0;

	for (j = k; j <= (i - 1) / 2; j++)
	{
		char temp;
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}

DWORD YunLai::GetProcessIDFromWnd(HWND hwnd)
{
	DWORD hProcessID = 0;
 	DWORD dwTID = GetWindowThreadProcessId(hwnd, &hProcessID);//�����̺߳�
	return hProcessID;
}

bool YunLai::ActiveWnd(HWND hwnd)
{
	DWORD hProcessID = GetWindowThreadProcessId(hwnd, 0);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, true);
	SetActiveWindow(hwnd);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, false);
	return SetForegroundWindow(hwnd);
}

bool YunLai::SetFocusToWnd(HWND hwnd)
{
	DWORD hProcessID = GetWindowThreadProcessId(hwnd, 0);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, true);
	HWND focusWnd  = SetFocus(hwnd);
	AttachThreadInput(GetCurrentThreadId(), hProcessID, false);
	if (GetFocus() == focusWnd)	//���㴰�ڵ���֮ǰ�Ľ��㴰�� ������ʧ��
		return false;
	return true;
}

bool YunLai::IsVisibleWnd(HWND hwnd)
{
	return IsWindowVisible(hwnd);
}

bool YunLai::SetWindowShow(HWND hwnd)
{
	if (hwnd == nullptr)
	{
		if (m_lastHwnd == nullptr)
			m_lastHwnd;
	}else
		m_lastHwnd = hwnd;
	if (IsVisibleWnd(m_lastHwnd) == false)
	{
		::ShowWindow(m_lastHwnd, 9);
		return true;
	}
	return false;
}
bool YunLai::SetWindowHide(HWND hwnd)
{
	if (hwnd == nullptr)
	{
		if (m_lastHwnd == nullptr)
			m_lastHwnd;
	}
	else
		m_lastHwnd = hwnd;
	if (IsVisibleWnd(m_lastHwnd))
	{
		::ShowWindow(m_lastHwnd, 0);
		return true;
	}
	return false;
}
	
