// QingFengGJ.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h> 
#include <list>
#include <tchar.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")
#include <map>
#include <stdio.h>
#include "XASM.h"
using namespace std;
std::map<DWORD, std::string> g_mlProcess;	//清风魔力的进程 定时轮询所有
XASM m_asm;
#pragma warning(disable 4096)
#define _CRT_SECURE_NO_DEPRECATE
bool EnableDebugPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}
	return true;
}
void GetAllProcess(std::map<DWORD, std::string>& processInfo)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//获取进程列表失败
		return;
	}
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		std::string exeName = pe32.szExeFile;	
		std::cout << exeName.c_str() << "\n";
		if (exeName.find("qfmoli.exe") != string::npos)//qfmoli
		{
			processInfo[pe32.th32ProcessID] = exeName;
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}
int ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress)
{
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	EnableDebugPriv();
	HANDLE hModel = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
	if (hModel == nullptr)
	{
		std::cout << "OpenProcess Failed!\n";
		return 0;
	}
	int memoryData = 0;
	bool bRet = ReadProcessMemory(hModel, pAddress, &memoryData, 4, 0);
	return memoryData;
}
void WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal)
{
	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
	EnableDebugPriv();
	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
}
void moveToPoint(DWORD hProcessID, int x, int y)
{
	WriteMemoryIntToProcess(hProcessID, "008E543C", x);
	WriteMemoryIntToProcess(hProcessID, "008E5440", y);
	EnableDebugPriv();
	std::cout << "moveToPoint" << x << "," << y <<"\n";
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessID);
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044D000), NULL, 0, &tId);
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ..
	CloseHandle(hRemoteThread);
	CloseHandle(hTarget);
	std::cout << "moveToPointSuccess\n";
}
POINT GetGamePersonCoordinate(DWORD hProcessID)
{
	POINT pointPerson;
	pointPerson.x = ReadMemoryIntFromProcessID(hProcessID, "0092BD28");
	pointPerson.y = ReadMemoryIntFromProcessID(hProcessID, "0092BD30");
	return pointPerson;
}
enum MOVE_DIRECTION
{
	MOVE_DIRECTION_UP = 1,
	MOVE_DIRECTION_DOWN = 5,
	MOVE_DIRECTION_LEFT = 7,
	MOVE_DIRECTION_RIGHT = 3,
	MOVE_DIRECTION_LEFTUP = 0,
	MOVE_DIRECTION_RIGHTUP = 2,
	MOVE_DIRECTION_LEFTDOWN = 6,
	MOVE_DIRECTION_RIGHTDOWN = 4,
};

//貌似界面不显示 这边就无效
void turn_about(DWORD hProcessID, int nDirection)
{
	//调用call方式
	POINT points = GetGamePersonCoordinate(hProcessID);
	switch (nDirection)
	{
	case MOVE_DIRECTION_UP:
	{
		points.x = points.x + 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_DOWN:
	{
		points.x = points.x - 1;
		points.y = points.y + 1;
		break;
	}
	case MOVE_DIRECTION_LEFT:
	{
		points.x = points.x - 1;
		points.y = points.y - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHT:
	{
		points.x = points.x + 1;
		points.y = points.y + 1;
	}
	case MOVE_DIRECTION_LEFTUP:	points.y = points.y - 1; break;
	case MOVE_DIRECTION_RIGHTUP:
	{
		points.x = points.x + 1;
		break;
	}
	case MOVE_DIRECTION_LEFTDOWN:
	{
		points.x = points.x - 1;
		break;
	}
	case MOVE_DIRECTION_RIGHTDOWN:
	{
		points.y = points.y + 1;
		break;
	}default:return;
	}
	std::cout << "turn_about "<< points.x<<":"<< points.y << " direction "<< nDirection <<"\n";
	WriteMemoryIntToProcess(hProcessID, "00BC7B58", points.x);
	WriteMemoryIntToProcess(hProcessID, "00BC7B5C", points.y);
	EnableDebugPriv();
	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessID);
	DWORD tId;
	HANDLE hRemoteThread= CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	CloseHandle(hRemoteThread);
	CloseHandle(hTarget);
	return;
}
int getPersionHP(DWORD processID)
{
	int nHP = ReadMemoryIntFromProcessID(processID, "00E2A418");
	return nHP;
}

int getPersionMP(DWORD processID)
{
	int nMP = ReadMemoryIntFromProcessID(processID, "00E2A420");
	return nMP;
}
//ANSI转化成UNICODE
static LPWSTR ANSITOUNICODE1(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW = 0;
	LPWSTR lpszFile = nullptr;
	lenW = MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, 0, 0);
	if (lenW > 0)
	{
		lpszFile = SysAllocStringLen(0, lenW);
		MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, lpszFile, lenW);
	}
	return lpszFile;
}
#define MAX_MEMORY_TEXT_SIZE 10000
char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen)
{
	EnableDebugPriv();
	bool bRet = false;
	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
	if (nLen > MAX_MEMORY_TEXT_SIZE)
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
	else
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, nLen, 0);
	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
	return pLen;
}
char* ReadMemoryStrFromProcessID(DWORD processID, DWORD pAddress, int nLen)
{
	EnableDebugPriv();
	bool bRet = false;
	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
	if (nLen > MAX_MEMORY_TEXT_SIZE)
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
	else
		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, nLen, 0);
	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
	return pLen;
}
std::string GetGameMapName(DWORD processID)
{
	string szMapName = string((ReadMemoryStrFromProcessID(processID, "009181C0", 100)));
	return szMapName;
}


