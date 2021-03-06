/****************************** Module Header ******************************\
* Module Name:	CppWindowsHookDlg.cpp
* Project:		CppWindowsHook
* Copyright (c) Microsoft Corporation.
* 
* This project is the user interface of CppWindowsHook sample. It uses the 
* dialog based MFC application template and move the code for the about 
* dialog.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/01/2009 9:04 PM Rong-Chun Zhang Created
\***************************************************************************/

#include "stdafx.h"
#include "CppWindowsHook.h"
#include "CppWindowsHookDlg.h"

#include "../CppHookDll/HookDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HWND g_hwnd = nullptr;

// CCppWindowsHookDlg dialog

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
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


CCppWindowsHookDlg::CCppWindowsHookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCppWindowsHookDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_hwnd = (HWND)0x00010B68;// FindMainWindow(17064);

}
BEGIN_MESSAGE_MAP(CCppWindowsHookDlg, CDialog)
	ON_MESSAGE(WM_KEYSTROKE, OnHookKeyboard)
	ON_MESSAGE(WM_KEYINPUT, OnHookLowKeyboard)
	ON_BN_CLICKED(IDC_SETHOOK, &CCppWindowsHookDlg::OnBnClickedSethook)
	ON_BN_CLICKED(IDC_SETHOOKTHREAD, &CCppWindowsHookDlg::OnBnClickedSethookthread)
	ON_BN_CLICKED(IDC_SETHOOKINPUT, &CCppWindowsHookDlg::OnBnClickedSethookinput)
	ON_BN_CLICKED(IDC_RESETTEXT, &CCppWindowsHookDlg::OnBnClickedResettext)
END_MESSAGE_MAP()


// CCppWindowsHookDlg message handlers

BOOL CCppWindowsHookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;
}

// Hook handler
long CCppWindowsHookDlg::OnHookKeyboard(WPARAM wParam, LPARAM lParam)
{
	CString str;
	GetKeyNameText(lParam, str.GetBuffer(80), 80);
	str.ReleaseBuffer();

	CString strItem(L"Key:" + str + L"\r\n");

	// Add key data into the editing box
	CString strEdit;
	GetDlgItem(IDC_MSG)->GetWindowText(strEdit);
	GetDlgItem(IDC_MSG)->SetWindowText(strItem + strEdit);

	return 0;
}

// Hook handler(WH_KEYBOARD_LL)
// wParam specifies the virtual key code
// lParam specifies the scan code
long CCppWindowsHookDlg::OnHookLowKeyboard(WPARAM wParam, LPARAM lParam)
{	
	CString str;
	// Convert the virtual key code into a scancode (as required by GetKeyNameText).
	UINT scanCode = MapVirtualKeyEx(wParam, 0, GetKeyboardLayout(0));
    switch(wParam)
    {
		// Certain keys end up being mapped to the number pad by the above function,
		// as their virtual key can be generated by the number pad too.
		// If it's one of the known number-pad duplicates, set the extended bit:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_NEXT:  // Page down
		case VK_PRIOR: // Page up
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		  scanCode |= 0x100; // Add extended bit
		  break;
    }      

    // GetKeyNameText() expects the scan code to be on the same format as WM_KEYDOWN
    GetKeyNameText(scanCode << 16, str.GetBuffer(80), 80);
    str.ReleaseBuffer();

	CString strItem(L"Keyboard input:" + str + L"\r\n");

	// Add key data into the editing box
	CString strEdit;
	GetDlgItem(IDC_MSG)->GetWindowText(strEdit);
	GetDlgItem(IDC_MSG)->SetWindowText(strItem + strEdit);

	return 0;
}

// Install or uninstall global keyboard hook
//设置全局勾子
void CCppWindowsHookDlg::OnBnClickedSethook()
{
	CString strButtonText;
	GetDlgItem(IDC_SETHOOK)->GetWindowText(strButtonText);
	if(strButtonText == "设置全局勾子")
	{
		// Install Hook	 安装勾子
		if (!SetKeyboardHook(TRUE, 0, g_hwnd))
		{
			AfxMessageBox(L"Fail to Install Hook!");
			return;
		}
		GetDlgItem(IDC_SETHOOK)->SetWindowText(L"取消全局勾子");
		GetDlgItem(IDC_SETHOOKTHREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETHOOKINPUT)->EnableWindow(FALSE);
	}
	else
	{
		if (!SetKeyboardHook(FALSE))
		{
			AfxMessageBox(L"Fail to Install Hook!");
			return;
		}
        GetDlgItem(IDC_SETHOOK)->SetWindowText(L"设置全局勾子");
		GetDlgItem(IDC_SETHOOKTHREAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_SETHOOKINPUT)->EnableWindow(TRUE);
	}
}

// Install or uninstall thread keyboard hook
void CCppWindowsHookDlg::OnBnClickedSethookthread()
{
	CString strButtonText;
	GetDlgItem(IDC_SETHOOKTHREAD)->GetWindowText(strButtonText);
	if(strButtonText == _T("设置线程勾子"))
	{
		// Install Hook
		if(!SetKeyboardHook(TRUE, ::GetCurrentThreadId(), m_hWnd))
		{
			AfxMessageBox(_T("无法安装钩子!"));
			return;
		}
		GetDlgItem(IDC_SETHOOKTHREAD)->SetWindowText(_T("取消线程勾子"));
		GetDlgItem(IDC_SETHOOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETHOOKINPUT)->EnableWindow(FALSE);
	}
	else
	{
		if(!SetKeyboardHook(FALSE))
		{
			AfxMessageBox(_T("无法安装钩子!"));
			return;
		}
        GetDlgItem(IDC_SETHOOKTHREAD)->SetWindowText(_T("设置线程勾子"));
		GetDlgItem(IDC_SETHOOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_SETHOOKINPUT)->EnableWindow(TRUE);
	}
}

void CCppWindowsHookDlg::OnBnClickedSethookinput()
{
	CString strButtonText;
	GetDlgItem(IDC_SETHOOKINPUT)->GetWindowText(strButtonText);
	if(strButtonText == "设置钩子(输入)")
	{
		// Install Hook
		if (!SetLowKeyboardHook(TRUE, 0, m_hWnd))
		{
			AfxMessageBox(L"Fail to Install Hook!");
			return;
		}
		GetDlgItem(IDC_SETHOOKINPUT)->SetWindowText(L"取消输入勾子");
		GetDlgItem(IDC_SETHOOKTHREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETHOOK)->EnableWindow(FALSE);
	}
	else
	{
		if (!SetLowKeyboardHook(FALSE))
		{
			AfxMessageBox(L"Fail to Install Hook!");
			return;
		}
        GetDlgItem(IDC_SETHOOKINPUT)->SetWindowText(L"设置钩子(输入)");
		GetDlgItem(IDC_SETHOOKTHREAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_SETHOOK)->EnableWindow(TRUE);
	}
}

void CCppWindowsHookDlg::OnBnClickedResettext()
{
	GetDlgItem(IDC_MSG)->SetWindowText(NULL);
}
