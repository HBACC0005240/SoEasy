#include "YunLai.h"
#include <tlhelp32.h>
#include <psapi.h> 
#include <QString>
#include <QMap>
#include <QDebug>
#include "../include/ITPublic.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")
#include "LibApiImport.h"
static HWND m_lastHwnd = nullptr;
#define FORMAT_PATH(path) path.replace('\\','/').toLower()

virtual_buffer_t::virtual_buffer_t() : m_cbSize(0), m_pBuffer(NULL)
{
}
virtual_buffer_t::virtual_buffer_t(size_t size) : m_cbSize(size), m_pBuffer(VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE))
{
}

virtual_buffer_t::~virtual_buffer_t()
{
	if (m_pBuffer)
		VirtualFree(m_pBuffer, 0, MEM_RELEASE);
}

void * virtual_buffer_t::GetSpace(size_t needSize)
{
	if (m_cbSize < needSize)
	{
		if (m_pBuffer)
			VirtualFree(m_pBuffer, 0, MEM_RELEASE);
		m_pBuffer = VirtualAlloc(NULL, needSize, MEM_COMMIT, PAGE_READWRITE);
		m_cbSize = needSize;
	}
	return m_pBuffer;
}

crt_buffer_t::crt_buffer_t() : m_cbSize(0), m_pBuffer(NULL)
{
}

crt_buffer_t::crt_buffer_t(size_t size) : m_cbSize(size), m_pBuffer(malloc(size))
{
}

crt_buffer_t::~crt_buffer_t()
{
	if (m_pBuffer)
		free(m_pBuffer);
}

void * crt_buffer_t::GetSpace(size_t needSize)
{
	if (m_cbSize < needSize)
	{
		if (m_pBuffer)
			m_pBuffer = realloc(m_pBuffer, needSize);
		else
			m_pBuffer = malloc(needSize);
		m_cbSize = needSize;
	}
	return m_pBuffer;
}
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

int YunLai::GetColorDepth()
{
	DEVMODEW dMode;
	dMode.dmSize = sizeof(DEVMODE);
	dMode.dmDriverExtra = 0;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS,&dMode);
	return dMode.dmBitsPerPel;//�����ص�λ��Ϊ��λ�����磬16ɫʹ��4λ��256ɫʹ��8λ����65536ɫʹ��16λ��
}
//��Ļȡ��ɫ, ������, , ȡ��Ļ��ָ���������ɫ, ����Ļλͼ��ָ���������������ɫ.(����10������ɫֵ)
//.���� ˮƽλ��, ������, , ָ���������
//.���� ��ֱλ��, ������, , ָ����������
//.���� ���ھ��, ������, �ɿ�, ָ�����ھ����, ���ȡ���ڿͻ�����ָ���������ɫֵ
DWORD YunLai::GetScreenColor(int x, int y, HWND hwnd)
{
	POINT screenPoint;
	if (hwnd)
	{
		ClientToScreen(hwnd,&screenPoint);
	}
	HDC pHDC = GetDC(nullptr);//��Ļ��
	DWORD dColor = GetPixel(pHDC, x + screenPoint.x, y + screenPoint.y);
	ReleaseDC(nullptr,pHDC);
}

