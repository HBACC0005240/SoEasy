// plantsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "plants.h"
#include "plantsDlg.h"
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
using namespace std;
std::map<DWORD, std::string> g_mlProcess;	//清风魔力的进程 定时轮询所有
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CplantsDlg 对话框




CplantsDlg::CplantsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CplantsDlg::IDD, pParent)
	, m_b_cd(FALSE)
	, m_u_money(0)
	, m_uSunlight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CplantsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_CD, m_b_cd);
	DDX_Text(pDX, IDC_EDIT_MONEY, m_u_money);
	DDX_Text(pDX, IDC_EDIT_SUNLIGHT, m_uSunlight);
}

BEGIN_MESSAGE_MAP(CplantsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_CD, &CplantsDlg::OnBnClickedCheckCd)
	ON_EN_CHANGE(IDC_EDIT_MONEY, &CplantsDlg::OnEnChangeEditMoney)
	ON_WM_TIMER()
	ON_EN_SETFOCUS(IDC_EDIT_MONEY, &CplantsDlg::OnEnSetfocusEditMoney)
	ON_BN_CLICKED(IDC_BUTTON_EDITMONEY, &CplantsDlg::OnBnClickedButtonEditmoney)
	ON_EN_KILLFOCUS(IDC_EDIT_MONEY, &CplantsDlg::OnEnKillfocusEditMoney)
	ON_EN_CHANGE(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnChangeEditSunlight)
	ON_EN_SETFOCUS(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnSetfocusEditSunlight)
	ON_EN_KILLFOCUS(IDC_EDIT_SUNLIGHT, &CplantsDlg::OnEnKillfocusEditSunlight)
	ON_BN_CLICKED(IDC_BUTTON_EDITSUNLIGHT, &CplantsDlg::OnBnClickedButtonEditsunlight)
	ON_BN_CLICKED(IDC_BUTTON_PLANT, &CplantsDlg::OnBnClickedButtonPlant)
END_MESSAGE_MAP()


// CplantsDlg 消息处理程序

BOOL CplantsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    SetTimer(101,1000,NULL);//金钱
	 SetTimer(102,1000,NULL);//阳光
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CplantsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CplantsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CplantsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//提升进程访问权限   
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
HANDLE GetGameProcessHanlde()
{
//	HWND h = ::FindWindow(NULL, L"植物大战僵尸中文版");
//	if (h == 0)
//	{
//		TRACE("游戏未打开");
//		return NULL;
//	}
//	//1005194
//	DWORD pid;
//	GetWindowThreadProcessId(h,&pid);
	//GetWindowThreadProcessId
	//OpenProcss
	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
	HANDLE hp=OpenProcess(PROCESS_ALL_ACCESS,false,16152);
	return hp;
	//
}
void CplantsDlg::OnBnClickedCheckCd()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);//更新窗口状态至变量
	UCHAR buf[2];
	DWORD bywrite;
    HANDLE hp=GetGameProcessHanlde();
	if (hp==NULL)
	{
		::MessageBox(0,L"打开进程出错",0,MB_OK);
		return;
	}
	//读取数据 
	
	if (m_b_cd)
	{   //禁用掉冷却时间
		//00487296 - 7e 14                      - jle 004872ac //nop nop  //去掉冷却时间
		//	90 90
		buf[0]=0x90;
		buf[1]=0x90;
	}else
	{   //启用冷却时间
		//0487296 - 7e 14  
		buf[0]=0x7e;
		buf[1]=0x14;
	}
	//把buf内数据写入 0x0487296
	WriteProcessMemory(hp,(LPVOID)0x0487296,buf,sizeof(buf),&bywrite);
	CloseHandle(hp);//释放掉句柄
}