bool moveto(DWORD processID,int x,int y)
{	
	int nTimeOut = 60000;//1分钟走不到 退出
	POINT lastPoint = GetGamePersonCoordinate(processID);//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
	while (1)
	{		
		POINT gamePoint = GetGamePersonCoordinate(processID);
		std::cout << "while(1)moveto" << gamePoint.x << "," << gamePoint.y << "destPoint" <<x <<"" <<y<<"\n";
		if (gamePoint.x == x && gamePoint.y == y)
		{
			break;
		}
		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
		{
			moveToPoint(processID,x,y);
		}
		lastPoint = gamePoint;
		Sleep(1000);
	}
	Sleep(100);//成功后 新坐标 等待一下
	std::cout << "moveto" << x <<"," << y<< "success \n";
	return true;
}
bool high(DWORD processID, int x, int y,int dstx,int dsty)
{
	int nTimeOut = 60000;//1分钟走不到 退出
	POINT lastPoint = GetGamePersonCoordinate(processID);//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
	while (1)
	{
		POINT gamePoint = GetGamePersonCoordinate(processID);
		std::cout << "while(1)high" << gamePoint.x << "," << gamePoint.y << "destPoint" << x << "" << y << "\n";
		if (gamePoint.x == dstx && gamePoint.y == dsty)
		{
			break;
		}
		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
		{
			moveToPoint(processID, x, y);
		}
		lastPoint = gamePoint;
		Sleep(1000);
	}
	Sleep(100);//成功后 新坐标 等待一下
	std::cout << "moveto" << x << "," << y << "success \n";
	return true;
}
bool nowhile(DWORD processID, string szData)
{	
	int nTryNum = 0;
	while (1)//战斗中 等待
	{
		nTryNum++;
		string szMap = GetGameMapName(processID);
		if (szMap.find(szData)!=string::npos)
		{
			std::cout << "nowhile";// << szData.c_str();
			break;
		}
		Sleep(100);
		if (nTryNum >= 100)
			break;
	}	
	return true;
}

