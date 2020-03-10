#include "stdafx.h"
#include "WPGTool.h"
#include <tlhelp32.h>
#include "ITPublic.h"
#include <QTextCodec>
#include <psapi.h> 
#include <QComboBox>
#include "QxtGlobalShortcut.h"
#include "YunLai.h"
static HHOOK hookid = 0;
static HHOOK g_hHook = 0;
static HDC m_gameHDC = 0;

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")
#include <..\include\easyHook\easyhook.h>
#if _WIN64
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook64.lib")
#else
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook32.lib")
#endif


static QList<HWND> m_pHwndList;
static HWND g_MainHwnd=nullptr;
static ulong m_mainPID = 0;

WPGTool::WPGTool(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_KeyStart = 2;
	m_KeyStop = 3;
	m_mainPID = 0;
	initMode();
	initHotKey();
	ui.label->installEventFilter(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setColumnCount(3);
	//	ui.tableWidget->setHorizontalHeaderLabels(QStringList() <<"进程" << "父进程ID" << "进程ID" << "线程数" << "类");
	ui.tableWidget->setHorizontalHeaderLabels(QStringList() <<"进程名" << "进程ID" <<"父进程ID" << "线程数" << "线程优先级");
	ui.pushButton->hide();
//	GetProcessList();
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
//	initHook();
}

WPGTool::~WPGTool()
{
	bool bOk = ::UnhookWindowsHookEx(hookid);
	hookid = NULL;
	DWORD   dwCurID;
	dwCurID = ::GetWindowThreadProcessId(g_MainHwnd, NULL);
	for (int i = 0; i < m_pHwndList.size(); ++i)
	{
		DWORD   dwThreadID = GetWindowThreadProcessId((HWND)m_pHwndList.at(i), NULL);
		::AttachThreadInput(dwCurID, dwThreadID, FALSE);
	}
}

void WPGTool::initMode()
{
	ui.comboBox_mode->addItem("模式1");
	ui.comboBox_mode->addItem("模式2");

}

#include <iostream>
using namespace std;
BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration);
BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration)
{
	cout << "\n****All your beeps belong to us!\n\n";
	return Beep(dwFreq + 800, dwDuration);
}
void WPGTool::initHook()
{
	HOOK_TRACE_INFO hHook = { NULL };
	GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep");
	// Install the hook
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep"),
		myBeepHook,
		NULL,
		&hHook);
	if (FAILED(result))
	{
		wstring s(RtlGetLastErrorString());
		wcout << "Failed to install hook: ";
		wcout << s;
		cout << "\n\nPress any key to exit.";
		cin.get();
		return ;
	}

	cout << "Beep after hook installed but not enabled.\n";
	Beep(500, 500);

	cout << "Activating hook for current thread only.\n";
	// If the threadId in the ACL is set to 0, 
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &hHook);

	cout << "Beep after hook enabled.\n";
	Beep(500, 500);

	cout << "Uninstall hook\n";
	LhUninstallHook(&hHook);

	cout << "Beep after hook uninstalled\n";
	Beep(500, 500);

	cout << "\n\nRestore ALL entry points of pending removals issued by LhUninstallHook()\n";
	LhWaitForPendingRemovals();

	cout << "Press any key to exit.";
	cin.get();
}