void CplantsDlg::OnEnChangeEditMoney()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CplantsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	CString s;
   switch (nIDEvent)
   {
   case 101: //更新金币数量至 IDC_EDIT_MONEY
	   
      //dd  [[6A9EC0]+82c]+28 
	   //6A9EC0
	   
	   //[[6A9EC0]+82c]+28 
	   ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	   ////[6A9EC0]+82c]
	   ReadProcessMemory(hp,(PVOID)(buf+0x82c),&buf,sizeof(buf),&byread);
	   ReadProcessMemory(hp,(PVOID)(buf+0x28),&buf,sizeof(buf),&byread);
	   m_u_money=buf;
	   
	   s.Format(L"%d",buf);
	   GetDlgItem(IDC_EDIT_MONEY)->SetWindowText(s);
	   break;
   case 102:
	   //更新阳光数组至窗口
	  // dd  [[[[6A9EC0]+768]+144]+2c]+5560 
ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x768),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x144),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x2c),&buf,sizeof(buf),&byread);
ReadProcessMemory(hp,(PVOID)(buf+0x5560),&buf,sizeof(buf),&byread);
        m_uSunlight=buf;	
		
		s.Format(L"%d",buf);
GetDlgItem(IDC_EDIT_SUNLIGHT)->SetWindowText(s);
	   
	   break;
   }
     // UpdateData(false);//把数值更新显示到窗口
   CloseHandle(hp);
	CDialog::OnTimer(nIDEvent);
}

void CplantsDlg::OnEnSetfocusEditMoney()
{
	// TODO: 在此添加控件通知处理程序代码
	//当编辑框 获取焦点时 禁用定时器
	KillTimer(101);
}

void CplantsDlg::OnBnClickedButtonEditmoney()
{
	// TODO: 在此添加控件通知处理程序代码
	//修改金币
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	//dd  [[6A9EC0]+82c]+28 
	//6A9EC0

	//[[6A9EC0]+82c]+28 
	ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	////[6A9EC0]+82c]
	ReadProcessMemory(hp,(PVOID)(buf+0x82c),&buf,sizeof(buf),&byread);
	UpdateData(TRUE);//把窗口数据更新至变量
	 
	WriteProcessMemory(hp,(PVOID)(buf+0x28),&m_u_money,sizeof(buf),&byread);
	
	//UpdateData(false);//把数值更新显示到窗口

	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnKillfocusEditMoney()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(101,1000,NULL);
}

void CplantsDlg::OnEnChangeEditSunlight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CplantsDlg::OnEnSetfocusEditSunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(102);
}

void CplantsDlg::OnEnKillfocusEditSunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTimer(102,1000,NULL);
}

void CplantsDlg::OnBnClickedButtonEditsunlight()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	HANDLE hp=GetGameProcessHanlde();
	DWORD buf=0,byread,bywrite;
	ReadProcessMemory(hp,(PVOID)0x6A9EC0,&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x768),&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x144),&buf,sizeof(buf),&byread);
	ReadProcessMemory(hp,(PVOID)(buf+0x2c),&buf,sizeof(buf),&byread);
	
	WriteProcessMemory(hp,(PVOID)(buf+0x5560),&m_uSunlight,sizeof(buf),&byread);
	CloseHandle(hp);
}