POINT YunLai::FindBMPColor(uchar* pData, DWORD nColor, int nDVal, POINT nFindPoint)
{
	return POINT();
	/*.�ֲ����� �����ֲ�����_6697, ������������_6732, , ,
		.�ֲ����� �����ֲ�����_6698, ������, , ,
		.�ֲ����� �����ֲ�����_6699, ������, , ,
		.�ֲ����� �����ֲ�����_6700, ������, , ,
		.�ֲ����� �����ֲ�����_6701, �ֽڼ�, , ,
		.�ֲ����� �����ֲ�����_6702, �ֽڼ�, , ,

		��ֵ(�����ֲ�����_6699.x, -1)
		��ֵ(�����ֲ�����_6699.y, -1)
		��ֵ(�����ֲ�����_6697, λͼȡɫ��(�ֽڼ�))
		.�����(����(�����ֲ�����_6697.������Ա6735, 0))
		����(�����ֲ�����_6699)
		.��������
		.���(����(С��(��ʼ����.x, 1), С��(��ʼ����.y, 1)))
		��ֵ(�����ֲ�����_6698, 55)
		.����
		��ֵ(�����ֲ�����_6698, ���(���(���(���(���(�����ֲ�����_6697.������Ա6734, ��ʼ����.y, 1), �����ֲ�����_6697.������Ա6733), ��ʼ����.x), �����ֲ�����_6697.������Ա6737), 55, �����ֲ�����_6697.������Ա6737))
		.�������
		��ֵ(�����ֲ�����_6701, ���ֽڼ�(��ɫ))
		.���(����(���, 0))
		.����ѭ����(�����ֲ�����_6698, ���(ȡ�ֽڼ�����(�ֽڼ�), 2), �����ֲ�����_6697.������Ա6737, �����ֲ�����_6700)
		.�����(����(С��(ȡ����ֵ(���(�ֽڼ�[���(�����ֲ�����_6700, 2)], �����ֲ�����_6701[1])), ���(���, 1)), С��(ȡ����ֵ(���(�ֽڼ�[���(�����ֲ�����_6700, 1)], �����ֲ�����_6701[2])), ���(���, 1)), С��(ȡ����ֵ(���(�ֽڼ�[�����ֲ�����_6700], �����ֲ�����_6701[3])), ���(���, 1))))
		��ֵ(�����ֲ�����_6699.x, ���(������(���(���(���(�����ֲ�����_6700, 55), �����ֲ�����_6697.������Ա6737), 1), �����ֲ�����_6697.������Ա6733), 1))
		��ֵ(�����ֲ�����_6699.y, ���(�����ֲ�����_6697.������Ա6734, ����(���(���(���(�����ֲ�����_6700, 55), �����ֲ�����_6697.������Ա6737), 1), �����ֲ�����_6697.������Ա6733), 1))
		����(�����ֲ�����_6699)
		.��������

		.����ѭ��β()
		����(�����ֲ�����_6699)
		.����
		��ֵ(�����ֲ�����_6702, { 0, 0, 0 })
		��ֵ(�����ֲ�����_6702[1], �����ֲ�����_6701[3])
		��ֵ(�����ֲ�����_6702[2], �����ֲ�����_6701[2])
		��ֵ(�����ֲ�����_6702[3], �����ֲ�����_6701[1])
		��ֵ(�����ֲ�����_6700, Ѱ���ֽڼ�(�ֽڼ�, �����ֲ�����_6702, �����ֲ�����_6698))
		.�ж�ѭ����(����(������(���(�����ֲ�����_6700, 55), �����ֲ�����_6697.������Ա6737), 0))
		.�����(����(�����ֲ�����_6700, -1))
		����(�����ֲ�����_6699)
		.��������
		��ֵ(�����ֲ�����_6700, Ѱ���ֽڼ�(�ֽڼ�, �����ֲ�����_6702, ���(�����ֲ�����_6700, 1)))
		.�ж�ѭ��β()
		.�����(����(�����ֲ�����_6700, -1))
		��������ı�(�����ֲ�����_6697.������Ա6733)
		��������ı�(�����ֲ�����_6700)
		����(�����ֲ�����_6699)
		.��������
		��ֵ(�����ֲ�����_6699.x, ���(������(���(���(���(�����ֲ�����_6700, 55), �����ֲ�����_6697.������Ա6737), 1), �����ֲ�����_6697.������Ա6733), 1))
		��ֵ(�����ֲ�����_6699.y, ���(�����ֲ�����_6697.������Ա6734, ����(���(���(���(�����ֲ�����_6700, 55), �����ֲ�����_6697.������Ա6737), 1), �����ֲ�����_6697.������Ա6733), 1))
		����(�����ֲ�����_6699)
		.�������*/

}