#define  BYTE_MAX 4096
bool transAssemble(void* pAddr, BYTE* buf, int nLen, list<string> szCmdList)
{
	wstring szMsg;
	char error[256] = { 0 };
	int count = szCmdList.size();
	int j = 0;
	int m = 0;
	for (auto it = szCmdList.begin();it!= szCmdList.end();++it)
	{
		char cmd[256] = { 0 };
		string szCmd = *it;
		int len = szCmd.size();// m_edit.GetLine(i, cmd, 256);
		if (len == 0) continue;
		memcpy(cmd, szCmd.c_str(), len);
		cmd[len] = '\0';
		t_asmmodel t_asm;
		j = m_asm.Assemble(cmd, (DWORD)pAddr + m, &t_asm, 0, 4, error);
		if (j <= 0)
		{
			std::cout << "error=" << error;
		}
		for (int k = 0; k < j; k++)
		{
			buf[m] = (BYTE)t_asm.code[k];
			std::cout << buf[m];
			m = m + 1;
			if (m > nLen)
				return false;//超出内存地址
		}
	}
	if ((m + 2) > nLen)
		return false;//超出内存地址
	buf[m] = 0x0c2;//ret 4
	buf[m + 1] = 0x04;
	buf[m + 2] = 0x00;	
	return true;
}
void __declspec(naked) WorkCall(DWORD* nindex)
{
	__asm
	{
		mov ebx, [esp+4] //xy
		mov eax, [ebx]
		mov ecx,0
		mov edx,0x00462EF8
		call edx
		ret
	}
}
__declspec(naked)  void WorkCall()
{
	_asm
	{
		mov eax, 0
		mov ecx, 0
		mov edx, 0x00462EF8
		call edx
		ret
	}
}
void Work(DWORD processID, int nIndex)
{	
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	DWORD nThreadSize = 0x8FFF;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		printf("提示：申请空间失败\n");
		return;
	}
	LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);
//	DWORD oldProtect = 0;
//	VirtualProtectEx(hProcess, callBase, nThreadSize, PAGE_EXECUTE_READWRITE, &oldProtect);//设置具有读写权限
	for (int i = 0; i < 15; ++i)//先10个  15个技能栏
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		std::string skillName = ReadMemoryStrFromProcessID(processID, pAddress, 100);
		pAddress += 0x38;
		int nShowIndex = i;//YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
		if (skillName.find("挖矿") != string::npos)
		{
			nIndex = i;
			std::cout << "挖矿 " << nIndex<<"\n";
			break;
		}
	}
	BYTE buf[100] = { 0 };
	list<string> szAssemble;
	char sVal[25] = { 0 };
	sprintf(sVal,"mov eax,0x%x",nIndex);
	szAssemble.push_back((sVal));
	szAssemble.push_back(("mov ecx,0"));
	szAssemble.push_back(("mov edx,0x00462EF8"));
	szAssemble.push_back(("call edx"));
	szAssemble.push_back(("ret"));
	bool bTAssable = transAssemble(callBase, buf, nThreadSize, szAssemble);
	if (bTAssable == false)
	{
		std::cout << "transAssemble Failed!" <<buf <<"\n";
		return;
	}
	//std::cout << "transAssemble Success!" << buf << "\n";
	//Sleep(1000);//太快？
	DWORD byWrite;
	//BYTE testData[100] = { 0x8B,0x5C ,0x24 ,0x04 ,0x8B ,0x03 ,0xB9 ,0x00 ,0x00 ,0x00 ,0x00 ,0xBA ,0xF8 ,0x2E ,0x46 ,0x00 ,0x8B ,0xC2 ,0xFF ,0xD0 ,0xC3 };//{ 0x66,0xbb,0x20,0x20,0x20,0x20,0x66,0x67,0x8b,0x03,0x66,0xb9,0x20,0x20,0x20,0x20,0x66,0xba,0xf8,0x2e,0x46,0x20,0x66,0xff,0xd2,0xc3};
	//bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)testData, 100, &byWrite);
	//int eRoCode = GetLastError();
	//std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
	//HANDLE hRemoteThread1 = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, 0);

	bool bRet = WriteProcessMemory(hProcess, callBase, WorkCallArg, nThreadSize, &byWrite);
	if (bRet == false)
	{
		int eRoCode = GetLastError();
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
		VirtualFreeEx(hProcess, callArg, 0x8FFF, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
	bRet = WriteProcessMemory(hProcess, callArg, &nIndex, sizeof(int), &byWrite);
	if (bRet == false)
	{
		std::cout << "erorCode:"<<GetLastError() << "提示：写入参数失败\n";
		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
		VirtualFreeEx(hProcess, callArg, sizeof(int), MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, callArg, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		printf( "提示：远程调用代码失败\n");
		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
		VirtualFreeEx(hProcess, callArg, sizeof(DWORD), MEM_DECOMMIT);
		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
		return;
	}
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
	VirtualFreeEx(hProcess, callArg, sizeof(DWORD), MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	std::cout << "Work Success !\n";

}

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};
BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK  EnumWindowsCallback(HWND handle, LPARAM lParam)
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

HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}
void SendString(DWORD hProcessID)
{
	HWND gameHwnd = FindMainWindow(hProcessID);
	SendMessage(gameHwnd, WM_CHAR, 48, 0);
	SendMessage(gameHwnd, WM_CHAR, 13, 0);
	//	keybd_event(13,0,0,0);
}
static POINT g_defResumeHMP = { 302, 174 };	//恢复hp mp
static POINT g_defResumePetHMP = { 285, 252 };	//恢复宠物hp mp
static POINT g_defYes = { 250, 315 };	//是
static POINT g_defNo = { 383, 315 };	//否
static POINT g_defOk = { 313, 319 };	//确定
void selectRenew(DWORD processID)
{
	HWND gameHwnd = FindMainWindow(processID);
	POINT points;
	points.x = g_defResumeHMP.x;
	points.y = g_defResumeHMP.y;
	//ClientToScreen(m_gameHwnd, &points);
		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
	LPARAM newl = MAKELPARAM(points.x, points.y);
	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(1500);//GetDoubleClickTime());
	points.x = g_defYes.x;
	points.y = g_defYes.y;
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
	Sleep(1500);//GetDoubleClickTime());
	points.x = g_defOk.x;
	points.y = g_defOk.y;
	newl = MAKELPARAM(points.x, points.y);
	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
}
void __declspec(naked) SelectBuMpAndHp()
{
	__asm
	{
		mov ecx, 0xA0000000
		push ecx
		push 0
		push 0
		push 0
		mov edx, 0x00479900
		call edx
		add esp, 0x10
		ret
	}
}
void __declspec(naked) SelectYes()
{
	__asm
	{
		push 0
		push 0
		push -1
		push 4
		mov edx, 0x00479900
		call edx
		add esp, 0x10
		ret
	}
}
void remoteCall(DWORD processID,void* pCall)
{
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	DWORD nThreadSize = 0x400;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		std::cout << "erorCode:" << GetLastError() << ("提示：申请空间失败\n");
		return;
	}
	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
	DWORD byWrite;
	bool bRet = WriteProcessMemory(hProcess, callBase, pCall, nThreadSize, &byWrite);
	if (bRet == false)
	{
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}	
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		printf("提示：远程调用代码失败\n");
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
		return;
	}
	//WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	//VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	//CloseHandle(hRemoteThread);
	//CloseHandle(hProcess);
	std::cout << "remoteCall Success !\n";
}
void selectMpAndHp(DWORD processID)
{
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	DWORD nThreadSize = 0x8FFF;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{ 
		std::cout << " selectMpAndHp erorCode:" << GetLastError() << "提示：写入代码失败\n";
		return;
	}
	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
	DWORD byWrite;
	bool bRet = WriteProcessMemory(hProcess, callBase, SelectBuMpAndHp, nThreadSize, &byWrite);
	if (bRet == false)
	{
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		printf("提示：远程调用代码失败\n");
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
		return;
	}
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	std::cout << "remoteCall Success !\n";
}