void WPGTool::initHotKey()
{
	for (int i = 0; i < 11; ++i)
	{
		QString szItem = QString("F%1").arg(i+1);
		ui.comboBox->addItem(szItem,i);
		ui.comboBox_2->addItem(szItem,i);
	}
	ui.comboBox->setCurrentIndex(m_KeyStart);
	ui.comboBox_2->setCurrentIndex(m_KeyStop);
	ui.comboBox->removeItem(m_KeyStop);
	ui.comboBox_2->removeItem(m_KeyStart);
	/*if (RegisterHotKey((HWND)this->winId(), 0, 0, VK_F1 + m_KeyStart) == FALSE)
	{
		QMessageBox::information(this,"提示","注册启动热键失败",QMessageBox::Ok);
		return;
	}

	if (RegisterHotKey((HWND)this->winId(), 1, 0, VK_F1 + m_KeyStop) == FALSE)
	{
		QMessageBox::information(this, "提示", "注册停止热键失败", QMessageBox::Ok);
		return;
	}*/
	QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
	if (shortcut)
	{
		if (shortcut->setShortcut(QKeySequence(ui.comboBox->currentText()),(HWND) NULL) == FALSE)
		{
			QMessageBox::information(this, "提示", "注册启动热键失败", QMessageBox::Ok);
		}
		//connect(shortcut, SIGNAL(activated(int, int)), this, SIGNAL(signal_keyReleaseEvent(int, int)));
		connect(shortcut, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	}
	QxtGlobalShortcut *shortcut2 = new QxtGlobalShortcut(this);
	if (shortcut2)
	{
		if (shortcut2->setShortcut(QKeySequence(ui.comboBox_2->currentText()), (HWND)NULL) == FALSE)
		{
			QMessageBox::information(this, "提示", "注册停止热键失败", QMessageBox::Ok);
		}
		//connect(shortcut, SIGNAL(activated(int, int)), this, SIGNAL(signal_keyReleaseEvent(int, int)));
		connect(shortcut2, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	}

}

bool WPGTool::GetProcessList()

{

	HANDLE             hProcessSnap;
	HANDLE             hProcess;
	PROCESSENTRY32     pe32;
	//对系统中当前所有的进程拍下快照 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//在使用 PROCESSENTRY32 结构之间需要先设置好该结构的大小 
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//获取第一个进程 
	if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	//采用 Do - While 遍历所有进程 
	int rowCount=0;
	do
	{
		rowCount = ui.tableWidget->rowCount();		
		printf("\n-----------------------------------------------------");
		printf("\n  PROCESS NAME:     = %s", pe32.szExeFile);	
		ui.tableWidget->setItem(rowCount-1,0,new QTableWidgetItem(QString::fromStdWString(pe32.szExeFile)));

		printf("\n  process ID        = 0x%08X", pe32.th32ProcessID);
		ui.tableWidget->setItem(rowCount-1,1,new QTableWidgetItem(QString::number(pe32.th32ProcessID)));

		printf("\n  parent process ID = 0x%08X", pe32.th32ParentProcessID);
		ui.tableWidget->setItem(rowCount-1,2,new QTableWidgetItem(QString::number(pe32.th32ParentProcessID)));		
		printf("\n  thread count      = %d", pe32.cntThreads);
		ui.tableWidget->setItem(rowCount-1,3,new QTableWidgetItem(QString::number(pe32.cntThreads)));
		printf("\n  Priority Base     = %d", pe32.pcPriClassBase);
		ui.tableWidget->setItem(rowCount-1,4,new QTableWidgetItem(QString::number(pe32.pcPriClassBase)));
		//遍历获取下一个进程 
		rowCount++;
		ui.tableWidget->setRowCount(rowCount);	
	} while(Process32Next(hProcessSnap, &pe32));
	ui.tableWidget->setRowCount(rowCount-1);	
	CloseHandle(hProcessSnap);
	return TRUE;
}

bool WPGTool::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == ui.label)
	{
		if(event->type() == QEvent::MouseButtonPress)
		{
			QApplication::setOverrideCursor(QCursor(QPixmap(":/WPGTool/Resources/eye.ico")));    //将鼠标指针更改为自己设置的图片
			ui.label->setPixmap(QPixmap(":/WPGTool/Resources/drag2.ico"));			
		}else if(event->type() == QEvent::MouseButtonRelease)
		{
			QApplication::restoreOverrideCursor();
			ui.label->setPixmap(QPixmap(":/WPGTool/Resources/drag.ico"));			
			POINT pt;
			GetCursorPos(&pt);
			HWND g_hWnd=::WindowFromPoint(pt);
			HWND tempHandle;
			char tempstr[255]="\0";
			tempHandle = g_hWnd;   //本窗口句柄				
			int rowCount = ui.tableWidget->rowCount();
			ui.tableWidget->setRowCount(rowCount+1);			
			DWORD thread,process;
			thread=GetWindowThreadProcessId(tempHandle,&process);
			ui.tableWidget->setItem(rowCount,1,new QTableWidgetItem(QString::number(process)));		
			memset(tempstr,0,255);
			HANDLE processHWND = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, process);   
			QString string ;
			wchar_t szExePath[MAX_PATH] = L"";				
			GetModuleBaseNameW(processHWND, 0, szExePath, MAX_PATH);
			string = QString::fromWCharArray(szExePath);
			SendMessage(tempHandle, WM_GETTEXT, 255, (LPARAM)(tempstr));				
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			QTableWidgetItem* pNewItem=new QTableWidgetItem(string);
			pNewItem->setData(Qt::UserRole,(int) tempHandle);
			ui.tableWidget->setItem(rowCount,0,pNewItem);	
			ui.tableWidget->setCurrentItem(pNewItem);
			if (ui.tableWidget->rowCount() > 1)
			{
				m_pHwndList.append(tempHandle);
			}
			else if (ui.tableWidget->rowCount() == 1)
			{
				g_MainHwnd = tempHandle;
				m_mainPID = process;
			}
		}
	}
	return QWidget::eventFilter(obj,event);
}

