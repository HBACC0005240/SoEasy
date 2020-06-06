// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
#include <easyhook.h>
#include <map>
#include <tlhelp32.h>
#include <psapi.h> 
#include <list>
#include <tchar.h>
#include <string>
using namespace std;
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")
#if _WIN64
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook64.lib")
#else
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook32.lib")
#endif
std::map<DWORD, std::string> g_mlProcess;	//清风魔力的进程 定时轮询所有


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
		if (exeName.find("AsmManual.exe") != string::npos)//qfmoli
		{
			processInfo[pe32.th32ProcessID] = exeName;
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
}
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD processId;
	/*std::wcout << "Enter the target process Id: ";
	std::cin >> processId;*/
	HWND asmHwnd = FindWindow(NULL, "AsmManual");
	GetWindowThreadProcessId(asmHwnd,&processId);
	DWORD freqOffset = (DWORD)asmHwnd;
	//std::cout << "Enter a frequency offset in hertz (e.g. 800): ";
	//std::cin >> freqOffset;
	//GetAllProcess(g_mlProcess);
	//std::cout << "进程列表" << g_mlProcess.size() << "\n";
	////2、遍历打开进程，查看人物信息是否满足条件 不满足进行补魔处理 满足跳过
	//for (auto it = g_mlProcess.begin(); it != g_mlProcess.end(); ++it)
	//{

	//}
	WCHAR* dllToInject = L".\\MLHook.dll"; //L".\\SocketHook.dll";
	wprintf(L"Attempting to inject: %s\n\n", dllToInject);
	
	// Inject dllToInject into the target process Id, passing 
	// freqOffset as the pass through data.
	NTSTATUS nt = RhInjectLibrary(
		processId,   // The process to inject into
		0,           // ThreadId to wake up upon injection
		EASYHOOK_INJECT_DEFAULT,
		dllToInject, // 32-bit
		NULL,		 // 64-bit not provided
		&freqOffset, // data to send to injected DLL entry point
		sizeof(DWORD)// size of data to send
	);

	if (nt != 0)
	{
		printf("RhInjectLibrary failed with error code = %d\n", nt);
		PWCHAR err = RtlGetLastErrorString();
		std::wcout << err << "\n";
	}
	else 
	{
		std::wcout << L"Library injected successfully.\n";
	}

	std::wcout << "Press Enter to exit";
	std::wstring input;
	std::getline(std::wcin, input);
	std::getline(std::wcin, input);
	return 0;
}