void selectDlgYes(DWORD processID)
{
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	DWORD nThreadSize = 0x400;
	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
	if (callBase == nullptr)
	{
		std::cout << " selectDlgYes erorCode:" << GetLastError() << "提示：写入代码失败\n";
		return;
	}
	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
	DWORD byWrite;
	bool bRet = WriteProcessMemory(hProcess, callBase, SelectYes, nThreadSize, &byWrite);
	if (bRet == false)
	{
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hProcess);
		return;
	}
	DWORD tId;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
	if (hRemoteThread == nullptr)
	{
		printf("提示：远程调用代码失败\n");
		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
		return;
	}
	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	std::cout << "remoteCall Success !\n";
}
bool renew(DWORD processID, int direction)
{
	return true;
	SendString(processID);
//	turn_about(processID,direction);
	int nHp = getPersionHP(processID);
	int nMp = getPersionMP(processID);
//	Sleep(1500);
//	selectMpAndHp(processID);
////	remoteCall(processID, SelectBuMpAndHp);//选补血魔
//	Sleep(1500);
//	selectDlgYes(processID);
//	remoteCall(processID, SelectYes);//选是
//	selectRenew(processID);
	int nRenewHp = getPersionHP(processID);
	int nRenewMp = getPersionMP(processID);
	if (nHp == nRenewHp && nMp == nRenewMp)
	{
		renew(processID, direction);//递归
	}
	std::cout << "renew success \n";
	return true;
}
#define RNEW_MP 100000
void StartWork(DWORD processID)
{
	int nMp = getPersionMP(processID);
	std::cout << "MP" << nMp << " \n";
	if (nMp < RNEW_MP)					// 魔无回城
	{
		POINT characterPoint = GetGamePersonCoordinate(processID);
		//2个位置  在挂机点，在医院  
		 if (characterPoint.x == 221 && characterPoint.y == 84)
		{
			 high(processID, 221, 83,12,42);
			 nowhile(processID, ("城东医院"));
			 moveto(processID, 12, 35);
			 moveto(processID, 6, 35);
			 moveto(processID, 6, 34);
			 Sleep(2000);
//			 turn_about(processID, 0);// 
//			 Sleep(1000);
			 renew(processID,0);			// 恢复人宠
			 Sleep(1000);
			 moveto(processID, 6, 35);
			 moveto(processID, 12, 35);
			 moveto(processID, 12, 41);
			 high(processID, 12, 42,221,83);
			 nowhile(processID, ("法兰城"));
			 moveto(processID, 221, 84);
			 StartWork(processID);
			 Sleep(1000);
		}
	}
	else
	{
		std::cout << "begin Work" << nMp << " \n";
		/*
		Work(processID,102);
		Sleep(1000);*/
	}		
}