// Hook callback   钩子回调函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)//if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	LPKBDLLHOOKSTRUCT pKeyBoardHook = (KBDLLHOOKSTRUCT  FAR*)lParam;
	POINT pi;
	::GetCursorPos(&pi);
	HWND curHwnd = WindowFromPoint(pi);
	if (curHwnd != g_MainHwnd)
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	if (wParam >= WM_KEYDOWN && wParam <= WM_MBUTTONDBLCLK)
	{
	}
	for (int i = 0; i < m_pHwndList.size(); i++)
	{
		HWND hwnduse = (HWND)m_pHwndList.at(i);
		::SendMessageA(hwnduse, WM_KEYDOWN, wParam, lParam);//
	//	qDebug() << msg->hwnd << "DstHwnd" << hwnduse;// << "记录iD" << m_mainPID;
//			::SendMessageA(hwnduse, _dwCode, _wParam, _lParam);
//			::PostMessageA(hwnduse, msg->message, msg->paramL, _lParam);
//			::PostMessage(hwnduse, WM_KEYDOWN, 0x50, NULL);
	}
	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

LRESULT WPGTool::MouseHookProc(DWORD _dwCode, DWORD _wParam, DWORD _lParam)
{
	if (_dwCode != HC_ACTION)
		return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
	LPMSLLHOOKSTRUCT pMouseHook = (MSLLHOOKSTRUCT FAR*)_lParam;
	POINT pi;
	::GetCursorPos(&pi);
	HWND curHwnd = WindowFromPoint(pi);
	if (curHwnd != g_MainHwnd)
	{
		return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
	}
	if (_wParam >= WM_MOUSEMOVE && _wParam <= WM_MBUTTONDBLCLK)
	{
		::ScreenToClient(g_MainHwnd, &pi);
		MSLLHOOKSTRUCT *pkbhs = (MSLLHOOKSTRUCT *)_lParam;
		LPARAM newl = MAKELPARAM(pi.x, pi.y);	
		for (int i = 0; i < m_pHwndList.size(); i++)
		{
			HWND hwnduse = (HWND)m_pHwndList.at(i);
			::PostMessageA(hwnduse, _wParam, _wParam, newl);
		}
	}
	return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
}
LRESULT WPGTool::CallBackProc(DWORD _dwCode,DWORD _wParam,DWORD _lParam)
{
	if (_dwCode != HC_ACTION)
		return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
	//准确来说 要用另一个结构体 
	LPMSLLHOOKSTRUCT pMouseHook = (MSLLHOOKSTRUCT FAR*)_lParam;
	POINT pi;
	::GetCursorPos(&pi);
	HWND curHwnd = WindowFromPoint(pi);
	if (curHwnd != g_MainHwnd)
	{
		return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);	
	}
	//qDebug() << msg->message  << pmsg->message << cwpStru->message << pMouseHook->hwnd << _wParam;
//	qDebug() << pMouseHook->mouseData << g_MainHwnd << pMouseHook->pt.x << pMouseHook->pt.y << pMouseHook->dwExtraInfo;// << msg->hwnd << pmsg->hwnd << cwpStru->hwnd << _dwCode << _lParam<< pMouseHook->wHitTestCode;
	if (_wParam>= WM_MOUSEMOVE && _wParam <= WM_MBUTTONDBLCLK)
	{		
		::ScreenToClient(g_MainHwnd,&pi);		
		MSLLHOOKSTRUCT *pkbhs = (MSLLHOOKSTRUCT *)_lParam;
		LPARAM newl = MAKELPARAM(pi.x, pi.y); 
		//qDebug() << pi.x << pi.y << LOWORD(_lParam) << HIWORD(_lParam);
		//点击单独处理
		//COLORREF tempc = GetPixel(m_gameHDC, pi.x, pi.y);// 486, 130);
		//qDebug() << tempc;
		//if (tempc == 7243681)
		//{
		//	qDebug() << pi.x << pi.y;
		//}
		//if (_wParam == WM_RBUTTONDOWN)
		//	qDebug() << pi.x << pi.y << LOWORD(_lParam) << HIWORD(_lParam);
		if (_wParam == WM_LBUTTONDOWN)
		{
			for (int i = 0; i < m_pHwndList.size(); i++)
			{
				HWND hwnduse = (HWND)m_pHwndList.at(i);		
				YunLai::SetFocusToWnd(hwnduse);
				::PostMessageA(hwnduse, _wParam, _wParam, newl);				
			}
		}
		else
		{
			for (int i = 0; i < m_pHwndList.size(); i++)
			{
				HWND hwnduse = (HWND)m_pHwndList.at(i);
				::PostMessageA(hwnduse, _wParam, _wParam, newl);
			}
		}		
	}
	return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);	
}
void WPGTool::doBtnClicked()
{
	QObject* obj=sender();
	if(obj == ui.pushButton)
	{

	}else if(obj == ui.pushButton_2)
	{
		clearTableItems();
	}else if(obj == ui.pushButton_3)
	{
		clearTableItems();
		GetProcessList();
	}
}