//void plantOne(DWORD x,DWORD y)
//{
//	DWORD xy[2];
//	xy[0]=x;//0..7
//	xy[1]=y;// 0..4
//	DWORD byWrite;
//	//游戏进程句柄
//	HANDLE hp=GetGameProcessHanlde();
//	//在目标进程分配内存空间 以方便写入要执行的代码
//	PVOID FarCall=VirtualAllocEx(hp,NULL,0x8FFF,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
//	PVOID CallArg=VirtualAllocEx(hp,NULL,sizeof(int)*2,MEM_COMMIT,PAGE_READWRITE);
//	//向目标进程的 目标地址写入我们要执行的代码 
//	WriteProcessMemory(hp,FarCall,plant1,0x8FFF,&byWrite);
//	//向目标进程 写入参数
//	WriteProcessMemory(hp,CallArg,xy,sizeof(DWORD)*2,&byWrite);
//	//在目标进程 指定地址 执行代码
//	TRACE("\n addr=%x \n",FarCall);
//	HANDLE th=CreateRemoteThread(hp,NULL,NULL,( LPTHREAD_START_ROUTINE)FarCall,CallArg,NULL,NULL);
//	WaitForSingleObject(th,0xFFFFFFF);//等待 ...
//	VirtualFreeEx(hp,FarCall,0x8FFF,MEM_DECOMMIT);
//	CloseHandle(th);
//	CloseHandle(hp);
//}
//
//void CplantsDlg::GetAllProcess(std::map<DWORD, std::wstring>& processInfo)
//{
//	PROCESSENTRY32 pe32;
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hProcessSnap == INVALID_HANDLE_VALUE)
//	{
//		//获取进程列表失败
//		return;
//	}
//	BOOL bMore = Process32First(hProcessSnap, &pe32);
//	while (bMore)
//	{
//		std::wstring exeName = pe32.szExeFile;
//		std::cout << exeName.c_str() << "\n";
//		if (exeName.find(L"qfmoli.exe") != string::npos)//qfmoli
//		{
//			processInfo[pe32.th32ProcessID] = exeName;
//		}
//		bMore = Process32Next(hProcessSnap, &pe32);
//	}
//	CloseHandle(hProcessSnap);
//}
//int ReadMemoryIntFromProcessID(DWORD processID, const char* szAddress)
//{
//	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
//	EnableDebugPriv();
//	HANDLE hModel = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
//	if (hModel == nullptr)
//	{
//		std::cout << "OpenProcess Failed!\n";
//		return 0;
//	}
//	int memoryData = 0;
//	bool bRet = ReadProcessMemory(hModel, pAddress, &memoryData, 4, 0);
//	return memoryData;
//}
//void WriteMemoryIntToProcess(DWORD hProcessID, const char* szAddress, int nVal)
//{
//	LPVOID pAddress = (LPVOID)strtoul(szAddress, NULL, 16);
//	EnableDebugPriv();
//	HANDLE hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, hProcessID);
//	WriteProcessMemory(hProcessHandle, pAddress, &nVal, 4, 0);
//}
//void moveToPoint(DWORD hProcessID, int x, int y)
//{
//	WriteMemoryIntToProcess(hProcessID, "008E543C", x);
//	WriteMemoryIntToProcess(hProcessID, "008E5440", y);
//	EnableDebugPriv();
//	std::cout << "moveToPoint" << x << "," << y << "\n";
//	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessID);
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044D000), NULL, 0, &tId);
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ..
//	CloseHandle(hRemoteThread);
//	CloseHandle(hTarget);
//	std::cout << "moveToPointSuccess\n";
//}
//POINT GetGamePersonCoordinate(DWORD hProcessID)
//{
//	POINT pointPerson;
//	pointPerson.x = ReadMemoryIntFromProcessID(hProcessID, "0092BD28");
//	pointPerson.y = ReadMemoryIntFromProcessID(hProcessID, "0092BD30");
//	return pointPerson;
//}
//enum MOVE_DIRECTION
//{
//	MOVE_DIRECTION_UP = 1,
//	MOVE_DIRECTION_DOWN = 5,
//	MOVE_DIRECTION_LEFT = 7,
//	MOVE_DIRECTION_RIGHT = 3,
//	MOVE_DIRECTION_LEFTUP = 0,
//	MOVE_DIRECTION_RIGHTUP = 2,
//	MOVE_DIRECTION_LEFTDOWN = 6,
//	MOVE_DIRECTION_RIGHTDOWN = 4,
//};
////貌似界面不显示 这边就无效
//void turn_about(DWORD hProcessID, int nDirection)
//{
//	//调用call方式
//	POINT points = GetGamePersonCoordinate(hProcessID);
//	switch (nDirection)
//	{
//	case MOVE_DIRECTION_UP:
//	{
//		points.x = points.x + 1;
//		points.y = points.y - 1;
//		break;
//	}
//	case MOVE_DIRECTION_DOWN:
//	{
//		points.x = points.x - 1;
//		points.y = points.y + 1;
//		break;
//	}
//	case MOVE_DIRECTION_LEFT:
//	{
//		points.x = points.x - 1;
//		points.y = points.y - 1;
//		break;
//	}
//	case MOVE_DIRECTION_RIGHT:
//	{
//		points.x = points.x + 1;
//		points.y = points.y + 1;
//	}
//	case MOVE_DIRECTION_LEFTUP:	points.y = points.y - 1; break;
//	case MOVE_DIRECTION_RIGHTUP:
//	{
//		points.x = points.x + 1;
//		break;
//	}
//	case MOVE_DIRECTION_LEFTDOWN:
//	{
//		points.x = points.x - 1;
//		break;
//	}
//	case MOVE_DIRECTION_RIGHTDOWN:
//	{
//		points.y = points.y + 1;
//		break;
//	}default:return;
//	}
//	std::cout << "turn_about " << points.x << ":" << points.y << " direction " << nDirection << "\n";
//	WriteMemoryIntToProcess(hProcessID, "00BC7B58", points.x);
//	WriteMemoryIntToProcess(hProcessID, "00BC7B5C", points.y);
//	EnableDebugPriv();
//	HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessID);
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hTarget, NULL, 0, (LPTHREAD_START_ROUTINE)(0x0044CCC0), NULL, 0, &tId);
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	CloseHandle(hRemoteThread);
//	CloseHandle(hTarget);
//	return;
//}
//int getPersionHP(DWORD processID)
//{
//	int nHP = ReadMemoryIntFromProcessID(processID, "00E2A418");
//	return nHP;
//}
//
//int getPersionMP(DWORD processID)
//{
//	int nMP = ReadMemoryIntFromProcessID(processID, "00E2A420");
//	return nMP;
//}
////ANSI转化成UNICODE
//static LPWSTR ANSITOUNICODE1(const char* pBuf)
//{
//	int lenA = lstrlenA(pBuf);
//	int lenW = 0;
//	LPWSTR lpszFile = nullptr;
//	lenW = MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, 0, 0);
//	if (lenW > 0)
//	{
//		lpszFile = SysAllocStringLen(0, lenW);
//		MultiByteToWideChar(CP_ACP, 0, pBuf, lenA, lpszFile, lenW);
//	}
//	return lpszFile;
//}
//#define MAX_MEMORY_TEXT_SIZE 10000
//char* ReadMemoryStrFromProcessID(DWORD processID, const char* szAddress, int nLen)
//{
//	EnableDebugPriv();
//	bool bRet = false;
//	LPCVOID pAddress = (LPCVOID)strtoul(szAddress, NULL, 16);
//	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
//	if (nLen > MAX_MEMORY_TEXT_SIZE)
//		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
//	else
//		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), pAddress, pLen, nLen, 0);
//	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
//	return pLen;
//}
//char* ReadMemoryStrFromProcessID(DWORD processID, DWORD pAddress, int nLen)
//{
//	EnableDebugPriv();
//	bool bRet = false;
//	char pLen[MAX_MEMORY_TEXT_SIZE] = "";
//	if (nLen > MAX_MEMORY_TEXT_SIZE)
//		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, MAX_MEMORY_TEXT_SIZE, 0);
//	else
//		bRet = ReadProcessMemory(OpenProcess(PROCESS_ALL_ACCESS, 0, processID), (LPCVOID)pAddress, pLen, nLen, 0);
//	//	qDebug()<<processID << szAddress << pLen << bRet << pAddress;
//	return pLen;
//}
//std::string GetGameMapName(DWORD processID)
//{
//	string szMapName = string((ReadMemoryStrFromProcessID(processID, "009181C0", 100)));
//	return szMapName;
//}
//
//
//bool moveto(DWORD processID, int x, int y)
//{
//	int nTimeOut = 60000;//1分钟走不到 退出
//	POINT lastPoint = GetGamePersonCoordinate(processID);//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
//	while (1)
//	{
//		POINT gamePoint = GetGamePersonCoordinate(processID);
//		std::cout << "while(1)moveto" << gamePoint.x << "," << gamePoint.y << "destPoint" << x << "" << y << "\n";
//		if (gamePoint.x == x && gamePoint.y == y)
//		{
//			break;
//		}
//		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
//		{
//			moveToPoint(processID, x, y);
//		}
//		lastPoint = gamePoint;
//		Sleep(1000);
//	}
//	Sleep(100);//成功后 新坐标 等待一下
//	std::cout << "moveto" << x << "," << y << "success \n";
//	return true;
//}
//bool high(DWORD processID, int x, int y, int dstx, int dsty)
//{
//	int nTimeOut = 60000;//1分钟走不到 退出
//	POINT lastPoint = GetGamePersonCoordinate(processID);//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
//	while (1)
//	{
//		POINT gamePoint = GetGamePersonCoordinate(processID);
//		std::cout << "while(1)high" << gamePoint.x << "," << gamePoint.y << "destPoint" << x << "" << y << "\n";
//		if (gamePoint.x == dstx && gamePoint.y == dsty)
//		{
//			break;
//		}
//		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
//		{
//			moveToPoint(processID, x, y);
//		}
//		lastPoint = gamePoint;
//		Sleep(1000);
//	}
//	Sleep(100);//成功后 新坐标 等待一下
//	std::cout << "moveto" << x << "," << y << "success \n";
//	return true;
//}
//bool nowhile(DWORD processID, string szData)
//{
//	int nTryNum = 0;
//	while (1)//战斗中 等待
//	{
//		nTryNum++;
//		string szMap = GetGameMapName(processID);
//		if (szMap.find(szData) != string::npos)
//		{
//			std::cout << "nowhile";// << szData.c_str();
//			break;
//		}
//		Sleep(100);
//		if (nTryNum >= 100)
//			break;
//	}
//	return true;
//}
//__declspec(naked) void WorkCallArg(int* nindex)
//{
//	_asm
//	{
//		mov ebx, [esp + 4] //xy
//		mov eax, [ebx]
//		mov ecx, 0
//		mov edx, 0x00462EF8
//		call edx
//		ret
//	}
//}
//__declspec(naked)  void WorkCall()
//{
//	_asm
//	{
//		mov eax, 0
//		mov ecx, 0
//		mov edx, 0x00462EF8
//		call edx
//		ret
//	}
//}
//void Work(DWORD processID, int nIndex)
//{
//	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
//	DWORD nThreadSize = 0x8FFF;
//	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
//	if (callBase == nullptr)
//	{
//		printf("提示：申请空间失败\n");
//		return;
//	}
//	LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);
//	//	DWORD oldProtect = 0;
//	//	VirtualProtectEx(hProcess, callBase, nThreadSize, PAGE_EXECUTE_READWRITE, &oldProtect);//设置具有读写权限
//	for (int i = 0; i < 15; ++i)//先10个  15个技能栏
//	{
//		DWORD pAddress = 0x00D84FEC;
//		DWORD offset = i * 0x49FC;
//		pAddress += offset;
//		std::string skillName = ReadMemoryStrFromProcessID(processID, pAddress, 100);
//		pAddress += 0x38;
//		int nShowIndex = i;//YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
//		if (skillName.find("挖矿") != string::npos)
//		{
//			nIndex = i;
//			std::cout << "挖矿 " << nIndex << "\n";
//			break;
//		}
//	}
//	
//	//std::cout << "transAssemble Success!" << buf << "\n";
//	//Sleep(1000);//太快？
//	DWORD byWrite;
//	//BYTE testData[100] = { 0x8B,0x5C ,0x24 ,0x04 ,0x8B ,0x03 ,0xB9 ,0x00 ,0x00 ,0x00 ,0x00 ,0xBA ,0xF8 ,0x2E ,0x46 ,0x00 ,0x8B ,0xC2 ,0xFF ,0xD0 ,0xC3 };//{ 0x66,0xbb,0x20,0x20,0x20,0x20,0x66,0x67,0x8b,0x03,0x66,0xb9,0x20,0x20,0x20,0x20,0x66,0xba,0xf8,0x2e,0x46,0x20,0x66,0xff,0xd2,0xc3};
//	//bool bRet = WriteProcessMemory(hProcess, callBase, (LPCVOID)testData, 100, &byWrite);
//	//int eRoCode = GetLastError();
//	//std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//	//HANDLE hRemoteThread1 = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, 0);
//
//	bool bRet = WriteProcessMemory(hProcess, callBase, WorkCallArg, nThreadSize, &byWrite);
//	if (bRet == false)
//	{
//		int eRoCode = GetLastError();
//		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
//		VirtualFreeEx(hProcess, callArg, 0x8FFF, MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	bRet = WriteProcessMemory(hProcess, callArg, &nIndex, sizeof(int), &byWrite);
//	if (bRet == false)
//	{
//		std::cout << "erorCode:" << GetLastError() << "提示：写入参数失败\n";
//		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
//		VirtualFreeEx(hProcess, callArg, sizeof(int), MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, callArg, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		printf("提示：远程调用代码失败\n");
//		VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
//		VirtualFreeEx(hProcess, callArg, sizeof(DWORD), MEM_DECOMMIT);
//		CloseHandle(hRemoteThread);
//		CloseHandle(hProcess);
//		return;
//	}
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	VirtualFreeEx(hProcess, callBase, 0x8FFF, MEM_DECOMMIT);
//	VirtualFreeEx(hProcess, callArg, sizeof(DWORD), MEM_DECOMMIT);
//	CloseHandle(hRemoteThread);
//	CloseHandle(hProcess);
//	std::cout << "Work Success !\n";
//
//}
//
//struct handle_data {
//	unsigned long process_id;
//	HWND best_handle;
//};
//BOOL IsMainWindow(HWND handle)
//{
//	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
//}
//
//BOOL CALLBACK  EnumWindowsCallback(HWND handle, LPARAM lParam)
//{
//	handle_data& data = *(handle_data*)lParam;
//	unsigned long process_id = 0;
//	GetWindowThreadProcessId(handle, &process_id);
//	if (data.process_id != process_id || !IsMainWindow(handle)) {
//		return TRUE;
//	}
//	data.best_handle = handle;
//	return FALSE;
//}
//
//HWND FindMainWindow(unsigned long process_id)
//{
//	handle_data data;
//	data.process_id = process_id;
//	data.best_handle = 0;
//	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
//	return data.best_handle;
//}
//
//static POINT g_defResumeHMP = { 302, 174 };	//恢复hp mp
//static POINT g_defResumePetHMP = { 285, 252 };	//恢复宠物hp mp
//static POINT g_defYes = { 250, 315 };	//是
//static POINT g_defNo = { 383, 315 };	//否
//static POINT g_defOk = { 313, 319 };	//确定
//void selectRenew(DWORD processID)
//{
//	HWND gameHwnd = FindMainWindow(processID);
//	POINT points;
//	points.x = g_defResumeHMP.x;
//	points.y = g_defResumeHMP.y;
//	//ClientToScreen(m_gameHwnd, &points);
//		//YunLai::SetFocusToWnd(m_gameHwnd);//已经是焦点了 不需要
//	LPARAM newl = MAKELPARAM(points.x, points.y);
//	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
//	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
//	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
//	Sleep(1500);//GetDoubleClickTime());
//	points.x = g_defYes.x;
//	points.y = g_defYes.y;
//	newl = MAKELPARAM(points.x, points.y);
//	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
//	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
//	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
//	Sleep(1500);//GetDoubleClickTime());
//	points.x = g_defOk.x;
//	points.y = g_defOk.y;
//	newl = MAKELPARAM(points.x, points.y);
//	SendMessage(gameHwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, newl);
//	SendMessage(gameHwnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, newl);
//	SendMessage(gameHwnd, WM_LBUTTONUP, WM_LBUTTONUP, newl);
//}
//__declspec(naked) void SelectBuMpAndHp()
//{
//	_asm
//	{
//		mov ecx, 0xA0000000
//		push ecx
//		push 0
//		push 0
//		push 0
//		mov edx, 0x00479900
//		call edx
//		add esp, 0x10
//		ret
//	}
//}
//__declspec(naked) void SelectYes()
//{
//	_asm
//	{
//		push 0
//		push 0
//		push - 1
//		push 4
//		mov edx, 0x00479900
//		call edx
//		add esp, 0x10
//		ret
//	}
//}
//void remoteCall(DWORD processID, void* pCall)
//{
//	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
//	DWORD nThreadSize = 0x400;
//	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
//	if (callBase == nullptr)
//	{
//		printf("提示：申请空间失败\n");
//		return;
//	}
//	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
//	DWORD byWrite;
//	bool bRet = WriteProcessMemory(hProcess, callBase, pCall, nThreadSize, &byWrite);
//	if (bRet == false)
//	{
//		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		printf("提示：远程调用代码失败\n");
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hRemoteThread);
//		CloseHandle(hProcess);
//		return;
//	}
//	//WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	//VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//	//CloseHandle(hRemoteThread);
//	//CloseHandle(hProcess);
//	std::cout << "remoteCall Success !\n";
//}
//void selectMpAndHp(DWORD processID)
//{
//	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
//	DWORD nThreadSize = 0x8FFF;
//	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
//	if (callBase == nullptr)
//	{
//		printf("提示：申请空间失败\n");
//		return;
//	}
//	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
//	DWORD byWrite;
//	bool bRet = WriteProcessMemory(hProcess, callBase, SelectBuMpAndHp, nThreadSize, &byWrite);
//	if (bRet == false)
//	{
//		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		printf("提示：远程调用代码失败\n");
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hRemoteThread);
//		CloseHandle(hProcess);
//		return;
//	}
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//	CloseHandle(hRemoteThread);
//	CloseHandle(hProcess);
//	std::cout << "remoteCall Success !\n";
//}
//
//void selectDlgYes(DWORD processID)
//{
//	EnableDebugPriv();//提升进程权限 VS2008以后的版本才需要
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
//	DWORD nThreadSize = 0x400;
//	LPVOID callBase = VirtualAllocEx(hProcess, nullptr, nThreadSize, MEM_COMMIT /*| MEM_RESERVE*/, PAGE_EXECUTE_READWRITE);
//	if (callBase == nullptr)
//	{
//		printf("提示：申请空间失败\n");
//		return;
//	}
//	//LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(DWORD), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);	
//	DWORD byWrite;
//	bool bRet = WriteProcessMemory(hProcess, callBase, SelectYes, nThreadSize, &byWrite);
//	if (bRet == false)
//	{
//		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)callBase, nullptr, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		printf("提示：远程调用代码失败\n");
//		VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hRemoteThread);
//		CloseHandle(hProcess);
//		return;
//	}
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
//	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//	CloseHandle(hRemoteThread);
//	CloseHandle(hProcess);
//	std::cout << "remoteCall Success !\n";
//}
//bool renew(DWORD processID, int direction)
//{
//	turn_about(processID, direction);
//	int nHp = getPersionHP(processID);
//	int nMp = getPersionMP(processID);
//	Sleep(1500);
//	//	selectMpAndHp(processID);
//	remoteCall(processID, SelectBuMpAndHp);//选补血魔
//	Sleep(1500);
//	//	selectDlgYes(processID);
//	remoteCall(processID, SelectYes);//选是
////	selectRenew(processID);
//	int nRenewHp = getPersionHP(processID);
//	int nRenewMp = getPersionMP(processID);
//	if (nHp == nRenewHp && nMp == nRenewMp)
//	{
//		renew(processID, direction);//递归
//	}
//	std::cout << "renew success \n";
//	return true;
//}
//#define RNEW_MP 100000
//void StartWork(DWORD processID)
//{
//	int nMp = getPersionMP(processID);
//	std::cout << "MP" << nMp << " \n";
//	if (nMp < RNEW_MP)					// 魔无回城
//	{
//		POINT characterPoint = GetGamePersonCoordinate(processID);
//		//2个位置  在挂机点，在医院  
//		if (characterPoint.x == 221 && characterPoint.y == 84)
//		{
//			high(processID, 221, 83, 12, 42);
//			nowhile(processID, ("城东医院"));
//			moveto(processID, 12, 35);
//			moveto(processID, 6, 35);
//			moveto(processID, 6, 34);
//			Sleep(2000);
//			//			 turn_about(processID, 0);// 
//			//			 Sleep(1000);
//			renew(processID, 0);			// 恢复人宠
//			Sleep(1000);
//			moveto(processID, 6, 35);
//			moveto(processID, 12, 35);
//			moveto(processID, 12, 41);
//			high(processID, 12, 42, 221, 83);
//			nowhile(processID, ("法兰城"));
//			moveto(processID, 221, 84);
//			StartWork(processID);
//			Sleep(1000);
//		}
//	}
//	else
//	{
//		std::cout << "begin Work" << nMp << " \n";
//
//		Work(processID, 102);
//		Sleep(1000);
//	}
//}
__declspec(naked) void plant1(DWORD *pxy)
{
	//__asm
	//{ //esp+4
	//	     
 //           mov ebx,[esp+4] //xy
	//		mov ecx,[ebx]  //x
	//		mov edx,[ebx+4] //y
	//		push -1
	//		push 2
	//		push ecx //X列
	//		//mov ebx,[esp+4+0xc] //xy
	//		//mov ecx,[ebx]  //y
	//		//mov edx,[ebx+4] //y
	//		mov eax,dword ptr ds:[0x6a9ec0] //mov eax,0x6a9ec0
	//		mov eax,dword ptr ds:[eax+0x768]
	//		push eax
	//		mov eax,edx//Y行
	//		mov edx,0x0040D120
	//		call edx
	//		ret 
	//}
	_asm
	{
		mov ebx, [esp + 4] //xy
		mov eax, [ebx]
		mov ecx, 0
		mov edx, 0x00462EF8
		call edx
		ret
	}
}
void CplantsDlg::OnBnClickedButtonPlant()
{
	// TODO: 在此添加控件通知处理程序代码
	 
	/*for (int x=0;x<=8;x++)
	{
		for (int y=0;y<=4;y++)
		{
			plantOne(x,y);
		}
	}*/
	DWORD xy[2];
	xy[0] = 0;//0..7
	xy[1] = 1;// 0..4
	DWORD byWrite;
	//游戏进程句柄
	HANDLE hp = GetGameProcessHanlde();
	//在目标进程分配内存空间 以方便写入要执行的代码
	PVOID FarCall = VirtualAllocEx(hp, NULL, 0x8FFF, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PVOID CallArg = VirtualAllocEx(hp, NULL, sizeof(int) * 2, MEM_COMMIT, PAGE_READWRITE);
	//向目标进程的 目标地址写入我们要执行的代码 
	bool bRet = WriteProcessMemory(hp, FarCall, plant1, 0x8FFF, &byWrite);
	if (bRet == false)
	{
		int eRoCode = GetLastError();
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
	}
	//向目标进程 写入参数
	bRet = WriteProcessMemory(hp, CallArg, xy, sizeof(DWORD) * 2, &byWrite);
	if (bRet == false)
	{
		int eRoCode = GetLastError();
		std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
	}
	//在目标进程 指定地址 执行代码
	TRACE("\n addr=%x \n", FarCall);
	HANDLE th = CreateRemoteThread(hp, NULL, NULL, (LPTHREAD_START_ROUTINE)FarCall, CallArg, NULL, NULL);
	WaitForSingleObject(th, 0xFFFFFFF);//等待 ...
	VirtualFreeEx(hp, FarCall, 0x8FFF, MEM_DECOMMIT);
	CloseHandle(th);
	CloseHandle(hp);


}