POINT YunLai::FindScreenColor(DWORD nColor, int nLeftX, int nLeftY, int nWidth, int nHeight, int nDVal, POINT nFindPoint, HWND hwnd/*=nullptr*/)
{
	return FindBMPColor(nColor);
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

void YunLai::WriteMemoryIntToWnd(HWND hwnd, const char* szAddress, int nVal)
{
//	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
	DWORD hProcessID = GetProcessIDFromWnd(hwnd);
	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
}
void YunLai::WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal)
{
	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
}

bool YunLai::ForceOpenProcess(DWORD hProcessID, DWORD nExitStatus)
{

}

bool YunLai::ForceCloseProcess(DWORD hProcessID, DWORD nExitStatus)
{
	bool bRet = false;
	if (ZwCreateJobObject_Import())
	{

	}
		��ֵ(�����ֲ�����_7624.������Ա7638, 24)
		�����ӳ���_27995(��ntdll.dll��, ��ZwCreateJobObject��)
		��ֵ(�����ֲ�����_7622, ZwCreateJobObject_7699(�����ֲ�����_7623, #��������_7704, �����ֲ�����_7624))
		.�����(���ڻ����(�����ֲ�����_7622, 0))
		�����ӳ���_27995(��ntdll.dll��, ��ZwAssignProcessToJobObject��)
		��ֵ(�����ֲ�����_7622, ZwAssignProcessToJobObject_7705(�����ֲ�����_7623, hProcess))
		.�����(���ڻ����(�����ֲ�����_7622, 0))
		�����ӳ���_27995(��ntdll.dll��, ��ZwTerminateJobObject��)
		��ֵ(�����ֲ�����_7622, ZwTerminateJobObject_7708(�����ֲ�����_7623, ExitStatus))
		.�����(���ڻ����(�����ֲ�����_7622, 0))
		��ֵ(�����ֲ�����_7625, ��)
		.��������

		.��������
		�����ӳ���_27995(��ntdll.dll��, ��ZwClose��)
		ZwClose_7697(�����ֲ�����_7623)
		.��������
		.�����(����(�����ֲ�����_7625, ��))
		�����ӳ���_27995(��ntdll.dll��, ��ZwTerminateProcess��)
		��ֵ(�����ֲ�����_7622, ZwTerminateProcess_7711(hProcess, ExitStatus))
		.�����(���ڻ����(�����ֲ�����_7622, 0))
		��ֵ(�����ֲ�����_7625, ��)
		.��������

		.��������
		����(�����ֲ�����_7625)
}
//��ȡ����  ���� ����  ϵͳ���ǿ��� ����ȫ·��
HMODULE YunLai::GetLibraryHandle(const char* szLib)
{
	HMODULE hModule = ::GetModuleHandle((LPCWSTR)szLib);
	if (NULL == hModule) hModule = ::LoadLibrary((LPCWSTR)szLib);
	if (NULL == hModule) throw;
	return hModule;
}

FARPROC YunLai::GetFunAddress(const char* szLib, const char* szFun)
{
	HMODULE hModel = GetLibraryHandle(szLib);
	if (hModel == nullptr)
		return nullptr;
	FARPROC funAddr = GetProcAddress(hModel, (LPCSTR)szFun);
	if (funAddr == nullptr)
		return nullptr;
	return funAddr;
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

void YunLai::KeyClickedEvent()
{
	//PostMessage(aHandle, WM_char, ord(s[i]), 0); //�����ַ�
	keybd_event(13, 0, 0, 0);//�س�
}

int YunLai::MakeKeyLParam(int VirtualKey, int flag)
{
	UINT sCode;
	//Firstbyte ; lparam ������ 24-31λ
	UINT Firstbyte;
	switch (flag)
	{
	case WM_KEYDOWN:    Firstbyte = 0;   break;
	case WM_KEYUP:      Firstbyte = 0xC0; break;
	case WM_CHAR:       Firstbyte = 0x20; break;
	case WM_SYSKEYDOWN: Firstbyte = 0x20; break;
	case WM_SYSKEYUP:   Firstbyte = 0xE0; break;
	case WM_SYSCHAR:    Firstbyte = 0xE0; break;
	}
	// ����ɨ����; lparam ���� �� 16-23λ
	// 16�C23 Specifies the scan code. 
	UINT iKey = MapVirtualKeyW(VirtualKey, 0);
	// 1Ϊ lparam ������ 0-15λ�������ʹ���
	// 0�C15 Specifies the repeat count for the current message. 
	sCode = (Firstbyte << 24) + 1 + (iKey << 16) + 1;
	return sCode;
}

void YunLai::SimKeyClick(UINT vk_Code, BOOL bDown)
{
	DWORD dwFlages = 0;
	switch (vk_Code)
	{
	default:
		break;
	case(VK_NUMLOCK):
	case(VK_CAPITAL):
	case(VK_SCROLL):
	case(VK_CONTROL):
	case(VK_LCONTROL):
	case(VK_RCONTROL):
	case(VK_SHIFT):
	case(VK_LSHIFT):
	case(VK_RSHIFT):
	case(VK_MENU):
	case(VK_LMENU):
	case(VK_RMENU):
		dwFlages |= KEYEVENTF_EXTENDEDKEY;
	}
	WORD wScan = MapVirtualKeyW(vk_Code, 0);
	INPUT Input[1] = { 0 };
	Input[0].type = INPUT_KEYBOARD;
	Input[0].ki.wVk = vk_Code;
	Input[0].ki.wScan = wScan;
	Input[0].ki.dwFlags = (bDown) ? dwFlages : dwFlages | KEYEVENTF_KEYUP;
	SendInput(1, Input, sizeof(INPUT));
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
//ģ�������һ��
void YunLai::SimMouseClick(int nType, int nCtrl)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.mouseData = 0;
	//input.mi.dwFlags = bDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;

	DWORD mouseDown = MOUSEEVENTF_LEFTDOWN;
	DWORD mouseUp = MOUSEEVENTF_LEFTUP;
	switch (nType)
	{
	case 1:
	{
		mouseDown = MOUSEEVENTF_LEFTDOWN;
		mouseUp = MOUSEEVENTF_LEFTUP;
		break;
	}
	case 2:
	{
		mouseDown = MOUSEEVENTF_RIGHTDOWN;
		mouseUp = MOUSEEVENTF_RIGHTUP;
		break;
	}
	case 3:
	{
		mouseDown = MOUSEEVENTF_MIDDLEDOWN;
		mouseUp = MOUSEEVENTF_MIDDLEUP;
		break;
	}
	default:
		break;
	}
	switch (nCtrl)
	{
	case 2:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		uint clickTime = GetDoubleClickTime();
		Sleep(clickTime);
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		break;
	}
	case 3:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		break;

	}
	case 4:
	{
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
		break;
	}
	case 1:
	default:
	{
		input.mi.dwFlags = mouseDown;
		SendInput(1, &input, sizeof(INPUT));
		input.mi.dwFlags = mouseUp;
		SendInput(1, &input, sizeof(INPUT));
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
//
//bool YunLai::DIBSnapshot(HWND hWnd, int scale, const SnapshotCallback &callback)
//{
//	HDC hDC = GetDC(hWnd);
//	if (!hDC)
//	{
//		return false;
//	}
//
//	const auto hDCScope = std::experimental::make_scope_exit([hWnd, hDC] { ReleaseDC(hWnd, hDC); });
//
//	RECT rcScreen;
//
//	if (hWnd == GetDesktopWindow())
//	{
//		rcScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
//		rcScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
//		rcScreen.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//		rcScreen.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//	}
//	else
//	{
//		GetWindowRect(hWnd, &rcScreen);
//	}
//
//	int destW = (scale == 100) ? (rcScreen.right - rcScreen.left) : (rcScreen.right - rcScreen.left) * scale / 100;
//	int destH = (scale == 100) ? (rcScreen.bottom - rcScreen.top) : (rcScreen.bottom - rcScreen.top) * scale / 100;
//
//	BITMAPINFOHEADER bi;
//	memset(&bi, 0, sizeof(bi));
//	bi.biSize = sizeof(bi);
//	bi.biWidth = destW;
//	bi.biHeight = -destH;
//	bi.biPlanes = 1;
//	bi.biBitCount = 32;
//	bi.biCompression = BI_RGB;
//
//	PVOID pv = NULL;
//	HBITMAP hBitmap = CreateDIBSection(hDC, (BITMAPINFO *)&bi, DIB_RGB_COLORS, &pv, 0, 0);
//	if (!hBitmap)
//	{
//		OutputDebugStringA("CreateDIBSection failed");
//		return false;
//	}
//	const auto hBitmapScope = std::experimental::make_scope_exit([hBitmap] { DeleteObject(hBitmap); });
//
//	HDC hMemDC = CreateCompatibleDC(hDC);
//	if (!hMemDC)
//	{
//		OutputDebugStringA("CreateCompatibleDC failed");
//		return false;
//	}
//	const auto hMemDCScope = std::experimental::make_scope_exit([hMemDC] { DeleteDC(hMemDC); });
//
//	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);
//
//	const auto hOldBmpScope = std::experimental::make_scope_exit([hMemDC, hOldBmp] { SelectObject(hMemDC, hOldBmp); });
//
//	if (scale != 100)
//	{
//		SetStretchBltMode(hMemDC, STRETCH_HALFTONE);
//
//		if (!StretchBlt(hMemDC, 0, 0, destW, destH, hDC, 0, 0, rcScreen.right, rcScreen.bottom, SRCCOPY))
//		{
//			OutputDebugStringA("StretchBlt failed");
//			return false;
//		}
//	}
//	else
//	{
//		if (!BitBlt(hMemDC, 0, 0, rcScreen.right, rcScreen.bottom, hDC, 0, 0, SRCCOPY))
//		{
//			OutputDebugStringA("BitBlt failed");
//			return false;
//		}
//	}
//
//	BITMAP bitmap;
//	if (!GetObjectW(hBitmap, sizeof(BITMAP), &bitmap))
//	{
//		OutputDebugStringA("GetObjectW failed");
//		return false;
//	}
//	PVOID pBuffer = m_snapshot_buffer.GetSpace(bitmap.bmHeight * bitmap.bmWidthBytes);
//	if (!pBuffer)
//	{
//		OutputDebugStringA("m_snapshot_buffer.GetSpace failed");
//		return false;
//	}
//
//	if (GetBitmapBits(hBitmap, bitmap.bmHeight * bitmap.bmWidthBytes, pBuffer) <= 0)
//	{
//		OutputDebugStringA("GetBitmapBits failed");
//		return false;
//	}
//
//	callback(pBuffer, bitmap.bmHeight * bitmap.bmWidthBytes, bitmap.bmWidth, bitmap.bmHeight, bitmap.bmBitsPixel);
//
//	return true;
//}

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
	

//
//bool YunLai::DIBToCvMat(cv::Mat &mat, void *pBuffer, size_t cbBuffer, int width, int height, int bbp)
//{
//	mat.create(height, width, CV_8UC3);
//
//	int nChannels = bbp / 8;
//
//	int nStep = nChannels * width;
//
//	for (int nRow = 0; nRow < height; nRow++)
//	{
//		auto pucRow = (mat.ptr<uchar>(nRow));
//		for (int nCol = 0; nCol < width; nCol++)
//		{
//			pucRow[nCol * 3 + 0] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 0);
//			pucRow[nCol * 3 + 1] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 1);
//			pucRow[nCol * 3 + 2] = *((uchar *)pBuffer + nRow * nStep + nCol * nChannels + 2);
//		}
//	}
//
//	return true;
//}