void WPGTool::dealHotKeyEvent(const QKeySequence& key)
{
	if (key == QKeySequence(ui.comboBox->currentText()))
	{
		//开始同步
		startSync();
	}
	else if (key == QKeySequence(ui.comboBox_2->currentText()))
	{
		//停止同步
		stopSync();
	}
}

//************************************/
//函数名:  clearTableItems
//描述：清除表格，清除关联关系
//参数：无
//返回值：bool
//时间：2016/7/22 WZQ
//************************************/
void WPGTool::clearTableItems()
{
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol=-1;
	for(int i=0;i<rowCounts-1;i++)
	{
		for(int j=0;j<colCounts;j++)
		{
			QTableWidgetItem* tableitem = ui.tableWidget->item(i,j);
			if(tableitem!=NULL)
				delete tableitem;
		}
		if(nWidgetCol >= 0)
		{
			QWidget* tempWidget =  ui.tableWidget->cellWidget(i,nWidgetCol);//把checkBox值取出来  去释放
			if(tempWidget!=NULL)
				delete tempWidget;
		}
		ui.tableWidget->removeRow(i);               
	}
	ui.tableWidget->clearContents();	
	ui.tableWidget->setRowCount(0);
	m_pHwndList.clear();
}

void WPGTool::startSync()
{
	int nLastError=0;
	if (hookid == 0)
	{
		ui.pushButton->setText("停止同步");
		HMODULE curModle = NULL;
		curModle = GetModuleHandle(NULL);
		//取第一个窗口句柄
	/*	QTableWidgetItem* pItem = ui.tableWidget->item(0, 0);
		if (pItem)
			curModle = (HMODULE)pItem->data(Qt::UserRole).toInt();*/
	//	DWORD   dwThreadID = GetWindowThreadProcessId((HWND)curModle, NULL);
		if (ui.comboBox_mode->currentIndex() == 0)
		{
			hookid = ::SetWindowsHookEx(WH_MOUSE_LL/*WH_JOURNALRECORD*/, HOOKPROC(&MouseHookProc), (HMODULE)curModle, 0);
		}
		else if (ui.comboBox_mode->currentIndex() == 1)
		{
			hookid = ::SetWindowsHookEx(WH_MOUSE_LL/*WH_JOURNALRECORD*/, HOOKPROC(&CallBackProc), (HMODULE)curModle, 0);
		}
		nLastError =  ::GetLastError ();	
		m_gameHDC = GetDC(g_MainHwnd);
		//g_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, (HMODULE)curModle, 0);
	/*	if (hookid != 0)
		{
			HWND hCurWnd = NULL;
			DWORD dwMyID;
			DWORD   dwCurID;
			dwCurID = ::GetWindowThreadProcessId(g_MainHwnd, NULL);
			
			for (int i=0;i<m_pHwndList.size();++i)
			{
				DWORD   dwThreadID = GetWindowThreadProcessId((HWND)m_pHwndList.at(i), NULL);
				bool bret=::AttachThreadInput(dwCurID, dwThreadID, TRUE);
				qDebug() << bret << GetLastError();
			}
		}	*/	
	}	
}

void WPGTool::stopSync()
{
	ui.pushButton->setText("开始同步");
	UnhookWindowsHookEx(hookid);
	hookid = 0;
	DWORD   dwCurID;
	dwCurID = ::GetWindowThreadProcessId(g_MainHwnd, NULL);
	for (int i = 0; i < m_pHwndList.size(); ++i)
	{
		DWORD   dwThreadID = GetWindowThreadProcessId((HWND)m_pHwndList.at(i), NULL);
		::AttachThreadInput(dwCurID, dwThreadID, FALSE);
	}
}

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

BOOL WPGTool::IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}


HWND WPGTool::FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

BOOL CALLBACK WPGTool::EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) 
	{
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}