//判断坐标 
int main()
{
    std::cout << "Hello World!\n";
	//1、隔一段时间获取所有进程 
	while (true)
	{
		g_mlProcess.clear();
		GetAllProcess(g_mlProcess);
		std::cout << "进程列表" <<  g_mlProcess.size() << "\n";
		//2、遍历打开进程，查看人物信息是否满足条件 不满足进行补魔处理 满足跳过
		for (auto it = g_mlProcess.begin(); it != g_mlProcess.end(); ++it)
		{
			DWORD processID = it->first;
//			remoteCall(processID, WorkCall);
			Work(processID, 0);
			remoteCall(processID, SelectBuMpAndHp);//选补血魔
			Sleep(1500);
			remoteCall(processID, SelectYes);//选是
			Work(processID,0);
			selectMpAndHp(processID);
			std::cout << processID << "\n";
			//判断当前坐标
			POINT characterPoint = GetGamePersonCoordinate(processID);
			string mapName = GetGameMapName(processID);
			std::cout << processID <<" "<< characterPoint.x << " " << characterPoint.y << " " << mapName.c_str()<< "\n";
			//2个位置  在挂机点，在医院  
			if (characterPoint.x == 226 && characterPoint.y == 101)	//挂机点 
			{
				moveto(processID, 226, 89);
				moveto(processID, 221, 84);
				StartWork(processID);
			}
			else if (characterPoint.x == 221 && characterPoint.y == 84)
			{
				StartWork(processID);
			}
			else if (characterPoint.x == 6 && characterPoint.y == 34)
			{
				Sleep(3000);
				renew(processID, 0);			// 恢复人宠
				Sleep(1000);
				moveto(processID, 6, 35);
				moveto(processID, 12, 35);
				moveto(processID, 12, 41);
				high(processID, 12, 42, 221, 83);
				nowhile(processID, ("法兰城"));
				moveto(processID, 221, 84);
				StartWork(processID);
				Sleep(1000);
			}
			else if (mapName.find(("城东医院")) != string::npos)//地图名称在医院  
			{
				moveto(processID, 6, 35);
				moveto(processID, 12, 35);
				moveto(processID, 12, 41);
				moveto(processID, 12, 42);
				nowhile(processID, ("法兰城"));
				moveto(processID, 221, 84);
				StartWork(processID);
			}
			else if (mapName.find(("法兰城")) != string::npos)	//去挂机点 上面判断过是否在挂机点 
			{
				moveto(processID, 226, 89);
				moveto(processID, 221, 84);
				StartWork(processID);
			}
		}
		Sleep(3600000);//3600秒 1个小时
	}	
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
