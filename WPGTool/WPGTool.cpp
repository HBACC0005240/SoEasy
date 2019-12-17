#include "stdafx.h"
#include "WPGTool.h"
#include <tlhelp32.h>
#include "ITPublic.h"
#include <QTextCodec>
#include <psapi.h> 
#include <QComboBox>
static HHOOK hookid = 0;
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")

WPGTool::WPGTool(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.label->installEventFilter(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setColumnCount(3);
	//	ui.tableWidget->setHorizontalHeaderLabels(QStringList() <<"����" << "������ID" << "����ID" << "�߳���" << "��");
	ui.tableWidget->setHorizontalHeaderLabels(QStringList() <<"������" << "����ID" <<"������ID" << "�߳���" << "�߳����ȼ�");
	ui.tableWidget->setRowCount(1);
	ui.pushButton->hide();
	GetProcessList();
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(doBtnClicked()));

}

WPGTool::~WPGTool()
{

}


bool WPGTool::GetProcessList()

{

	HANDLE             hProcessSnap;
	HANDLE             hProcess;
	PROCESSENTRY32     pe32;
	//��ϵͳ�е�ǰ���еĽ������¿��� 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//��ʹ�� PROCESSENTRY32 �ṹ֮����Ҫ�����úøýṹ�Ĵ�С 
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//��ȡ��һ������ 
	if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	//���� Do - While �������н��� 
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
		//������ȡ��һ������ 
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
			QApplication::setOverrideCursor(QCursor(QPixmap(":/WPGTool/Resources/eye.ico")));    //�����ָ�����Ϊ�Լ����õ�ͼƬ
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
			tempHandle = g_hWnd;   //�����ھ��				
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
			ui.tableWidget->setItem(rowCount,0,pNewItem);	
			ui.tableWidget->setCurrentItem(pNewItem);
		}
	}
	return QDialog::eventFilter(obj,event);
}

LRESULT CALLBACK CallBackProc(DWORD _dwCode,DWORD _wParam,DWORD _lParam)
{

	CallNextHookEx(hookid,_dwCode,_wParam,_lParam);

	//	PEVENTMSG  msg = (PEVENTMSG )_lParam;
	// 		for (int i = 1;i < pList->GetItemCount();i++)
	// 		{
	// 			HWND hwnduse = (HWND)_tcstoul(pList->GetItemText(i,0), NULL, 16);
	// 			::SendMessageA(hwnduse,_dwCode,_wParam,_lParam);
	// 		}
	// 		

	//if (_dwCode == HC_ACTION)
	//{
	//	PEVENTMSG  msg = (PEVENTMSG )_lParam;

	//	HWND hwndmain = (HWND)_tcstoul(pList->GetItemText(0,0), NULL, 16);

	//	if (msg->hwnd != hwndmain)
	//	{
	//		HWND hwndnow = GetForegroundWindow();
	//		DWORD thrid,pid;
	//		thrid = GetWindowThreadProcessId(hwndnow,&pid);

	//		DWORD pid2 = (DWORD)_tcstoul(pList->GetItemText(0,1), NULL, 16);

	//		if (pid2 != pid) return NULL;

	//		hwndnow = GetFocus();

	//		if (hwndnow != hwndmain) return NULL;
	//	}

	//	if (msg->message >= 0x200 && msg->message <= 0x209)
	//	{
	//		POINT pi;
	//		::GetCursorPos(&pi);
	//		::ScreenToClient(hwndmain,&pi);
	//		LPARAM newl = MAKELPARAM(pi.x,pi.y);
	//		for (int i = 1;i < pList->GetItemCount();i++)
	//		{
	//			HWND hwnduse = (HWND)_tcstoul(pList->GetItemText(i,0), NULL, 16);
	//			::SendMessageA(hwnduse,msg->message,0,newl);
	//		}
	//	}
	//	// 		else if (msg->message == WM_KEYDOWN)
	//	// 		{
	//	// 			USHORT shift;
	//	// 			DWORD uScanCode,num;
	//	// 			BYTE szKeyState[256];
	//	// 			BYTE szAscii[32];
	//	// 
	//	// 			GetKeyboardState(szKeyState);
	//	// 			shift = GetKeyState(VK_SHIFT);
	//	// 			shift &= 0x00FF;
	//	// 			szKeyState[VK_SHIFT] = (BYTE)shift;
	//	// 			uScanCode = msg->paramH >> 16;
	//	// 			num = ToAscii(msg->paramL,uScanCode,szKeyState,(LPWORD)szAscii,0);
	//	// 			szAscii[num] = 0;
	//	// 			OutputDebugString((char *)szAscii);
	//	// 			for (int i = 1;i < pList->GetItemCount();i++)
	//	// 			{
	//	// 				HWND hwnduse = (HWND)_tcstoul(pList->GetItemText(i,0), NULL, 16);
	//	// 				::PostMessageA(hwnduse,msg->message,msg->paramL,0x1c0001);
	//	// 			}	
	//	// 		}
	//}

	return NULL;
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
	/*int nLastError=0;
	if (hookid == 0)
	{
		ui.pushButton->setText("ֹͣͬ��");
		HMODULE curModle = NULL;
		curModle = GetModuleHandle(NULL);
		hookid = ::SetWindowsHookEx(WH_JOURNALRECORD,HOOKPROC(&CallBackProc),curModle,0);
		nLastError =  ::GetLastError ();
	}
	else
	{
		ui.pushButton->setText("��ʼͬ��");
		UnhookWindowsHookEx(hookid);
		hookid = 0;
	}*/
}
//************************************/
//������:  clearTableItems
//���������������������ϵ
//��������
//����ֵ��bool
//ʱ�䣺2016/7/22 WZQ
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
			QWidget* tempWidget =  ui.tableWidget->cellWidget(i,nWidgetCol);//��checkBoxֵȡ����  ȥ�ͷ�
			if(tempWidget!=NULL)
				delete tempWidget;
		}
		ui.tableWidget->removeRow(i);               
	}
	ui.tableWidget->clearContents();	
	ui.tableWidget->setRowCount(1);
}