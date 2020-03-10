/****************************** Module Header ******************************\
* Module Name:	HookDll.cpp
* Project:		CppHookDll
* Copyright (c) Microsoft Corporation.
* 
* Defines the exported hook procedure.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/01/2009 11:04 PM RongChun Zhang Created
\***************************************************************************/


#include <windows.h>

#define HOOKDLL_EXPORTS
#include "HookDll.h"


// Shared data among all instances.在所有实例共享数据。
#pragma data_seg(".HOOKDATA")
HWND g_hWnd = NULL;	        // Window handle   窗口句柄
HHOOK g_hHook = NULL;		// Hook handle	  钩子处理

// Get module from address	从地址获取模块
HMODULE WINAPI ModuleFromAddress(PVOID pv) 
{
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}

// Hook callback   钩子回调函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
    if (lParam & 0x40000000)	// Check the previous key state
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	
	// Post private messages to Main window
	// wParam specifies the virtual key code
	// lParam specifies the key data
	//Post   私人信息的主要窗口
	//wParam 指定虚拟键的代码
	//lParam 指定的关键数据
    //::PostMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
	::SendMessage(g_hWnd, WM_KEYSTROKE, wParam, lParam);
    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Install or uninstall the hook  安装或卸载钩子
BOOL WINAPI SetKeyboardHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;
	
	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardHookProc, 
			ModuleFromAddress(KeyboardHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}


// Hook callback	  低级勾子回调函数
LRESULT CALLBACK LowKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
    if (lParam & 0x40000000)	// Check the previous key state
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT  *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

	//check that the message is from keyboard or is synthesized by SendInput API
	if((pkbhs->flags & LLKHF_INJECTED))
        return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if(wParam == WM_KEYDOWN)
	{
		// Post private messages to Main window
		// wParam specifies the virtual key code
		// lParam specifies the scan code
		//Post   私人信息的主要窗口
		//wParam 指定虚拟键的代码
		//lParam 指定的关键数据
		::PostMessage(g_hWnd, WM_KEYINPUT, pkbhs->vkCode, pkbhs->scanCode);
	}
	
    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
  //安装低级勾子
BOOL WINAPI SetLowKeyboardHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWnd = hWndCaller;
	
	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowKeyboardHookProc, 
			ModuleFromAddress(LowKeyboardHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}

// Hook callback	  低级鼠标回调函数
LRESULT CALLBACK LowMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if (lParam & 0x40000000)	// Check the previous key state
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT  *pkbhs = (KBDLLHOOKSTRUCT *)lParam;

	//check that the message is from keyboard or is synthesized by SendInput API
	if((pkbhs->flags & LLKHF_INJECTED))
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	if(wParam == WM_KEYDOWN)
	{
		// Post private messages to Main window
		// wParam specifies the virtual key code
		// lParam specifies the scan code
		//Post   私人信息的主要窗口
		//wParam 指定虚拟键的代码
		//lParam 指定的关键数据
		::PostMessage(g_hWnd, WM_KEYINPUT, pkbhs->vkCode, pkbhs->scanCode);
	}

	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
//安装低级鼠标勾子
 BOOL WINAPI SetMouseHook(BOOL bInstall,DWORD dwThreadId,HWND hWndCaller)
 {
	 BOOL bOk;
	 g_hWnd = hWndCaller;

	 if (bInstall)
	 {
		 g_hHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowMouseHookProc, 
			 ModuleFromAddress(LowMouseHookProc), dwThreadId);
		 bOk = (g_hHook != NULL);
	 }
	 else 
	 {
		 bOk = ::UnhookWindowsHookEx(g_hHook);
		 g_hHook = NULL;
	 }

	 return bOk;

 }

