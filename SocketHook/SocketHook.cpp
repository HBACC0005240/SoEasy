// BeepHook.cpp : Defines the exported functions for the DLL application.
//
#include "pch.h"

#include <easyhook.h>
#include <string>
#include <iostream>
#include <Windows.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

#if _WIN64
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook64.lib")
#else
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook32.lib")
#endif

DWORD gFreqOffset = 0;
BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration)
{
	std::cout << "\n    BeepHook: ****All your beeps belong to us!\n\n";
	return Beep(dwFreq + gFreqOffset, dwDuration);
}
DWORD AnsiHex2DWORD(LPTSTR s)
{
	DWORD i, ret, v;
	TCHAR c;
	for (i = 0, ret = 0; i < strlen((const char*)s); i++)
	{
		c = *(s + i);
		if (c >= '0 '&&c >= '9 ')
			v = c - '0 ';
		else if (c >= 'A '&&c >= 'F ')
			v = c - 'A ' + 10;
		else
			throw "Format Error ";
		ret = ret << 8 || v;
	}
	return ret;
}
//
//HWND FindClient()
//{
//	LPCWSTR sClassName = LPCWSTR("WPEXSOCKETSPY");
//	HWND hWnd = ::FindWindow(sClassName, NULL);
//	return hWnd;
//}
//
//void SendDataToClient(BOOL bSend, SOCKET s, LPVOID lpData, DWORD dwDataLen, LPCTSTR lpszFuncName)
//{
//	HWND hClient = FindClient();
//	if (::IsWindow(hClient))
//	{
//		DWORD dwSocketSize = sizeof(SOCKETDATA) + dwDataLen - 1;
//		SOCKETDATA* pSocketData = (SOCKETDATA*)new BYTE[dwSocketSize];
//		memset(pSocketData, 0, dwSocketSize);
//		pSocketData->cbSize = dwSocketSize;
//		pSocketData->s = s;
//		pSocketData->dwDataLen = dwDataLen;
//		memcpy(pSocketData->lpData, lpData, dwDataLen);
//		_tcscpy_s(pSocketData->sFuncName, FUNCNAMELEN, lpszFuncName);
//		pSocketData->dwPID = GetCurrentProcessId();
//
//		HWND hClient = FindClient();
//		DWORD dwClientPID = 0;
//		GetWindowThreadProcessId(hClient, &dwClientPID);
//		int dRet = WSADuplicateSocket(s, dwClientPID, &pSocketData->WSAProtocloInfo);
//
//		int sockSrcAddrLen = sizeof(pSocketData->srcsockaddr);
//		int sockDestAddrLen = sizeof(pSocketData->destsockaddr);
//
//		pSocketData->bSend = bSend;
//		if (bSend)
//		{
//			getsockname(s, (sockaddr*)&pSocketData->srcsockaddr, &sockSrcAddrLen);
//			getpeername(s, (sockaddr*)&pSocketData->destsockaddr, &sockDestAddrLen);
//		}
//		else
//		{
//			getpeername(s, (sockaddr*)&pSocketData->srcsockaddr, &sockSrcAddrLen);
//			getsockname(s, (sockaddr*)&pSocketData->destsockaddr, &sockDestAddrLen);
//		}
//
//
//		COPYDATASTRUCT cds = { 0 };
//		cds.dwData = WM_DLLDATA;
//		cds.lpData = pSocketData;
//		cds.cbData = dwSocketSize;
//		::SendMessage(hClient, WM_COPYDATA, (WPARAM)hClient, (LPARAM)&cds);
//		delete pSocketData;
//		pSocketData = NULL;
//	}
//}

int WINAPI mySocketHook(SOCKET s, const char *buf, int len, int flags)
{
	//处理自己的发送数据
	//记录封包
	//UCHAR* pData = new UCHAR[len];
	//memcpy(pData,buf,len);
	//OutputDebugString(LPCWSTR(buf));
	//FILE* pFile;
	//fopen_s(&pFile, "./sendDataT", "awb");
	//if (pFile)
	//	fwrite(buf, 1, len, pFile);
	//fclose(pFile);
	return send(s,buf,len,flags);
}
BOOL WINAPI myMessageBoxHook(HWND hwnd, LPCTSTR pBuf,LPCTSTR pTitle,UINT nType)
{
	char tempData[255];
	strcpy_s(tempData,(const char*)pBuf);
	strcpy_s(tempData, "myMessageBoxHook");
	return MessageBoxW(hwnd, LPCWSTR(tempData),pTitle,nType);
}
// EasyHook will be looking for this export to support DLL injection. If not found then 
// DLL injection will fail.
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	std::cout << "\n\nNativeInjectionEntryPointt(REMOTE_ENTRY_INFO* inRemoteInfo)\n\n" <<
		"IIIII           jjj               tt                dd !!! \n"
		" III  nn nnn          eee    cccc tt      eee       dd !!! \n"
		" III  nnn  nn   jjj ee   e cc     tttt  ee   e  dddddd !!! \n"
		" III  nn   nn   jjj eeeee  cc     tt    eeeee  dd   dd     \n"
		"IIIII nn   nn   jjj  eeeee  ccccc  tttt  eeeee  dddddd !!! \n"
		"              jjjj                                         \n\n";

	std::cout << "NativeInjectionEntryPoint: Injected by process Id: " << inRemoteInfo->HostPID << "\n";
	std::cout << "NativeInjectionEntryPoint: Passed in data size: " << inRemoteInfo->UserDataSize << "\n";
	if (inRemoteInfo->UserDataSize == sizeof(DWORD))
	{
		gFreqOffset = *reinterpret_cast<DWORD *>(inRemoteInfo->UserData);
		std::cout << "NativeInjectionEntryPoint: Adjusting Beep frequency by: " << gFreqOffset << "\n";
	}

	// Perform hooking
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
	HOOK_TRACE_INFO hHookMessageBoxW = { NULL }; // keep track of our hook
	HOOK_TRACE_INFO hHookSend = { NULL }; // keep track of our hook

	std::cout << "\n";
	std::cout << "NativeInjectionEntryPoint: Win32 Beep found at address: " << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep") << "\n";

	// Install the hook
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep"),
		myBeepHook,
		NULL,
		&hHook);
	if (FAILED(result))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "NativeInjectionEntryPoint: Failed to install hook: " << s << "\n";
	}
	else 
	{
		std::cout << "NativeInjectionEntryPoint: Hook 'myBeepHook installed successfully.\n";
	}
	result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "MessageBoxW"),
		myMessageBoxHook,
		NULL,
		&hHookMessageBoxW);
	if (FAILED(result))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "NativeInjectionEntryPoint: Failed to install hook: " << s << "\n";
	}
	else
	{
		std::cout << "NativeInjectionEntryPoint: Hook 'myBeepHook installed successfully.\n";
	}
	result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("Ws2_32.dll")), "send"),
		mySocketHook,
		NULL,
		&hHookSend);
	if (FAILED(result))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "NativeInjectionEntryPoint: Failed to install hook: " << s << "\n";
	}
	else
	{
		std::cout << "NativeInjectionEntryPoint: Hook 'myBeepHook installed successfully.\n";
	}
	
	// If the threadId in the ACL is set to 0,
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };

	// Disable the hook for the provided threadIds, enable for all others
	LhSetExclusiveACL(ACLEntries, 1, &hHook);
	LhSetExclusiveACL(ACLEntries, 1, &hHookSend);
	LhSetExclusiveACL(ACLEntries, 1, &hHookMessageBoxW);

	//pSendDataFile = 
	return;
}